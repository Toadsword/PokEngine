#include <PhysicsEngine/physics_engine.h>

#include <Utility/log.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>

namespace poke {
namespace physics {

PhysicsEngine::PhysicsEngine() {}

void PhysicsEngine::OnPhysicUpdate()
{
    //Update all position
	pok_BeginProfiling(Update_position, 0);
    for (size_t i = 0; i < physicsEngineData_.entities.size(); i++) {
        math::Transform& t = physicsEngineData_.worldTransforms[i];
        t.SetLocalPosition(
            t.GetLocalPosition() + math::Vec3(
                physicsEngineData_.rigidbodies[i].linearVelocity * (1 / 60.0f
                )));
    }
	pok_EndProfiling(Update_position);

    //Find new collision
	pok_BeginProfiling(Compute_aabb, 0);

	std::vector<AABB> aabbs;
	aabbs.resize(physicsEngineData_.entities.size());
	for (size_t i = 0; i < physicsEngineData_.entities.size(); i++) {
		const auto transform = physicsEngineData_.worldTransforms[i];
		const auto collider = physicsEngineData_.colliders[i];
        aabbs[i] = collider.ComputeAABB(
			transform.GetLocalPosition(),
			transform.GetLocalScale(),
			transform.GetLocalRotation());
	}

	pok_EndProfiling(Compute_aabb);
	pok_BeginProfiling(Find_new_collision, 0);

    for (size_t i = 0; i < physicsEngineData_.entities.size(); i++) {
		const auto aabb = aabbs[i];

        for (size_t j = i + 1; j < physicsEngineData_.entities.size(); j++) {
            //If both overlap
            if (aabb.OverlapAABB(aabbs[j])) {
				const auto firstEntity = physicsEngineData_.entities[i];
				const auto secondEntity = physicsEngineData_.entities[j];

                auto alreadyExist = false;
                //Check if the collision already exist
                for (auto collision : collisionContacts_) {
                    if (collision.first == firstEntity && collision.second ==
                        secondEntity ||
                        collision.second == firstEntity && collision.first ==
                        secondEntity) {
                        alreadyExist = true;
                        break;
                    }
                }

                //Check if the trigger already exist
                for (auto trigger : triggerContacts_) {
                    if (trigger.first == firstEntity && trigger.second ==
                        secondEntity ||
                        trigger.second == firstEntity && trigger.first ==
                        secondEntity) {
                        alreadyExist = true;
                        break;
                    }
                }

                //If doesn't exit => Create a new collider/trigger and use callbacks
                if (!alreadyExist) {
					const auto collider = physicsEngineData_.colliders[i];
					const auto otherCollider = physicsEngineData_.colliders[j];

                    if (collider.isTrigger || otherCollider.isTrigger) {
                        triggerContacts_.emplace_back(firstEntity, secondEntity);
                        callbackNotifyOnTriggerEnter_(
                            firstEntity,
                            {secondEntity});
                        callbackNotifyOnTriggerEnter_(
                            secondEntity,
                            {firstEntity});
                    } else {
                        collisionContacts_.emplace_back(firstEntity, secondEntity);
                        callbackNotifyOnColliderEnter_(
                            firstEntity,
                            {secondEntity});
                        callbackNotifyOnColliderEnter_(
                            secondEntity,
                            {firstEntity});
                    }
                }
            }
        }
    }

	pok_EndProfiling(Find_new_collision);
	pok_BeginProfiling(Clear_previous_collision, 0);
	for (auto it = triggerContacts_.begin(); it != triggerContacts_.end(); ) {
		size_t first = -1;
		size_t second = -1;

		for (size_t i = 0; i < physicsEngineData_.entities.size(); i++) {
			auto entityIndex = physicsEngineData_.entities[i];
			if (it->first == entityIndex) {
				first = i;
			}

			if (it->second == entityIndex) {
				second = i;
			}

			if (first != -1 && second != -1) {
				break;
			}
		}

		if (first == -1 || second == -1) {
			it = triggerContacts_.erase(it);
			continue;
		}

		if (!aabbs[first].OverlapAABB(aabbs[second])) {
			callbackNotifyOnTriggerExit_(it->first, { it->second });
			callbackNotifyOnTriggerExit_(it->second, { it->first });

			it = triggerContacts_.erase(it);
			continue;
		}
		++it;
	}

	for (auto it = collisionContacts_.begin(); it != collisionContacts_.end(); ) {

		size_t first = -1;
		size_t second = -1;

		for (size_t i = 0; i < physicsEngineData_.entities.size(); i++) {
			auto entityIndex = physicsEngineData_.entities[i];

			if (it->first == entityIndex) {
				first = i;
			}

			if (it->second == entityIndex) {
				second = i;
			}

			if (first != -1 && second != -1) {
				break;
			}
		}

		if (first == -1 || second == -1) {
			it = collisionContacts_.erase(it);
			continue;
		}

		if (!aabbs[first].OverlapAABB(aabbs[second])) {
			callbackNotifyOnColliderExit_(it->first, { it->second });
			callbackNotifyOnColliderExit_(it->second, { it->first });

			it = collisionContacts_.erase(it);
			continue;
		}
		++it;
	}
	pok_EndProfiling(Clear_previous_collision);
}

void PhysicsEngine::SetCallbackNotifyOnTriggerExit(
    const std::function<void(ecs::EntityIndex, Collision)>
    callbackNotifyOnTriggerExit)
{
    callbackNotifyOnTriggerExit_ = callbackNotifyOnTriggerExit;
}

void PhysicsEngine::SetCallbackNotifyOnColliderEnter(
    const std::function<void(ecs::EntityIndex, Collision)>
    callbackNotifyOnColliderEnter)
{
    callbackNotifyOnColliderEnter_ = callbackNotifyOnColliderEnter;
}

void PhysicsEngine::SetCallbackNotifyOnColliderExit(
    const std::function<void(ecs::EntityIndex, Collision)>
    callbackNotifyOnTriggerExit)
{
    callbackNotifyOnColliderExit_ = callbackNotifyOnTriggerExit;
}

void PhysicsEngine::SetPhysicsEngineData(
    const PhysicsData& physicsEngineData)
{
    physicsEngineData_ = physicsEngineData;
}

PhysicsData& PhysicsEngine::GetPhysicsEngineData()
{
    return physicsEngineData_;
}

void PhysicsEngine::ClearEntities(
    const std::vector<ecs::EntityIndex>& destroyedEntities)
{
    std::vector<std::pair<ecs::EntityIndex, ecs::EntityIndex>> contactToErase;
    contactToErase.reserve(collisionContacts_.size());
    for (auto collision : collisionContacts_) {
        auto found = false;
        for (const auto entityIndex : destroyedEntities) {
            if (collision.first == entityIndex || collision.second ==
                entityIndex) {
                found = true;
                break;
            }
        }

        if (found) { contactToErase.emplace_back(collision); }
    }

    for (const auto toRemove : contactToErase) {
        const auto it = std::find(
            collisionContacts_.begin(),
            collisionContacts_.end(),
            toRemove);
        collisionContacts_.erase(it);
    }

    contactToErase.clear();
    contactToErase.reserve(triggerContacts_.size());
    for (const auto trigger : triggerContacts_) {
        auto found = false;
        for (const auto entityIndex : destroyedEntities) {
            if (trigger.first == entityIndex || trigger.second == entityIndex) {
                found = true;
                break;
            }
        }

        if (found) { contactToErase.emplace_back(trigger); }
    }

    for (auto toRemove : contactToErase) {
        const auto it = std::find(
            triggerContacts_.begin(),
            triggerContacts_.end(),
            toRemove);
        triggerContacts_.erase(it);
    }
}

void PhysicsEngine::ClearCollisions()
{
    collisionContacts_.clear();
    triggerContacts_.clear();
}

std::vector<ecs::EntityIndex> PhysicsEngine::Raycast(
    const math::Vec3 origin,
    const math::Vec3 direction,
    const float maxDistance)
{
    const auto destination = origin + direction.Normalize() * maxDistance;

    return Raycast(origin, destination);
}

std::vector<ecs::EntityIndex> PhysicsEngine::Raycast(
    const math::Vec3 origin,
    const math::Vec3 destination)
{
    std::vector<ecs::EntityIndex> contacts;
    for (size_t i = 0; i < physicsEngineData_.entities.size(); i++) {
        const auto collider = physicsEngineData_.colliders[i];
        const auto transform = physicsEngineData_.worldTransforms[i];

        const auto aabb = collider.ComputeAABB(
            transform.GetLocalPosition(),
            transform.GetLocalScale(),
            transform.GetLocalRotation());

        if (TestIntersectionSegmentAABB(origin, destination, aabb)) {
            contacts.push_back(physicsEngineData_.entities[i]);
        }
    }

    return contacts;
}

void PhysicsEngine::SetCallbackNotifyOnTriggerEnter(
    const std::function<void(ecs::EntityIndex, Collision)>
    callbackNotifyOnTriggerEnter)
{
    callbackNotifyOnTriggerEnter_ = callbackNotifyOnTriggerEnter;
}

void PhysicsEngine::BroadPhase() { }

void PhysicsEngine::NarrowPhase() { }

bool PhysicsEngine::TestIntersectionSegmentAABB(
    const math::Vec3 segmentOrigin,
    const math::Vec3 segmentDestination,
    const AABB aabb)
{
    float st, et, fst = 0, fet = 1;

    for (int i = 0; i < 3; i++) {
        const float aabbMin = aabb.worldPosition[i] - aabb.worldExtent[i];
        const float aabbMax = aabb.worldPosition[i] + aabb.worldExtent[i];

        const float originCoord = segmentOrigin[i];
        const float endCoord = segmentDestination[i];
        if (originCoord < endCoord) {
			if (originCoord > aabbMax || endCoord < aabbMin) {
				return false;
			}
            const auto direction = endCoord - originCoord;
            st = originCoord < aabbMin ?
                     (aabbMin - originCoord) / direction :
                     0;
            et = endCoord > aabbMax ? (aabbMax - originCoord) / direction : 1;
        } else {
			if (endCoord > aabbMax || originCoord < aabbMin) {
				return false;
			}

            const float direction = endCoord - originCoord;
            st = originCoord > aabbMax ?
                     (aabbMax - originCoord) / direction :
                     0;
            et = endCoord < aabbMin ? (aabbMin - originCoord) / direction : 1;
        }

		if (st > fst) {
			fst = st;
		}
		if (et < fet) {
			fet = et;
		}
		if (fet < fst) {
			return false;
		}
    }

    return true;
}
} //namespace physics
} //namespace poke
