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
// Date : 18.03.20
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/ComponentManagers/transforms_manager.h>
#include <CoreEngine/Camera/core_camera.h>
#include <Game/ComponentManagers/spline_state_manager.h>
#include <Game/ComponentManagers/game_camera_manager.h>
#include <Game/game_system.h>
#include <Ecs/Utility/entity_vector.h>


namespace poke {
namespace game {
class SplineStateSystem : public GameSystem {
public:
	SplineStateSystem(Engine& engine, Game& game);
    ~SplineStateSystem() = default;
    
private:
	void OnUpdate();

	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;

	void OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) override;
	void OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) override;

	SplineStateManager& splineStatesManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::SplineFollowersManager& splineFollowersManager_;
	GameCameraManager& gameCameraManager_;

	ecs::EntityIndex cameraIndex_ = ecs::kNoEntity;


	const size_t kSplineStateNb_ = 20;

	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kSplineStateNb_);

	const float kRotationApproximationTolerance_ = 0.1f;
	const float kSpeedApproximationTolerance_ = 0.1f;
};
}//namespace game
}//namespace poke