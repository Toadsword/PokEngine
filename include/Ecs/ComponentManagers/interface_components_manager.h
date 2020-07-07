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
// Co-Author : Nicolas Schneider
// Date : 01.03.220
//----------------------------------------------------------------------------------

#pragma once

#include <Ecs/ecs_utility.h>
#include <Ecs/Archetypes/archetype.h>

namespace poke {
namespace ecs {

class IComponentsManager {
public:
	IComponentsManager() = default;
    virtual ~IComponentsManager() = default;

    /**
     * \brief Clear an entity by resetting all its component to default value.
     * \param entityIndex 
     */
    virtual void ClearEntity(EntityIndex entityIndex) = 0;
    /**
     * \brief Resize the number of entities. This function should only be used when adding a new pool of entities.
     * \param size 
     */
    virtual void ResizeEntities(size_t size) = 0;
    /**
	 * \brief Erase a certain number of entities. They all must be aligned in the memory. This function is very costly and should be avoided.
	 * \param pool 
	 * \param nbObjectToErase 
	 */
	virtual void EraseEntities(EntityPool pool, size_t nbObjectToErase) = 0;

    /**
	 * \brief Set component of all entities inside the given entity pool.
	 * \param entityPool 
	 * \param archetype 
	 */
	virtual void SetWithArchetype(EntityPool entityPool, const Archetype& archetype) = 0;
    /**
	 * \brief Insert a single entity anywhere inside the code. This function is very costly and should be avoided.
	 * \param entity 
	 * \param archetype 
	 */
	virtual void InsertArchetype(EntityIndex entity, const Archetype& archetype) = 0;

	virtual void SetComponentFromJson(EntityIndex entityIndex, const json& componentJson) = 0;
	virtual json GetJsonFromComponent(EntityIndex entityIndex) = 0;
};
}
} //namespace poke::ecs
