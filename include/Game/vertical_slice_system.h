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
#include <Game/game_system.h>

namespace poke::game {
/**
 * \brief Manage the main menu
*/
class VerticalSliceSystem final : public GameSystem {
public:
	explicit VerticalSliceSystem(Engine& engine, Game& game);

	~VerticalSliceSystem() = default;

	void OnLoadScene() override;

	void OnUpdate();
private:
	void InitializeScene();

	ICamera& camera_;
	Time& time_;
	PlayerManager& playerManager_;
	ecs::SplineFollowersManager& splineFollowersManager_;
	GamePrefabsManager& gamePrefabsManager_;
	ecs::TransformsManager transformManager_;
	poke::game::SplineStateManager& splineStateManager_;
	
	ecs::EntityIndex playerIndex = ecs::kNoEntity;
	ecs::EntityIndex splineIndex = ecs::kNoEntity;
	
	const std::vector<std::string> asteroidPrefabs_ = {
		"DecorA1",
		"DecorA2",
		"DecorA3",
		"DecorA4",
		"DecorA5",
		"DecorA6",
		"DestructibleA1",
		"DestructibleA2",
		"DestructibleA3",
		"DestructibleA4",
		"DestructibleA5",
		"DestructibleA6"
	};

	const float kTimeBeforeRestarting_ = 5.0f;
	float currentTimeBeforeRestarting = 0.0f;

	ecs::SplineFollower cameraSpline_;
	
	bool systemIsRunning_ = false;
	bool levelHasStarted = false;

	const std::vector<poke::math::Vec3> kCameraSplinePoints_ = {
		{ 0.0f,    210.0f, 0.0f },			// 0
		{ 0.0f,    200.0f, 0.0f },			// 1
		{ 0.0f,    -21.0f, 0.0f },			// 2
		{ 0.0f,   -156.0f, 6.75f },			// 3
		{ 0.0f,   -197.1f, 77.4f },			// 4
		{ -6.72f, -246.4f, 407.9f },		// 5
		{ -101.1f, -251.0f, 440.0f },		// 6
		{ -403.5f, -251.0f, 445.8f },		// 7
		{ -743.0f, -251.0f, 439.8f },		// 8
		{ -805.5f, -251.0f, 374.6f },		// 9
		{ -810.6f, -251.0f, 67.8f },		// 10
		{ -807.4f, -251.0f, -301.6f },		// 11
		{ -807.6f, -251.5f, -334.7f }		// 12
	};

	const std::vector<poke::math::Vec3> kCameraSplineRotation_ = {
		{ 0.0f, 0.0f, 0.0f },			// 0
		{ 0.0f, 0.0f, 1.0f },			// 1
		{ 0.0f, 0.0f, 1.0f },			// 2
		{ 0.0f, 0.0f, 1.0f },			// 3
		{ 0.0f, 1.0f, 0.0f },			// 4
		{ 0.0f, 1.0f, 0.0f },			// 5
		{ 0.0f, 1.0f, 0.0f },			// 6
		{ 0.0f, 1.0f, 0.0f },			// 7
		{ 0.0f, 1.0f, 0.0f },			// 8
		{ 0.0f, 1.0f, 0.0f },			// 9
		{ 0.0f, 1.0f, 0.0f },			// 10
		{ 0.0f, 1.0f, 0.0f },			// 11
		{ 0.0f, 1.0f, 0.0f },			//0 12
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
		0.05f		// 12
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
		20.0f		// 12
	};
};
}//namespace poke::game
