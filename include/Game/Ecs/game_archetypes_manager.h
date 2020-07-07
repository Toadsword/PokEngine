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
// Date :  16.03.2020
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/Archetypes/interface_archetypes_manager.h>
#include <Game/Ecs/game_archetype.h>
#include <json.hpp>

namespace poke {
namespace game {
class GameArchetypesManager : public ecs::IArchetypesManager {
public:
    GameArchetypesManager();
	virtual ~GameArchetypesManager() = default;

	const std::vector<GameArchetype>& GetAllGameArchetype() const {
	    return gameArchetypes_;
	};
	const std::vector<std::string>& GetAllArchetypeName() const {
		return archetypesNames_;
	};

	const ecs::Archetype& GetArchetype(const std::string& archetypeName) const override;

	const ecs::Archetype& GetArchetype(ecs::ArchetypeID archetypeID) const override;

	ecs::ArchetypeID GetArchetypeID(const std::string& archetypeName) const override;

	void AddArchetype(const ecs::Archetype& archetype, const std::string& name, size_t size) override;

	void SetFromJson(const json& archetypesJson) override;
	json ToJson() const override;

	void UpdateArchetype(ecs::ArchetypeID archetypeID, const ecs::Archetype& archetype) override;

	void ResizeArchetype(ecs::ArchetypeID archetypeID, size_t newSize) override;

	void RenameArchetype(ecs::ArchetypeID archetypeID, const std::string& newName) override;

	ecs::EntityPool GetEntityPool(ecs::ArchetypeID archetypeID) override;

	const std::vector<std::string>& GetAllArchetypesNames() override;

private:
	std::vector<std::string> archetypesNames_;
	std::vector<GameArchetype> gameArchetypes_;
};
}//namespace game
}//namespace poke