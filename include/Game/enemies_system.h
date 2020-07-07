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
// Date : 25.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <Ecs/ComponentManagers/spline_followers_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/game_system.h>
#include <Ecs/Utility/entity_vector.h>

#include <Math/quaternion.h>

namespace poke {
namespace game {
class EnemiesSystem : public GameSystem {
public:
	EnemiesSystem(Engine& engine, Game& game);
	~EnemiesSystem() = default;

private:
	void OnUpdate();

	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;

	void OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component);
	void OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component);

	ecs::TransformsManager& transformsManager_;
	ecs::SplineFollowersManager& splineFollowersManager_;
	DestructibleElementManager& destructibleElementManager_;
	EnemiesManager& enemiesManager_;
	WeaponManager& weaponManager_;
    

	const size_t kEnemyNb_ = 75;
	inline static const size_t maxPlayerNb = 4;
	const float kLerpDirectionFactor_ = 1.0f;
	const float kMaxPercentage_ = 98.0f / 100.0f;
	const float kAugmentationPercentage_ = 2.0f / 100.0f;
	const float kLerpSpeedFactor_ = 0.25f;
	float projectileSpeed_ = 1;
	bool projectileSpeedIsSet_ = false;

	std::array<math::Vec3, maxPlayerNb> oldPlayerPos_;
	std::array<math::Vec3, maxPlayerNb> newPlayerPos_;
	std::array<math::Vec3, maxPlayerNb> playerDirections_;
	std::array<float, maxPlayerNb> playerSpeeds_;

	inline static const size_t futurePosNb = 10;
	const float kTimeBetweenCheck_ = 0.05f;
	const float kPercentageFullCheck = 99.8f / 100.0f;
	std::array<math::Vec3, futurePosNb> playerFuturePositions_;

	std::array<float, futurePosNb> projectileFutureDistances_;

	ecs::EntityVector enemyIndexes_ = ecs::EntityVector(kEnemyNb_);
	ecs::EntityVector enemySplineIndexes_ = ecs::EntityVector(kEnemyNb_);
	std::vector<ecs::EntityIndex> enemyToSetInactiveIndexes_;
	ecs::EntityVector players_ = ecs::EntityVector(maxPlayerNb);
    
};
}//namespace game
}//namespace poke