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

#include <Ecs/system.h>
#include <Ecs/ComponentManagers/trail_renderer_manager.h>
#include <Ecs/Utility/entity_vector.h>
#include <GraphicsEngine/Models/model_command_buffer.h>
#include <ResourcesManager/MeshManagers/interface_mesh_manager.h>

namespace poke {
class TrailRendererSystem final : public ecs::System {
public:
    explicit TrailRendererSystem(Engine& engine);
    ~TrailRendererSystem() = default;

    void OnEntitySetActive(ecs::EntityIndex entityIndex) override;
    void OnEntitySetInactive(ecs::EntityIndex entityIndex) override;
private:
    void OnUpdate();

    void OnCulling();

    void OnAddComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component);

    void OnRemoveComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component);

	void OnEndOfFrame();

    ecs::TrailRendererManager& trailRendererManager_;
    ecs::TransformsManager& transformManager_;

    ecs::EntityVector entities_;
	ecs::EntityVector destroyedEntities_;
	ecs::EntityVector newEntities_;

    //Locator
	IMeshManager& meshManager_;
	graphics::ModelCommandBuffer& modelCommandBuffer_;

	std::vector<ResourceID> meshIDs_;

    struct MeshRebuildInfo {
		DynamicMeshIndex dynamicMeshIndex;
		std::vector<graphics::VertexMesh> vertices;
		std::vector<uint32_t> indexes;
    };

	std::vector<MeshRebuildInfo> meshesToUpdates_;
	std::vector<graphics::ModelForwardIndex> forwardIndexes_;

    struct TrailDrawInfos {
		ResourceID materialID;
		ResourceID meshID;
		graphics::ModelForwardIndex forwardIndex;
    };

	std::vector<TrailDrawInfos> drawInfosDrawing_;
	std::vector<TrailDrawInfos> drawInfosRendering_;
	std::vector<DynamicMeshIndex> dynamicMeshIndex_;
};
} //namespace poke
