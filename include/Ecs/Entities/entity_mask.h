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
// Date : 19.11.19
//----------------------------------------------------------------------------------

#pragma once
#include <vector>

#include <Ecs/ecs_utility.h>

namespace poke::ecs
{
class EntityMask
{
public:
	/**
	 * \brief Add a component to the entity's mask
	 * \param attribute: ComponentType(s) or EntityType(s)
	 */
	void AddComponent(const ComponentMask attribute);
	
	/**
	 * \brief Remove a component from the entity's mask
	 * \param attribute: ComponentType(s) or EntityType(s)
	 */
	void RemoveComponent(const ComponentMask attribute);
	
	/**
	 * \brief Check if the entity's mask possess or not a component
	 * \param attribute: ComponentType(s) or EntityType(s)
	 * \return True if the mask possess the component
	 */
	bool HasComponent(const ComponentMask attribute) const;

	/**
	 * \brief Check if the entity have at least the same component as this component mask
	 * \param componentMask : the component mask that contain all the component that you want to check
	 * \return true if all the components in the components mask match with the entity
	 */
	bool HasSameComponents(const ComponentMask componentMask) const;

	/**
	 * \brief Get the component mask that represent all the component of the entity
	 */
	ComponentMask GetComponentMask() const;

	std::vector<ComponentType::ComponentType> GetComponentsByType();

    /**
	 * \brief Set the entity mask to zero, deactivated all his component and set him to inactive
	 */
	void Clear();

	/**
	 * \brief Active the entity
	 */
	void SetActive();

	/**
	 * \brief Check if the entity is active
	 */
	bool IsActive() const;

	bool IsGraphicsActive() const;

	EntityTag GetTag() const;

    void SetTag(EntityTag tag);

    /**
     * \brief Check if the entity is inactive with no component
    */
	bool IsEmpty() const;

	bool operator==(const EntityMask other) const
	{
		return componentMask_ == other.componentMask_;
	}
	bool operator!=(const EntityMask other) const { return !(*this == other); }
	
private:
	ComponentMask componentMask_ = kNoEntity;
};
}//namespace ecs::poke