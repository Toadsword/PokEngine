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
// Date : 10.03.20
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/ComponentManagers/transforms_manager.h>
#include <CoreEngine/Camera/core_camera.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/game_system.h>
#include <Ecs/Utility/entity_vector.h>


namespace poke {
namespace game {
class AimingAidSystem : public GameSystem {
public:
	AimingAidSystem(Engine& engine, Game& game);
    ~AimingAidSystem() = default;
private:
	void OnUpdate();

	void SelectVisibleEntities();

	void LookAtCamera(const ecs::EntityIndex entityIndex);
	
	void OnEntitySetActive(ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(ecs::EntityIndex entityIndex) override;

	void OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) override;
	void OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) override;

    enum class TargetSightState : uint8_t {
        NEUTRAL,
        LOCK
    };

	ecs::TransformsManager& transformsManager_;
	PlayerManager& playerManager_;
	ecs::ModelsManager& modelsManager_;
	ICamera& camera_;
	GamePrefabsManager& gamePrefabsManager_;

	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	
	graphics::Model targetSightNeutralModel_;
	graphics::Model targetSightLockModel_;

    inline static const size_t maxTargetNb = 1000;
	inline static const size_t maxPlayerNb = 4;
    size_t beaconTargetNb_ = 0;
	size_t beaconConfirmedTargetNb_ = 0;
	const float kMaxTargetDistance_ = 200.0f;
	const float kMaxAimAngle_ = 4.0f;
	const float kKeepTargetUntilAngle_ = 4.5f;
	const float kDefaultAngle_ = 180.0f;

	const math::Vec3 kTargetSightSize_ = { 20.0f };
	const int kNoEntitySelected_ = -1;

	uint64_t reticuleNeutralMaterial_;
	uint64_t reticuleLockMaterial_;
	bool targetSightUpdated_ = false;
	math::Vec3 cameraPosition_;

	std::array<math::Vec3, maxPlayerNb> targetSightPositions_;
	std::array<math::Vec3, maxPlayerNb> playerPositions_;
	std::array<math::Vec3, maxTargetNb> targetPositions_;
	std::array<ecs::EntityIndex, maxTargetNb> targets_;
	std::array<ecs::EntityIndex, maxTargetNb> selectedTargets_;
	std::array<float, maxTargetNb> selectedTargetAngles_;
	std::array<graphics::Model, maxPlayerNb> targetSightModels_;

	std::array<TargetSightState, maxPlayerNb> newTargetSightStates_;
	std::array<TargetSightState, maxPlayerNb> currentTargetSightStates_;

	ecs::EntityVector players_ = ecs::EntityVector(maxPlayerNb);

	const float kDistAimingOne_ = 20.0f;
	const float kDistAimingTwo_ = 50.0f;
	const float kDistAimingThree_ = 100.0f;

	std::vector<math::Vec3> aimDirections_;
	std::vector<Player> playersDatas_;
};
}//namespace game
}//namespace poke