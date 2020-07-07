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
// Date : 18.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/ComponentManagers/interface_components_manager.h>
#include <Game/Components/enemy.h>

namespace poke {
namespace game {
class EnemiesManager : public ecs::IComponentsManager {
public:
    void ResizeEntities(size_t size) override;

    void ClearEntity(const ecs::EntityIndex entityIndex) override;
    void SetWithArchetype(ecs::EntityPool entityPool, const ecs::Archetype& archetype) override;

	void EraseEntities(ecs::EntityPool pool, size_t nbObjectToErase) override;
	void InsertArchetype(
		ecs::EntityIndex entity,
		const ecs::Archetype& archetype) override;

	json GetJsonFromComponent(ecs::EntityIndex entityIndex) override;
	void SetComponentFromJson(
		ecs::EntityIndex entityIndex,
		const json& componentJson) override;

	Enemy GetComponent(const ecs::EntityIndex entityIndex);
	void SetComponent(const ecs::EntityIndex entityIndex, Enemy enemy);

	constexpr static int GetComponentIndex()
	{
		return math::log2(static_cast<int>(ecs::ComponentType::ComponentType::ENEMY));
	}
private:
	std::vector<Enemy> enemies_;
};
} //namespace game
} //namespace poke
