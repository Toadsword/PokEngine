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
// Date : 21.03.20
//----------------------------------------------------------------------------------

#pragma once
#include <Ecs/ecs_utility.h>
#include <Ecs/Archetypes/archetype.h>
#include <CoreEngine/core_components_managers_container.h>


namespace poke::ecs {

template <typename T>
class HasGetComponentIndex
{
private:
	typedef char YesType[1];
	typedef char NoType[2];

	template <typename C> static YesType& test(decltype(&C::GetComponentIndex));
	template <typename C> static NoType& test(...);


public:
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

class ComponentsManagersContainer {
public:
	ComponentsManagersContainer();
	~ComponentsManagersContainer() = default;

	void Init();

	/**
	 * \brief Add a number of inactive entity
	 * \param size : the number of entity that you want to add
	 */
	void ResizeEntities(const size_t size);

	/**
	 * \brief Remove all component from an entity and set it inactive
	 * \param entityIndex : the index of the entity
	 */
	void ClearEntity(const EntityIndex entityIndex);

	void SetWithArchetype(const EntityPool archetypeData, const Archetype& archetype);
	void InsertArchetype(EntityIndex entity, const Archetype& archetype);
	void EraseArchetype(EntityPool pool, size_t nbObjectToErase);

    template<typename T>
	T& RegisterManager(
        T&& componentsManager)
    {
		static_assert(HasGetComponentIndex<T>::value, "T has to define a function called GetComponentIndex");
		static_assert(std::is_base_of<IComponentsManager, T>::value, "T has to be a subset of IComponentManager");

		const int componentsManagerIndex = T::GetComponentIndex();

		if (componentsManagers_[componentsManagerIndex] == nullptr) {
			componentsManagers_[componentsManagerIndex] =
				std::make_unique<T>(componentsManager);
			componentsManagers_[componentsManagerIndex]->ResizeEntities(currentEntitiesSize_);
		}

		return *static_cast<T*>(componentsManagers_[componentsManagerIndex].get());
	}

    template<typename T>
    T& GetComponentsManager() {
		static_assert(std::is_base_of<IComponentsManager, T>::value, "T has to be a subset of IComponentManager");

		const int componentsManagerIndex = T::GetComponentIndex();
		if (componentsManagers_[componentsManagerIndex] == nullptr) {
			cassert(false, "This Components Manager has not been created !");
		}
		return *static_cast<T*>(componentsManagers_[componentsManagerIndex].get());
	}


    
private:
	//Remove this version
	std::array<std::unique_ptr<IComponentsManager>, math::log2(static_cast<int>(ComponentType::LENGTH))> componentsManagers_;

	size_t currentEntitiesSize_ = 0;
};
}//namespace poke::ecs