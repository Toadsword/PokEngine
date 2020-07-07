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
// Co-Author :
// Date : 05.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <optional_custom.h>

// app systems part
#include <Game/player_controller.h>
#include <Game/destructible_element_system.h>
#include <Game/weapon_system.h>
#include <Game/projectile_system.h>
#include <Game/main_menu.h>
#include <Game/enemies_system.h>
#include <Game/missiles_system.h>
#include <Game/aiming_aid.h>
#include <Game/special_attack_system.h>
#include <Game/Systems/jiggle_system.h>
#include <Game/Systems/spline_state_system.h>
#include <Game/game_camera_system.h>
#include <Game/demo_system.h>
#include <Game/vertical_slice_system.h>

namespace poke {
namespace game {
class Game;
} //namespace game

struct AppSystemsContainer {
	AppSystemsContainer(Engine& engine, game::Game& game);
	void OnGameInit();

	std::experimental::optional<game::PlayerController> playerController = std::experimental::nullopt;
	std::experimental::optional<game::DestructibleElementSystem> destructibleElementManager = std::experimental::nullopt;
	std::experimental::optional<game::WeaponSystem> weaponController = std::experimental::nullopt;
	std::experimental::optional<game::ProjectileSystem> projectileController = std::experimental::nullopt;
	std::experimental::optional<game::MainMenu> mainMenu = std::experimental::nullopt;
	std::experimental::optional<game::EnemiesSystem> enemiesSystem = std::experimental::nullopt;
	std::experimental::optional<game::MissilesSystem> missilesSystem = std::experimental::nullopt;
	std::experimental::optional<game::AimingAidSystem> aimingAid = std::experimental::nullopt;
	std::experimental::optional<game::SpecialAttackSystem> specialAttackSystem = std::experimental::nullopt;
	std::experimental::optional<game::JiggleSystem> jiggleSystem = std::experimental::nullopt;
	std::experimental::optional<game::SplineStateSystem> splineStateSystem = std::experimental::nullopt;
	std::experimental::optional<game::GameCameraSystem> gameCameraSystem = std::experimental::nullopt;
	std::experimental::optional<game::DemoSystem> demoSystem = std::experimental::nullopt;
	std::experimental::optional<game::VerticalSliceSystem> verticalSliceSystem = std::experimental::nullopt;

private:
	Engine& engine_;
	game::Game& game_;
};
}//namespace poke