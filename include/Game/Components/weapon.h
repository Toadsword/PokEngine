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
#include <Ecs/ComponentManagers/interface_components_manager.h>
#include <Math/tranform.h>

namespace poke {
namespace game {
struct Weapon {
	Weapon() = default;
	~Weapon() = default;

	enum class WeaponType : uint8_t {
		PROJECTILE = 0u,
		PROJECTILE_PLAYER,
		MISSILE,
		MISSILE_PLAYER,
		LASER,
		SIZE
	};
	
	float shootCoolDown = 0.0f;
	std::vector<ecs::EntityIndex> gunPositions;
	WeaponType weaponType = WeaponType::PROJECTILE;
	float lightIntensity = 100.0f;
	float lightDuration = 4.0f;
	
	bool isShooting = false;
	ecs::EntityIndex origin = ecs::kNoEntity;
	bool isFiringMissile = false;
	std::vector<ecs::EntityIndex> targets;
	math::Vec3 shootDirection = { 0.0f };
	size_t activeGunID = 0u;
	float lastShootAt = -100.0f; //TODO(@Game) Rename it currentCooldown, it will be more accurate.

	bool operator==(const Weapon& other) const;
	bool operator!=(const Weapon& other) const;

	/**
	 * \brief Get a json with all the data from a weapon
	 */
    const json ToJson() const;

	/**
	 * \brief Set all the data from a json
	 */
    void SetFromJson(const json& weaponJson);
};
}//namespace ecs
}//namespace poke