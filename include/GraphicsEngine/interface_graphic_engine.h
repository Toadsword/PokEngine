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

#include <vector>
#include <thread>

#include <CoreEngine/module.h>
#include <GraphicsEngine/Devices//window.h>
#include <GraphicsEngine/Devices/instance.h>
#include <GraphicsEngine/Devices/surface.h>
#include <GraphicsEngine/Devices/logical_device.h>
#include <GraphicsEngine/Commands/command_pool.h>
#include <GraphicsEngine/Renderpass/swapchain.h>
#include <GraphicsEngine/Commands/command_buffer.h>
#include <GraphicsEngine/Devices/physical_device.h>
#include <GraphicsEngine/RenderStage/render_stage.h>
#include <GraphicsEngine/Renderers/renderer.h>
#include <GraphicsEngine/Pipelines/pipeline_material_manager.h>
#include <GraphicsEngine/camera_data.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <GraphicsEngine/Models/model_command_buffer.h>
#include <GraphicsEngine/Lights/light_command_buffer.h>
#include <GraphicsEngine/Particles/particle_command_buffer.h>

#include <vulkan/vulkan.h>

namespace poke {
namespace graphics {

class IGraphicEngine {
public:
	IGraphicEngine() = default;
    virtual ~IGraphicEngine() = default;

    /**
     * \brief Reset all render stage. Recreate swapchain and all attachments.
     */
    virtual void ResetRenderStages() = 0;

    /**
     * \brief Recreate the swapchain.
     */
    virtual void RecreateSwapchain() = 0;

    /**
     * \brief Recreate all command buffer
     */
    virtual void RecreateCommandBuffers() = 0;

    //Setter
    /**
     * \brief Set the main renderer
     * \param renderer
     */
    virtual void SetRenderer(std::unique_ptr<Renderer>&& renderer) = 0;

    //Getter
	virtual const PhysicalDevice& GetPhysicalDevice() const = 0;

	virtual const LogicalDevice& GetLogicalDevice() const = 0;

	virtual const Instance& GetInstance() const = 0;

	virtual const Surface& GetSurface() const = 0;

	virtual const Swapchain& GetSwapchain() const = 0;

	virtual const IDescriptor& GetAttachment(const nonstd::string_view& name) const = 0;

	virtual const CommandPool& GetCommandPool(
		const std::thread::id& threadId = std::this_thread::get_id()) = 0;

	virtual const VkPipelineCache& GetPipelineCache() const = 0;

	virtual const Window& GetWindow() const = 0;

	virtual void MinimizedWindow() = 0;

	virtual void MaximizedWindow() = 0;

	virtual void AddWindowResizeObserver(std::function<void()> observerCallback) = 0;

	//TODO(@Nico) Change to const &
	virtual Engine& GetEngine() const = 0;

	virtual const PipelineMaterialManager& GetPipelineMaterialManager() const = 0;

	//TODO(@Nico) Change to const &
	virtual PipelineMaterial& AddPipelineMaterial(
		const Pipeline::Stage& pipelineStage,
		const PipelineGraphicsCreateInfo&
		pipelineCreate) const = 0;

	virtual const CameraData& GetCameraData() const = 0;
	virtual void SetCameraData(CameraData cameraData) = 0;

	//TODO(@Nico) Change to const &
	virtual ModelCommandBuffer& GetModelCommandBuffer() = 0;
	//TODO(@Nico) Change to const &
	virtual GizmoCommandBuffer& GetGizmoCommandBuffer() = 0;
	//TODO(@Nico) Change to const &
	virtual LightCommandBuffer& GetLightCommandBuffer() = 0;
	//TODO(@Nico) Change to const &
	virtual ParticleCommandBuffer& GetParticleCommandBuffer() = 0;

	virtual const RenderStage& GetRenderStage(uint32_t index) const = 0;

    /**
	 * \brief Update the current skybox in use
	 * \details This function will update the current skybox use for rendering. This function is thread-safe
	 * \warning Called this function will reconstruct image use by PBR and can reduce performance of the worker thread.
	 * \param skybox 
	 */
	virtual void UpdateSkybox(const ImageCube& skybox) = 0;

	virtual void SetTitle(const char* newTitle) = 0;

	virtual void Render() = 0;

    //Observer register
    /**
	 * \brief Register an observer when skybox is updated.
	 * \details This function is for internal use of the graphics engine to observer when the skybox is updated.
	 * \param observer 
	 */
	virtual void RegisterObserverUpdateSkybox(std::function<void(const ImageCube&)> observer) = 0;

	virtual Renderer& GetRenderer() = 0;
};
} // namespace graphics 
} // namespace poke
