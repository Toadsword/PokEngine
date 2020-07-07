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
// Author : Cédric Chambaz
// Date : 10.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <CoreEngine/engine.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/player_manager.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <Utility/time_custom.h>
#include <Game/game_system.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Ecs/Utility/entity_vector.h>

#include <Game/Ecs/game_archetype.h>

namespace poke::game {
/**
 * \brief Central class for the weapons management
*/
class WeaponSystem final : public GameSystem {
public:
	explicit WeaponSystem(Engine& engine, Game& game);

	~WeaponSystem() = default;

	void OnUpdate();

	void OnLoadScene() override;
	
	void Shoot(const ecs::EntityIndex entityIndex);
private:
	void ShootProjectile(const ecs::EntityIndex entityIndex);
	void ShootMissile(const ecs::EntityIndex entityIndex);
	void UpdateLight(ecs::EntityIndex entityIndex) const;
    void OnAddComponent(
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component);

    void OnRemoveComponent(
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component);

	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;

	Time& time_;
	WeaponManager& weaponManager_;
	ProjectileManager& projectileManager_;
	MissilesManager& missilesManager_;
	PlayerManager& playerManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	ecs::ModelsManager& modelsManager_;
	ecs::LightsManager& lightsManager_;
	ecs::CollidersManager& collidersManager_;
	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	GamePrefabsManager& gamePrefabsManager_;
	ecs::AudioSourcesManager& audioSourcesManager_;

	const size_t kWeaponNb_ = 100;

	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kWeaponNb_);
	
	ecs::EntityPool projectilePool_;
	ecs::EntityPool missilePool_;

	const size_t kProjectilePlayerNb_ = 20;
	const size_t kProjectileNb_ = 200;
	const size_t kMissilePlayerNb_ = 100;
	const size_t kMissileNb_ = 100;

	const std::string kMissilePlayerName_ = "MissilePlayer";
	const std::string kMissileName_ = "Missile";
	const std::string kProjectilePlayerName_ = "ProjectilePlayer";
	const std::string kProjectileName_ = "Projectile";

	GameArchetype archetypeProjectilePlayer_;
	GameArchetype archetypeProjectile_;
	GameArchetype archetypeMissilePlayer_;
	GameArchetype archetypeMissile_;

	ecs::ArchetypeID projectilePlayerId_;
	ecs::ArchetypeID projectileId_;
	ecs::ArchetypeID missilePlayerId_;
	ecs::ArchetypeID missileId_;

	bool archetypeAlreadyLoad_ = false;
};
}//namespace poke::game
