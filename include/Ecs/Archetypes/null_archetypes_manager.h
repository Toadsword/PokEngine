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
// Author : Nicolas Schneider
// Co-Author :
// Date :  29.02.2020
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/Archetypes/interface_archetypes_manager.h>

namespace poke {
namespace ecs {
class NullArchetypesManager : public IArchetypesManager {
public:
    const Archetype& GetArchetype(const std::string& archetypeName) const
    override
    {
		archetypeName;
        cassert(false, "Impossible to acces to NullArchetypesManager");
		abort();
    }

    const Archetype& GetArchetype(ArchetypeID archetypeID) const override
    {
		archetypeID;
        cassert(false, "Impossible to acces to NullArchetypesManager");
		abort();
    }

    void AddArchetype(
        const Archetype& archetype,
        const std::string& name,
        size_t size) override
    {
		size;
		name;
		archetype;
    }

    ArchetypeID GetArchetypeID(const std::string& archetypeName) const override
    {
		archetypeName;
        return defaultArchetypeID;
    }

    void SetFromJson(const json& archetypesJson) override
    {
		archetypesJson;
    }

    json ToJson() const override { return json(); }

    void UpdateArchetype(
        ArchetypeID archetypeID,
        const Archetype& archetype) override
    {
		archetypeID;
		archetype;
    }

    void ResizeArchetype(ArchetypeID archetypeID, size_t newSize) override
    {
		archetypeID;
		newSize;
    }

    void RenameArchetype(
        ArchetypeID archetypeID,
        const std::string& newName) override
    {
		archetypeID;
		newName;
    }

    EntityPool GetEntityPool(ArchetypeID archetypeID) override
    {
		archetypeID;
		return EntityPool(0, 0);
    }

    const std::vector<std::string>& GetAllArchetypesNames() override
    {
		return {};
    }
};
} //namespace poke
} //namespace ecs
