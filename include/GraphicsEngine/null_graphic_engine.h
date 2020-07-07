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
// Date : 28.01.20
//-----------------------------------------------------------------------------
#pragma once
#include <GraphicsEngine/interface_graphic_engine.h>
#include <GraphicsEngine/Renderers/renderer.h>
#include <GraphicsEngine/RenderStage/render_stage.h>

namespace poke {
namespace graphics {
class NullGraphicEngine : public IGraphicEngine {
public:
	NullGraphicEngine() = default;
	virtual ~NullGraphicEngine() override {};

    void ResetRenderStages() override{}
    void RecreateSwapchain() override{}
    void RecreateCommandBuffers() override{}
    void SetRenderer(std::unique_ptr<Renderer>&& renderer) override
    {
		renderer;
    }

    const VkPipelineCache& GetPipelineCache() const override
    {
		cassert(false, "Impossible to get PipelineCache from a null GraphicEngine");
		abort();
    }

    void AddWindowResizeObserver(
        std::function<void()> observerCallback) override{}

    const PhysicalDevice& GetPhysicalDevice() const override
    {
		cassert(false, "Impossible to get PhysicalDevice from a null GraphicEngine");
		abort();
    }

    const LogicalDevice& GetLogicalDevice() const override
    {
		cassert(false, "Impossible to get LogicalDevice from a null GraphicEngine");
		abort();
    }

    const Instance& GetInstance() const override
    {
		cassert(false, "Impossible to get Instance from a null GraphicEngine");
		abort();
    }

    const Surface& GetSurface() const override
    {
		cassert(false, "Impossible to get Surface from a null GraphicEngine");
		abort();
    }

    const Swapchain& GetSwapchain() const override
    {
		cassert(false, "Impossible to get Swapchain from a null GraphicEngine");
		abort();
    }

    const IDescriptor& GetAttachment(const nonstd::string_view& name) const override
    {
		name;
		cassert(false, "Impossible to get Attachment from a null GraphicEngine");
		abort();
    }

    const CommandPool& GetCommandPool(const std::thread::id& threadId) override
    {
		threadId;
		cassert(false, "Impossible to get CommandPool from a null GraphicEngine");
		abort();
    }

    const Window& GetWindow() const override
    {
		cassert(false, "Impossible to get Window from a null GraphicEngine");
		abort();
    }

    Engine& GetEngine() const override
    {
		cassert(false, "Impossible to get Engine from a null GraphicEngine");
		abort();
    }

    const PipelineMaterialManager& GetPipelineMaterialManager() const override
    {
		cassert(false, "Impossible to get PipelineMaterialManager from a null GraphicEngine");
		abort();
    }

    PipelineMaterial& AddPipelineMaterial(
        const Pipeline::Stage& pipelineStage,
        const PipelineGraphicsCreateInfo& pipelineCreate) const override
    {
		pipelineStage;
		pipelineCreate;
		cassert(false, "Impossible to Add Pipeline from a null GraphicEngine");
		abort();
    }

    const CameraData& GetCameraData() const override
    {
		cassert(false, "Impossible to get Camera from a null GraphicEngine");
		abort();
    }

    void SetCameraData(CameraData cameraData) override
    {
		cameraData;
    }

    ModelCommandBuffer& GetModelCommandBuffer() override
    {
		cassert(false, "Impossible to get MOdelCommandBuffer from a null GraphicEngine");
		abort();
    }
    GizmoCommandBuffer& GetGizmoCommandBuffer() override
    {
		cassert(false, "Impossible to get GizmoCommandBuffer from a null GraphicEngine");
		abort();
    }

    const RenderStage& GetRenderStage(uint32_t index) const override
    {
		index;
		cassert(false, "Impossible to get RenderStage from a null GraphicEngine");
		abort();
    }

    void UpdateSkybox(const ImageCube& skybox) override
    {
		skybox;
    }

    void SetTitle(const char* newTitle) override
    {
		newTitle;
    }

	void Render() override {}

    LightCommandBuffer& GetLightCommandBuffer() override
	{
		cassert(false, "Impossible to get LightCommandBuffer from a null GraphicEngine");
		abort();
	}

	ParticleCommandBuffer& GetParticleCommandBuffer() override
	{
		cassert(false, "Impossible to get ParticleCommandBuffer from a null GraphicEngine");
		abort();
	}

    void RegisterObserverUpdateSkybox(std::function<void(const ImageCube&)> observer) override{}

	void MinimizedWindow() override{}

	void MaximizedWindow() override{}

    Renderer& GetRenderer() override
    {
		cassert(false, "Impossible to get renderer from a null GraphicEngine");
		abort();
    }
};
} //namespace graphics
} //namespace poke
