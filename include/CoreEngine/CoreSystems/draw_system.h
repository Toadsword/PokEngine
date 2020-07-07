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
// Date : 04.02.20
//-----------------------------------------------------------------------------
#pragma once
#include <Ecs/system.h>
#include <Ecs/ComponentManagers/models_manager.h>
#include <Ecs/ComponentManagers/transforms_manager.h>
#include <GraphicsEngine/Models/model_command_buffer.h>
#include <CoreEngine/Camera/interface_camera.h>
#include <Ecs/Utility/entity_vector.h>

namespace poke {
class DrawSystem final : public ecs::System {
public:
    explicit DrawSystem(Engine& engine);
	~DrawSystem() = default;

private:
    void OnDraw();

    void OnEndOfFrame();

	void OnCulling();

	void OnRender();

    void OnUnloadScene();

	void OnAddComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component);
	void OnRemoveComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component);
	void OnUpdateComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component);

	bool CullAABB(physics::AABB aabb, const FrustumPlanes& frustumPlanes);

	graphics::ModelCommandBuffer& modelCommandBuffer_;

	ecs::EntityVector entities_;
	ecs::EntityVector newEntities_;

	ecs::EntityVector forcedDrawEntities_;
	ecs::EntityVector newForcedEntities_;

	std::vector<ecs::EntityIndex> entitiesToDraw_;
	std::vector<ecs::EntityIndex> drawnEntities_;

	ecs::ModelsManager& modelsManager_;
	ecs::TransformsManager& transformsManager_;

	std::vector<graphics::ModelInstanceIndex> instancingIndexes_;
	std::vector<graphics::ModelForwardIndex> forwardIndexes_;

	std::vector<physics::MeshShape> meshShapes_;

	struct BlockDrawForwardInfo {
		math::Matrix4 worldMatrix;

		graphics::Model model;

		graphics::ModelForwardIndex forwardIndex;

		ecs::EntityIndex entityIndex;
	};

	std::vector<BlockDrawForwardInfo> forwardDrawInfo1_;
	std::vector<BlockDrawForwardInfo> forwardDrawInfo2_;

    struct BlockDrawInstanceInfo {
		math::Matrix4 worldMatrix;

		physics::AABB aabb;

		math::Vec3 worldPosition;

		ecs::EntityIndex entityIndex;
    };

    struct DrawInstancesInfo {
		bool frontToBackSorting;

		std::vector<BlockDrawInstanceInfo> instances;
    };

	std::vector<DrawInstancesInfo> instanceDrawInfos1_;
	std::vector<DrawInstancesInfo> instanceDrawInfos2_;
};
} //namespace poke
