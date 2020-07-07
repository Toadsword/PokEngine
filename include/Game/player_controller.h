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
// Date : 23.01.20
//----------------------------------------------------------------------------------

#pragma once
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/special_attack_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/game_system.h>
#include <Game/weapon_system.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <Inputs/input_manager.h>
#include <Ecs/Utility/entity_vector.h>

namespace poke::game {
/**
 * \brief Central class for the players management
*/
class PlayerController final : public GameSystem { //TODO(@Robin) Wrong naming convention all other system are called SomethingSystem (@Robin will do the raname when I am su there will be no trouble)
public:
	explicit PlayerController(Engine& engine, Game& game);
	void OnLoadScene() override;

	~PlayerController() = default;

	void OnUpdate();
private:
	void Move(const ecs::EntityIndex entityIndex);
	void Dash(const ecs::EntityIndex entityIndex) const;
	void ShootProjectile(const ecs::EntityIndex entityIndex, const bool isShooting) const;
	void ShootMissile(const ecs::EntityIndex entityIndex) const;
	void LockMissile(const ecs::EntityIndex entityIndex) const;
	void CheckPosition(const ecs::EntityIndex entityIndex);
	void TakeDamage(const ecs::EntityIndex entityIndex, Player& player, int damage) const;
	void SpecialCapacity(const ecs::EntityIndex entityIndex);
	void IncreaseSpecialCapacity(const ecs::EntityIndex entityIndex);
	void CleanSpecialCapacity(const ecs::EntityIndex entityIndex);

    void OnAddComponent(
        const ecs::EntityIndex entityIndex,
		const ecs::ComponentMask component);

	void OnRemoveComponent(
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask componentMask);

	void OnEntitySetActive(ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(ecs::EntityIndex entityIndex) override;

	void OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision);

	input::InputManager& inputManager_;
	PlayerManager& playerManager_;
	WeaponManager& weaponManager_;
	SpecialAttackManager& specialAttackManager_;
	ProjectileManager& projectileManager_;
	MissilesManager& missilesManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	ecs::ModelsManager& modelsManager_;
	DestructibleElementManager& destructibleElementManager_;

	const float kMaxJoyValue_ = 32768.0f;

	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;

	const float kBumpStrength_ = 250.0f;
	const size_t kMaxPlayerNb_ = 4;
	const math::Vec3 kMaxRotation_ = { 30.0f };
	const math::Vec3 kRotationSpeed_ = { 3.0f, 3.0f, 3.0f };
	/**
	 * \brief Represent the minimal Speed to be considered in movement
	 */
	const float kMinMoveSpeed_ = 0.5f;
	const int kDamageFromCollision_ = 5;
	const float kDashBumpDestructionCooldown = 3.0f;

	const float kDragSpeedDiminution_ = 7.0f;

	inline static const uint8_t maxPlayerNb = 4;
	std::array<math::Vec2, maxPlayerNb> currentSpeeds_;
	std::array<math::Vec3, maxPlayerNb> oldPositions_;

	bool archetypeAlreadyLoad_ = false;
	const std::string kTargetPrefabName_ = "TargetSight";
	const size_t kTargetNb_ = 200;
	GameArchetype archetypeTarget_;
	ecs::ArchetypeID targetId_;

	std::vector<ecs::EntityIndex> activeTargets_;
	std::vector<ecs::EntityIndex> entitiesInRange_;
	
	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kMaxPlayerNb_);
	ecs::EntityVector registeredEntities_ = ecs::EntityVector(kMaxPlayerNb_);
};
}//namespace poke::game
