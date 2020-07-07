//-----------------------------------------------------------------------------
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
// Author : Nicolas Schneider
// Co-Author :
// Date : 24.02.20
//-----------------------------------------------------------------------------
#pragma once

#include <Ecs/ComponentManagers/transforms_manager.h>
#include <PhysicsEngine/collision.h>
#include <PhysicsEngine/collider.h>
#include <PhysicsEngine/rigidbody.h>

namespace poke::physics {

struct PhysicsData {
    std::vector<Collider> colliders;
    std::vector<Rigidbody> rigidbodies;
    std::vector<math::Transform> worldTransforms;
    std::vector<ecs::EntityIndex> entities;
};

/**
 * \brief Core class of the engine physics, in charge of containing all physics object that are not related to the ECS.
 */
class IPhysicsEngine {
public:
    virtual ~IPhysicsEngine() = default;

    /**
     * \brief Must be called one per fixed update
     */
    virtual void OnPhysicUpdate() = 0;

    /**
     * \brief Set the callback to call the Notifiy() of the OnTriggerEnter subject.
     * \param callbackNotifyOnTriggerEnter 
     */
    virtual void SetCallbackNotifyOnTriggerEnter(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnTriggerEnter) = 0;

    /**
    * \brief Set the callback to call the Notifiy() of the OnTriggerExit subject.
    * \param callbackNotifyOnTriggerExit
    */
    virtual void SetCallbackNotifyOnTriggerExit(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnTriggerExit) = 0;

    /**
    * \brief Set the callback to call the Notifiy() of the OnColliderEnter subject.
    * \param callbackNotifyOnColliderEnter
    */
    virtual void SetCallbackNotifyOnColliderEnter(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnColliderEnter) = 0;

    /**
    * \brief Set the callback to call the Notifiy() of the OnColliderExit subject.
    * \param callbackNotifyOnTriggerExit
    */
    virtual void SetCallbackNotifyOnColliderExit(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnTriggerExit) = 0;

    /**
     * \brief This function must be called before updating the physics engine. It's used to send all physics data from the world.
     * \param physicsEngineData 
     */
    virtual void SetPhysicsEngineData(const PhysicsData& physicsEngineData) = 0;

    /**
     * \brief Get all physics data updated from the physics engine.
     * \return 
     */
    virtual PhysicsData& GetPhysicsEngineData() = 0;

    /**
     * \brief This function must be called once a frame to clear all deleted entities.
     * \param destroyedEntities 
     */
    virtual void ClearEntities(
        const std::vector<ecs::EntityIndex>& destroyedEntities) = 0;

    /**
     * \brief Clear all contacts.
     */
    virtual void ClearCollisions() = 0;

    /**
	 * \brief Raycast from the origin along the direction up to the max distance.
	 * \param origin 
	 * \param direction 
	 * \param maxDistance 
	 * \return 
	 */
	virtual std::vector<ecs::EntityIndex> Raycast(math::Vec3 origin, math::Vec3 direction, float maxDistance) = 0;

    /**
	 * \brief Raycast from the origin to the destination.
	 * \param origin 
	 * \param destination 
	 * \return 
	 */
	virtual std::vector<ecs::EntityIndex> Raycast(math::Vec3 origin, math::Vec3 destination) = 0;
};
} //namespace poke::physics
