//----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Robin Alves
// Date : 19.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/ComponentManagers/transforms_manager.h>
#include <Ecs/ComponentManagers/rigidbody_manager.h>

#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Math/quaternion.h>
#include <Ecs/Utility/entity_vector.h>

#include <Game/game_system.h>

namespace poke {
namespace game {
class MissilesSystem : public GameSystem {
public:
	MissilesSystem(Engine& engine, Game& game);
    void OnLoadScene();
    ~MissilesSystem() = default;

private:
	void OnUpdate();

	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;

	void OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) override;
	void OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) override;

	void OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision);

	void MissileExplosion(const ecs::EntityIndex entityIndex);
	bool IsInFieldOfAim(const math::Vec3 missileDirection, const math::Vec3 targetDirection, const float maxAngle) const;

    ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
    MissilesManager& missilesManager_;
	DestructibleElementManager& destructibleElementManager_;
	ecs::ParticleSystemsManager& particleSystemsManager_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;
	const float kParticleTime_ = 5.0f;

	const size_t kMaxMissileNb_ = 201;
	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kMaxMissileNb_);
	ecs::EntityVector registeredEntities_ = ecs::EntityVector(kMaxMissileNb_);

	const float kMaxAngle_ = 90.0f;
	const float kLerpValue_ = 0.5f;

    //Missile update data optimisation
	std::vector<Missile> missiles_;
	std::vector<physics::Rigidbody> rigidbodies_;
	std::vector<math::Transform> transforms_;
	std::vector<math::Vec3> missileTargetDirections_;
	size_t flagEndData_;
};
}//namespace game
}//namespace poke