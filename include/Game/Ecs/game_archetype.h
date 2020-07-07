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
// Co-Author : Nicolas Schneider
// Date : 18.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/Archetypes/archetype.h>

//Game Components
#include <Game/Components/player.h>
#include <Game/Components/weapon.h>
#include <Game/Components/destructible_element.h>
#include <Game/Components/projectile.h>
#include <Game/Components/enemy.h>
#include <Game/Components/missile.h>
#include <Game/Components/special_attack.h>
#include <Game/Components/spline_states.h>
#include <Game/Components/jiggle.h>
#include <Game/Components/game_camera.h>

namespace poke {
namespace game {
class GameArchetype : public ecs::Archetype {
public:
    GameArchetype() = default;
    ~GameArchetype() = default;

    void SetFromJson(const json& archetypeJson) override;

    json ToJson() const override;

	Player player;
	Weapon weapon;
	SpecialAttack specialAttack;
	DestructibleElement destructibleElement;
	Projectile projectile;
	Enemy enemy;
	Missile missile;
	SplineStates splineStates;
	Jiggle jiggle;
	GameCamera gameCamera;
};
} //namespace game
} //namespace poke
