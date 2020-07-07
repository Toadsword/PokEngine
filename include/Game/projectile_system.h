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
// Author : Cedric Chambaz
// Date : 10.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <CoreEngine/engine.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <Utility/time_custom.h>
#include <Game/destructible_element_system.h>
#include <Game/game_system.h>
#include <Ecs/Utility/entity_vector.h>

namespace poke::game {
/**
 * \brief Central class for the projectiles management
*/
class ProjectileSystem final : public GameSystem {
public:
	explicit ProjectileSystem(Engine& engine, Game& game);
    void OnLoadScene();

    ~ProjectileSystem() = default;

	void OnUpdate();
private:

    void OnAddComponent(
        ecs::EntityIndex entityIndex,
        ecs::ComponentMask component);

    void OnRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component);

	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;

	void OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision);

    void DestroyProjectile(ecs::EntityIndex entityIndex, DestructibleElement::Type type = DestructibleElement::Type::SHIP);

    Time& time_;
	ProjectileManager& projectileManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	DestructibleElementManager& destructibleElementManager_;
	ecs::ParticleSystemsManager& particleSystemsManager_;
	graphics::ParticleSystem particleSystem_;
	const float kParticleTime_ = 1;

	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;

	const size_t kMaxProjectileNb_ = 100;

	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kMaxProjectileNb_);
	ecs::EntityVector registeredEntities_ = ecs::EntityVector(kMaxProjectileNb_);
};
}//namespace poke::game
