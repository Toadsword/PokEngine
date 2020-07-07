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
// Co-Author :
// Date : 18.02.20
//----------------------------------------------------------------------------------

#pragma once 

#include <Utility/json_utility.h>
#include <Ecs/ecs_utility.h>
#include <Math/vector.h>

namespace poke {
namespace game {
struct Player;

struct Enemy {
	Enemy() = default;
	~Enemy() = default;

    enum class State {
        NONE = 0,
        STAND_BY,
        LAUNCH,
        APPROACH,
        ATTACK,
        FLEE,
        DYING,
        RESET,
        INACTIVE
    };

    //State
    State state = State::NONE;

    //Speed each part
    float approachSpeed = 0;
	float attackSpeed = 0;
	float fleeSpeed = 0;

    // Spline flag
	int splineFireStartPoint = 0;
	int splineFireEndPoint = 0;

	// Weapons
	std::vector<ecs::EntityIndex> weapons;
	ecs::EntityIndex enemyReactorIndex;

	bool operator==(const Enemy& other) const;
	bool operator!=(const Enemy& other) const;

	/**
     * \brief Get a json with all the data from an enemy
     */
	const json ToJson() const;

	/**
	 * \brief Set all the data from a json
	 */
	void SetFromJson(const json& destructibleElementJson);
};
}//namespace game
}//namespace poke