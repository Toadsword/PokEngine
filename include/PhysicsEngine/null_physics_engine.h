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

#include <PhysicsEngine/interface_physics_engine.h>

namespace poke::physics {

class NullPhysicsEngine : public IPhysicsEngine {
public:
    void OnPhysicUpdate() override {}

    void SetCallbackNotifyOnTriggerEnter(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnTriggerEnter) override {}

    void SetCallbackNotifyOnTriggerExit(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnTriggerExit) override {}

    void SetCallbackNotifyOnColliderEnter(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnColliderEnter) override {}

    void SetCallbackNotifyOnColliderExit(
        const std::function<void(ecs::EntityIndex, Collision)>
        callbackNotifyOnTriggerExit) override {}

    void SetPhysicsEngineData(const PhysicsData& physicsEngineData) override
    {
		physicsEngineData;
    }

    PhysicsData& GetPhysicsEngineData()
    {
        cassert(false, "Impossible to use NullPhysicsEngine");
		abort();
    }

    void ClearEntities(
        const std::vector<ecs::EntityIndex>& destroyedEntities) override
    {
		destroyedEntities;
    }

    void ClearCollisions() override {}

    std::vector<ecs::EntityIndex> Raycast(
        math::Vec3 origin,
        math::Vec3 direction,
		float maxDistance) override {
		maxDistance;
		direction;
		origin;
		return {};
	}


    std::vector<ecs::EntityIndex> Raycast(
        math::Vec3 origin,
        math::Vec3 destination) override
    {
		origin;
		destination;
		return {};
    }
};
} //namespace poke::physics
