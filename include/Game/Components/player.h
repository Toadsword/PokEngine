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
#include <Math/vector.h>
#include <Ecs/ecs_utility.h>
#include "Math/tranform.h"

namespace poke {
namespace game {
struct Player {
	enum class PlayerState : uint8_t {
		NONE = 0u,
		DASHING,
		SIZE
	};

	enum class PlayerMode : uint8_t {
		STANDARD = 0u,
		BATTERING_RAM,
		SIZE
	};

	enum class LifeState : uint8_t {
		NONE = 0u,
		ONE,
		TWO,
		THREE,
		SIZE
	};

	Player() = default;
	~Player() = default;

	bool operator==(const Player& other) const;
	bool operator!=(const Player& other) const;

	// General stats
	int maxLife = 0;
	int life = 0;
	PlayerMode mode = PlayerMode::STANDARD;
	int score = 0;
	int destructionPoint = 0;
	int missileAmount = 0;
	size_t maxLockingTargets = 4;
	//ecs::EntityIndex aimIndex = ecs::kNoEntity;
	//ecs::EntityIndex lockedAimIndex = ecs::kNoEntity;
	ecs::EntityIndex cameraObjectIndex = ecs::kNoEntity;

	ecs::EntityIndex targetSightPositionOne = ecs::kNoEntity;
	ecs::EntityIndex targetSightPositionTwo = ecs::kNoEntity;
	ecs::EntityIndex targetSightPositionThree = ecs::kNoEntity;

	ecs::EntityIndex targetSightModelOne = ecs::kNoEntity;
	ecs::EntityIndex targetSightModelTwo = ecs::kNoEntity;
	ecs::EntityIndex targetSightModelThree = ecs::kNoEntity;
    

	// Special capacity values
	int specialCapacityAvailableAt = 500;
	float specialCapacityMaxRadius = 40000.0f;
	float currentSpecialCapacityRadius = 0.0f;
	float specialCapacityIncrementedBy = 10000.0f;

	// Movement values
	float moveSpeed = 0.0f;
	float maxMoveSpeed = 0.0f;

	// Dash values
	float dashSpeed = 0.0f;
	float dashDuration = 0.0f;
	float dashCooldown = 0.0f;

	// In game variable
	math::Vec2 moveDirection;
	PlayerState state = PlayerState::NONE;
	LifeState lifeState = LifeState::NONE;
	float dashStartedAt = 0.0f;
	float lastDashEndedAt = 0.0f;

	math::Vec2 movementArea = { 15.0f, 10.0f };
	ecs::EntityIndex missileWeapon = ecs::kNoEntity;
	ecs::EntityIndex projectileWeapon = ecs::kNoEntity;
	ecs::EntityIndex currentTarget = ecs::kNoEntity;
	std::vector<ecs::EntityIndex> lockingTargets;
	std::vector<ecs::EntityIndex> damageAreas;
	ecs::EntityIndex specialAttackIndex;
	math::Vec3 fireDirection = math::Vec3(0, 0, 1);
	math::Transform lockedAimBaseTransform;

	//Shield values
	float timeBeforeShieldRegen = 0.0f;
	float shieldRegenSpeed = 0.0f;
	float maxShieldLife = 0.0f;
	float shieldLife = 0.0f;
	ecs::EntityIndex shieldIndex = ecs::kNoEntity;
	ecs::EntityIndex dashSoundIndex = ecs::kNoEntity;
	ecs::EntityIndex hitSoundIndex = ecs::kNoEntity;

	float shieldRegenTimeStamp = 0.0f;

	bool isDashing = false;
	bool isPreparingSpecialAttack = false;

	/**
	 * \brief Get a json with all the data from the player
	 */
	json ToJson() const;

	/**
	 * \brief Set all the data from a json
	 */
    void SetFromJson(const json& playerJson);

	/*bool operator==(const Player& player);
	bool operator!=(const Player& player);*/
};
}//namespace ecs
}//namespace poke