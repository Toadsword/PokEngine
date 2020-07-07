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
// Author : Simon Canas
// Date : 03.03.20
//----------------------------------------------------------------------------------

#pragma once
#include <Ecs/system.h>
#include <CoreEngine/engine.h>
#include <Game/ComponentManagers/special_attack_manager.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/time_custom.h>
#include <Game/game_system.h>
#include <Game/ComponentManagers/destructible_element_manager.h>

namespace poke::game
{
/**
 * \brief Central class for the weapons management
*/
class SpecialAttackSystem final : public GameSystem
{
public:
	explicit SpecialAttackSystem(Engine& engine, Game& game);
	~SpecialAttackSystem() = default;

	void OnUpdate();

	void OnTriggerEnter(const poke::ecs::EntityIndex entityIndex, const poke::physics::Collision collision) const;
private:

	void OnAddComponent(
		ecs::EntityIndex entityIndex,
		ecs::ComponentMask component);

	void OnRemoveComponent(
		ecs::EntityIndex entityIndex,
		ecs::ComponentMask component);

	void OnEntitySetActive(const ecs::EntityIndex entityIndex) override;
	void OnEntitySetInactive(const ecs::EntityIndex entityIndex) override;
	
	Time& time_;
	SpecialAttackManager& specialAttackManager_;
	DestructibleElementManager& destructibleElementManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::CollidersManager& colliderManager_;

	graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	const size_t kEntityDefaultNb_ = 10;
	ecs::EntityVector entityIndexes_ = ecs::EntityVector(kEntityDefaultNb_);
	ecs::EntityVector registeredEntities_ = ecs::EntityVector(kEntityDefaultNb_);
};
} //namespace poke::game
