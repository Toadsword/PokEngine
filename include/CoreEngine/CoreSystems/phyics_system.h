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
// Date : 01.02.20
//-----------------------------------------------------------------------------
#pragma once
#include <Ecs/system.h>
#include <PhysicsEngine/physics_engine.h>
#include <Ecs/ComponentManagers/rigidbody_manager.h>
#include <Ecs/ComponentManagers/colliders_manager.h>

namespace poke {
class PhysicsSystem final : public ecs::System {
public:
    explicit PhysicsSystem(Engine& engine);
	~PhysicsSystem() = default;

    void OnUnloadScene() override;
    void OnEntityDestroy(ecs::EntityIndex entityIndex) override;
    void OnEntityAddComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component) override;

    void OnEntitySetActive(ecs::EntityIndex entityIndex) override;
    void OnEntitySetInactive(ecs::EntityIndex entityIndex) override;
private:
    void OnPhysicUpdate();

	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	ecs::CollidersManager& collidersManager_;

	physics::PhysicsData physicsEngineData_;

	std::vector<ecs::EntityIndex> entities_;

	std::vector<ecs::EntityIndex> destroyedEntities_;
};
} //namespace poke
