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
// Date : 28.03.2020
//----------------------------------------------------------------------------------
#pragma once

#include <vector>

#include <Ecs/ComponentManagers/interface_components_manager.h>
#include <Ecs/Components/trail_renderer.h>

namespace poke {
namespace ecs {
class TrailRendererManager final : public IComponentsManager {
public:
	TrailRendererManager() = default;
    ~TrailRendererManager() = default;

    /**
     * \brief Add a lot of entity in one go.
     * \param size : the number of entity that you want to add.
     */
    void ResizeEntities(std::size_t size) override;

    /**
     * \brief Remove all component from an entity and set it inactive
     * \param entityIndex : the index of the entity
     */
    void ClearEntity(EntityIndex entityIndex) override;

    void SetWithArchetype(EntityPool entityPool, const Archetype& archetype) override;
    void InsertArchetype(EntityIndex entity, const Archetype& archetype) override;
    void EraseEntities(EntityPool pool, size_t nbObjectToErase) override;

    /**
     * \brief Get a struct with all the data from the component of one entity
     * \param entityIndex : the index of the entity
     * \return a struct with all the data and the index of the component
     */
    const TrailRenderer& GetComponent(EntityIndex entityIndex) const;

    void SetComponent(EntityIndex entityIndex, const TrailRenderer& trailRenderer);

    void SetComponentFromJson(
        const EntityIndex entityIndex,
        const json& componentJson) override;

    json GetJsonFromComponent(EntityIndex entityIndex) override;

    constexpr static int GetComponentIndex()
    {
        return math::log2(static_cast<int>(ComponentType::TRAIL_RENDERER));
    }

private:
    std::vector<TrailRenderer> trailRenderers_;
};
} // namespace ecs
} //namespace poke