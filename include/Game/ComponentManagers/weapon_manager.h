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
// Author : Cedric Chambaz
// Date : 10.02.20
//----------------------------------------------------------------------------------

#pragma once
#include <Utility/json_utility.h>
#include <Math/tranform.h>
#include <Ecs/ComponentManagers/interface_components_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>

#include <Game/Components/weapon.h>

namespace poke::game {
class WeaponManager : public ecs::IComponentsManager {
public:
	WeaponManager() = default;
	~WeaponManager() = default;

	/**
	 * \brief Add a lot of entity in one go.
	 * \param size : the number of entity that you want to add. 
	 */
	void ResizeEntities(const std::size_t size) override;

	/**
	 * \brief Remove all component from an entity and set it inactive
	 * \param entityIndex : the index of the entity
	 */
	void ClearEntity(const ecs::EntityIndex entityIndex) override;

	/**
	 * \brief Get a struct with all the data from the component of one entity
	 * \param entityIndex : the index of the entity
	 * \return a struct with all the data and the index of the component
	 */
	Weapon GetComponent(const ecs::EntityIndex entityIndex);

	void SetWithArchetype(ecs::EntityPool entityPool,
						  const ecs::Archetype& archetype) override;
	
	void SetComponent(const ecs::EntityIndex entityIndex, const Weapon& weapon);

	void SetComponentFromJson(ecs::EntityIndex entityIndex, const json& componentJson) override;
	json GetJsonFromComponent(ecs::EntityIndex entityIndex) override;

    void EraseEntities(ecs::EntityPool pool, size_t nbObjectToErase) override;

    void InsertArchetype(
        ecs::EntityIndex entity,
        const ecs::Archetype& archetype) override;

	constexpr static int GetComponentIndex()
	{
		return math::log2(static_cast<int>(ecs::ComponentType::ComponentType::WEAPON));
	}
private:
	std::vector<Weapon> weapons_;
};
}// namespace poke::game