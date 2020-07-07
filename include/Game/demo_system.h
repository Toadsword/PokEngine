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
#include <Utility/time_custom.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/game_system.h>
#include <CoreEngine/Camera/interface_camera.h>

namespace poke::game {
/**
 * \brief Manage the main menu
*/
class DemoSystem final : public GameSystem {
public:
	explicit DemoSystem(Engine& engine, Game& game);

	~DemoSystem() = default;

	void OnLoadScene() override;
private:
	void InitializeScene();
	void CreateBoxes();
	void GenerateAsteroids();

	ICamera& camera_;
	Time& time_;
	PlayerManager& playerManager_;
	WeaponManager& weaponManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::SplineFollowersManager& splineFollowersManager_;
	GamePrefabsManager& gamePrefabsManager_;
	ecs::TrailRendererManager& trailRendererManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	EnemiesManager& enemiesManager_;
	JiggleManager& jiggleManager_;
	SplineStateManager& splineStateManager_;

	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	// Player parameters
	math::Vec3 playerPosOffset_ = { 0.0f,0.0f,10.0f };
	std::vector<ecs::EntityIndex> playerIndex_;;
	ecs::EntityIndex trailIndex_ = ecs::kNoEntity;

	ecs::EntityIndex cameraSplineFollower_ = ecs::kNoEntity;
	
	// Asteroid pool parameters
	std::vector<std::string> asteroidPrefabs_ = {
		"DestructibleA1",
		"DestructibleA2",
		"DestructibleA3",
		"DestructibleA4",
		"DestructibleA5",
		"DestructibleA6"
	};
	std::vector<ecs::EntityIndex> asteroidPool_;
	const size_t kAsteroidPoolSize_ = 3;
	const float kAsteroidPerBox = 3;
	const math::Vec3 kAsteroidBoxSize = { 50.0f };
	const math::Vec3 kAsteroidSpawnMaxPos_ = { 100.0f, 50.0f, 2000.0f };
	const math::Vec3 kAsteroidSpawnMinPos_ = { -100.0f, -50.0f, 20.0f };
	std::vector<math::Vec3> fields;
	
	// Asteroid parameters
	const float kAsteroidMaxSpeed_ = 6.0f;
	const float kAsteroidMinSpeed_ = 1.0f;
	const math::Vec3 kInteractibleAsteroidOffset_ = { 10.0f, 0.0f, 30.0f };

	// Asteroid spawn parameters
	const float kMaxSpawnDelay_ = 10.0f;
	float spawnDelay_ = 0.0f;
	float lastSpawn_ = 0.0f;

	// Other parameters
	const math::Vec3 kCameraBasePosition_ = { 100.0f, 50.0f, -1000.0f };
	const math::Vec3 kCameraBaseDirection_ = { 0.0f, 0.0f, 0.0f };
	std::string levelName_ = "VerticalSlice";
	
	bool systemIsRunning_ = false;

	bool isStraightLine = false;

	inline static const size_t kEnemyNb = 7;
	inline static const size_t kSplinePointNb = 12;

	const size_t kEnemyStartFireAt_ = 2;
	const size_t kEnemyStartFleeAt_ = 3;

	const float kEnemyApproachSpeed_ = 120.0f;
	const float kEnemyAttakSpeed_ = 20.0f;
	const float kEnemyFleeSpeed_ = 50.0f;
	
    const std::vector<std::vector<math::Vec3>> kEnemySplines_{
		{	// Top right enemy
			{64.5f, 79.2f, -30.f},
			{64.5f, 79.2f,  -23.6f},
			{64.5f, 79.2f, -125.0f},
			{64.5f, 79.2f, -500.0f},
			{64.5f, 79.2f, -950.0f},
			{64.5f, 79.2f, -1000.0f}
		},
        {	// Bottom right enemy
			{64.5f, 29.3f, -30.f},
			{64.5f, 29.3f,  -23.6f},
			{64.5f, 29.3f, -125.0f},
			{64.5f, 29.3f, -500.0f},
			{64.5f, 29.3f, -950.0f},
			{64.5f, 29.3f, -1000.0f}
		},
		{	// Bottom left enemy
			{131.5f, 29.3f, -30.f},
			{131.5f, 29.3f,  -23.6f},
			{131.5f, 29.3f, -125.0f},
			{131.5f, 29.3f, -500.0f},
			{131.5f, 29.3f, -950.0f},
			{131.5f, 29.3f, -1000.0f}
		},
		{	// Top left enemy
			{131.5f, 79.2f, -30.f},
			{131.5f, 79.2f,  -23.6f},
			{131.5f, 79.2f, -125.0f},
			{131.5f, 79.2f, -500.0f},
			{131.5f, 79.2f, -950.0f},
			{131.5f, 79.2f, -1000.0f}
		}
	};

	const size_t kDestroyerStartFireAt_ = 2;
	const size_t kDestroyerStartFleeAt_ = 3;

	const float kDestroyerApproachSpeed_ = 1.55f;
	const float kDestroyerAttakSpeed_ = 20.0f;
	const float kDestroyerFleeSpeed_ = 50.0f;
	
	const std::vector<std::vector<math::Vec3>> kDestroyerSplines_ {
		{	// Destroyer left
			{260.0f, 50.0f, 2000.0f},
			{250.0f, 50.0f, 2000.0f},
			{151.1f, 50.0f, 2200.0f},
			{151.1f, 50.0f, 2700.0f},
			{250.0f, 50.0f, 3000.0f},
			{260.0f, 50.0f, 3000.0f}
		},
		{	// Destroyer right
			{-64.0f, 50.0f, 2000.0f},
			{-54.0f, 50.0f, 2000.0f},
			{44.5f, 50.0f, 2200.0f},
			{44.5f, 50.0f, 2700.0f},
			{-54.0f, 50.0f, 3000.0f},
			{-64.0f, 50.0f, 3000.0f}
		}
	};

	const std::vector<poke::math::Vec3> kCameraSplineRotation_ = {
		{ 0.0f, 1.0f, 0.0f },			// 0
		{ 0.0f, 1.0f, 0.0f },			// 1
		{ 0.0f, 1.0f, 0.0f },			// 2
		{ 0.0f, 1.0f, 0.0f },			// 3
		{ 0.0f, 1.0f, 0.0f },			// 4
		{ 0.0f, 1.0f, 0.0f },			// 5
		{ 0.0f, 1.0f, 0.0f },			// 6
		{ 0.0f, 1.0f, 0.0f },			// 7
		{ 0.0f, 1.0f, 0.0f },			// 8
		{ 0.0f, 1.0f, 0.0f },			// 9
		{ 0.0f, 1.0f, 0.0f },			// 10
		{ 0.0f, 1.0f, 0.0f }			// 11
	};

	const std::vector<float> kCameraSplineLerpSpeed_ = {
		0.01f,		// 0
		0.01f,		// 1
		0.01f,		// 2
		0.05f,		// 3
		0.05f,		// 4
		0.05f,		// 5
		0.05f,		// 6
		0.05f,		// 7
		0.05f,		// 8
		0.05f,		// 9	
		0.05f,		// 10
		0.05f,		// 11
	};

	const std::vector<float> kCameraSplineMoveSpeed_ = {
		20.0f,		// 0
		20.0f,		// 1
		20.0f,		// 2
		20.0f,		// 3
		20.0f,		// 4
		20.0f,		// 5
		20.0f,		// 6
		20.0f,		// 7
		20.0f,		// 8
		20.0f,		// 9
		20.0f,		// 10
		20.0f,		// 11
	};
};
}//namespace poke::game
