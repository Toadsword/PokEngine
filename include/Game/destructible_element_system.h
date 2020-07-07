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
// Date : 06.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <Game/ComponentManagers/player_manager.h>
#include <CoreEngine/engine.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/game_system.h>
#include <Ecs/Utility/entity_vector.h>

namespace poke::game {
/**
 * \brief System managing the collision of all destructible elements
*/
class DestructibleElementSystem final : public GameSystem {
public:
	explicit DestructibleElementSystem(Engine& engine, Game& game);
    void OnLoadScene();

    void OnUpdate();

    ~DestructibleElementSystem() = default;

	void OnTriggerEnter(const ecs::EntityIndex entityIndex, const  physics::Collision collision);
private:
	void Destruction(ecs::EntityIndex entityIndex, DestructibleElement destructibleElement);

    void OnEntityAddComponent(const ecs::EntityIndex entityIndex, const  ecs::ComponentMask component) override;
    void OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const  ecs::ComponentMask component) override;
	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
    void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;

	ecs::TransformsManager& transformsManager_;
    PlayerManager& playerManager_;
	DestructibleElementManager& destructibleElementManager_;
	ProjectileManager& projectileManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
    ecs::ParticleSystemsManager& particleSystemsManager_;
	MissilesManager& missilesManager_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;
	ecs::AudioSourcesManager& audioSourcesManager_;

	const float kParticleAsteroidMultiplier_ = 50;
	const float kParticleTimeAsteroid_ = 1.25f;
	const float kParticleTimeShip_ = 1.25f;
	const float kParticleTimeTurret_ = 1.25f;

	const std::size_t kMaxArchetypeDebrisNb_ = 1000;
	const std::size_t kDebrisNbPerAsteroids_ = 10;
	const float kDebrisLifeTime_ = 10;
	const float kDebrisSpeed_ = 25.0f;
	const float kDebrisSize_ = 0.25f;

	ecs::ArchetypeID debrisArchetypeId_;


	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kDebrisNbPerAsteroids_);
	ecs::EntityVector registeredEntities_ = ecs::EntityVector(kDebrisNbPerAsteroids_);
};
}//namespace poke::game