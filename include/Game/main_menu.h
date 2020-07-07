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
// Date : 12.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <Inputs/input_manager.h>
#include <Utility/time_custom.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/game_system.h>
#include <CoreEngine/Camera/interface_camera.h>

namespace poke::game {
/**
 * \brief Manage the main menu
*/
class MainMenu final : public GameSystem {
public:
	explicit MainMenu(Engine& engine, Game& game);

	~MainMenu() = default;

	void OnLoadScene() override;

	void OnUpdate();
private:
	void SetupCamera();
	void InitAsteroidePool();
	void InstantiateAsteroide();
	void LoadLevel();
	void ShowOptions();
	void ShowCredits();

	ICamera& camera_;
	Time& time_;
	PlayerManager& playerManager_;
	WeaponManager& weaponManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	ecs::ModelsManager& modelsManager_;
	ecs::CollidersManager& collidersManager_;
	DestructibleElementManager& destructibleElementManager_;
	GamePrefabsManager& gamePrefabsManager_;

	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	// Player parameters
	math::Vec3 playerPosOffset_ = { 0.0f,0.0f,15.0f };
	
	// Asteroid pool parameters
	std::vector<std::string> asteroidPrefabs_ = {
		"DestructibleA1",
		"DestructibleA2",
		"DestructibleA3",
		"DestructibleA4",
		"DestructibleA5",
		"DestructibleA6",
	};
	std::vector<ecs::EntityIndex> asteroidPool_;
	const size_t kAsteroidPoolSize_ = 3;
	const math::Vec3 kAsteroidSpawnMaxPos_ = { 40.0f, 30.0f, 20.0f };
	const math::Vec3 kAsteroidSpawnMinPos_ = { 0.0f, 0.0f, 20.0f };

	// Asteroid parameters
	const float kAsteroidMaxSpeed_ = 6.0f;
	const float kAsteroidMinSpeed_ = 1.0f;
	const math::Vec3 kInteractibleAsteroidOffset_ = { 10.0f, 0.0f, 30.0f };

	// Asteroid spawn parameters
	const float kMaxSpawnDelay_ = 10.0f;
	float spawnDelay_ = 0.0f;
	float lastSpawn_ = 0.0f;

	// Other parameters
	const math::Vec3 kCameraBasePosition_ = { 20.0f, 15.0f, -5.0f };
	const math::Vec3 kCameraBaseDirection_ = { 20.0f, 15.0f, 10.0f };
	std::string levelName_ = "VerticalSlice";
	
	bool systemIsRunning_ = false;
};
}//namespace poke::game
