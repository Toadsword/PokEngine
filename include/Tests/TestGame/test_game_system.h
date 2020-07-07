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
// Author : Cédric Chambaz
// Date : 11.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <Ecs/system.h>
#include <Ecs/ComponentManagers/transforms_manager.h>
#include <Ecs/Entities/entity_mask.h>
#include <Ecs/ComponentManagers/spline_followers_manager.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>

namespace poke {
class TestGameSystem final : public ecs::System {
public:
	explicit TestGameSystem(Engine& engineExport);
	~TestGameSystem() = default;

	void OnLoadScene() override;
private:
	ecs::SplineFollowersManager& splinesManager_;
	ecs::TransformsManager& transformsManager_;
	game::PlayerManager& playerManager_;
	game::DestructibleElementManager& destructibleElementManager_;
	ecs::ModelsManager& modelsManager_;
	ecs::CollidersManager& colliderManager_;
	game::WeaponManager& weaponManager_;
};
}//namespace poke