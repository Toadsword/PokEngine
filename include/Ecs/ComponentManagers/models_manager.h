//-----------------------------------------------------------------------------
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
// Date : 11.02.20
//-----------------------------------------------------------------------------
#pragma once

#include <Ecs/ComponentManagers/interface_components_manager.h>

#include <GraphicsEngine/Models/model.h>

namespace poke {
namespace ecs {

class ModelsManager : public IComponentsManager {
public:
    explicit ModelsManager() = default;
    ~ModelsManager() = default;

    void ResizeEntities(size_t size) override;

    void ClearEntity(EntityIndex entityIndex) override;

    void SetWithArchetype(const EntityPool entityPool, const Archetype& archetype) override;
    void InsertArchetype(EntityIndex entity, const Archetype& archetype) override;
    void EraseEntities(EntityPool pool, size_t nbObjectToErase) override;

    graphics::Model GetComponent(EntityIndex entityIndex);

    static graphics::VertexInput GetVertexInput(uint32_t binding = 0);

    void SetComponent(EntityIndex entityIndex, const graphics::Model& model);

    void SetComponentFromJson(
        EntityIndex entityIndex,
        const json& componentJson) override;

    json GetJsonFromComponent(EntityIndex entityIndex) override;

	constexpr static int GetComponentIndex()
	{
		return math::log2(static_cast<int>(ComponentType::MODEL));
	}
private:
	std::vector<graphics::Model> models_;
};
} //namespace ecs
} //namespace poke
