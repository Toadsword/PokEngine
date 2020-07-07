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
// Author : Guillaume Volgyesi
// Date : 20.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Game/Ecs/game_prefab.h>

namespace poke::editor {
class EditorPrefab : public game::GamePrefab {

public:
	const std::vector<game::GameArchetype>& GetArchetypes() const;

	void SetArchetypes(const std::vector<game::GameArchetype>& archetypes);

	std::vector<ecs::EntityIndex> Instantiate() const override;

	const std::vector<size_t>& GetParentIndexes() const;

	const ecs::Archetype& GetObject(size_t index) const override;

	void SetParentIndexes(const std::vector<size_t>& parentIndexes);

    void Clear();

	void SetNames(const std::vector<std::string>& names);

	void SetArchetypesId(const std::vector<ecs::ArchetypeID>& archetypesId);

	void SetFromJson(const json& prefabJson);

	json ToJson() const;
private:
};

}//namespace poke::editor