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

#include <Ecs/Archetypes/archetype.h>

namespace poke {
namespace ecs {

/**
 * \brief Manage all custom archetype. The default archetype is managed by the ecs managers
 */
class IArchetypesManager {
public:
    virtual ~IArchetypesManager() = default;
    /**
     * \brief Get an archetype by its name. This function is less efficient than the one using the ArchetypeID.
     * \param archetypeName 
     * \return 
     */
    virtual const Archetype& GetArchetype(
        const std::string& archetypeName) const = 0;

    /**
     * \brief Get an archetype by its ArchetypeID.
     * \param archetypeID 
     * \return 
     */
    virtual const Archetype& GetArchetype(ArchetypeID archetypeID) const = 0;

    /**
     * \brief Add a new archetype to the manager. Its also handling the memory allocation from the ecsManager.
     * \param archetype 
     * \param name
     * \param size
     */
    virtual void AddArchetype(
        const Archetype& archetype,
        const std::string& name,
        size_t size) = 0;

    /**
	 * \brief Update an already existing archetype. WARNING, this operation is heavy And should only be used in the editor.
	 * \param archetypeID 
	 * \param archetype 
	 */
	virtual void UpdateArchetype(ArchetypeID archetypeID, const Archetype& archetype) = 0;

    /**
	 * \brief Resize an archetype. WARNING, this operation is heavy And should only be used in the editor.
	 * \param archetypeID 
	 * \param newSize 
	 */
	virtual void ResizeArchetype(ArchetypeID archetypeID, size_t newSize) = 0;

    /**
	 * \brief Rename an archetype.
	 * \param archetypeID 
	 * \param newName 
	 */
	virtual void RenameArchetype(ArchetypeID archetypeID, const std::string& newName) = 0;

    /**
     * \brief Get an ArchetypeID from a string. If the string if not found, it return the default ID.
     * \param archetypeName 
     * \return 
     */
    virtual ArchetypeID GetArchetypeID(
        const std::string& archetypeName) const = 0;

    /**
	 * \brief Get the entity pool assigned to the given archetype.
	 * \param archetypeID 
	 * \return 
	 */
	virtual EntityPool GetEntityPool(ArchetypeID archetypeID) = 0;

    /**
	 * \brief Get all names of every archetypes.
	 * \return 
	 */
	virtual const std::vector<std::string>& GetAllArchetypesNames() = 0;

    virtual void SetFromJson(const json& archetypesJson) = 0;
    virtual json ToJson() const = 0;
};
} //namespace poke
} //namespace ecs
