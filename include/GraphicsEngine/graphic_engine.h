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
// Date : 09.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <vector>
#include <map>
#include <thread>

#include <GraphicsEngine/interface_graphic_engine.h>

#include <vulkan/vulkan.h>
#include <xxhash.h>

namespace poke {
namespace graphics {

/**
 * \brief Create a vulkan's validation layer. The validation layer is used to catch vulkan's specific error
 * \param instance It's the current vulkan instance
 * \param pCreateInfo Information used to create the validation layer
 * \param pAllocator custom allocator
 * \param pDebugMessenger ptr that will store new messenger
 * \return if succeeded return a new messenger, otherwise return VK_ERROR_EXTENSION_NOT_PRESENT
 */
inline VkResult CreateDebugUtilsMessengerEXT(
    const VkInstance instance,
    const
    VkDebugUtilsMessengerCreateInfoEXT*
    pCreateInfo,
    const VkAllocationCallbacks*
    pAllocator,
    VkDebugUtilsMessengerEXT*
    pDebugMessenger)
{
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

/**
 * \brief Destroy a messenger acting as the validation layer
 * \param instance The current Vulkan Instance
 * \param debugMessenger ptr to delete
 * \param pAllocator custom allocator
 */
inline void DestroyDebugUtilsMessengerEXT(
    const VkInstance instance,
    const VkDebugUtilsMessengerEXT
    debugMessenger,
    const VkAllocationCallbacks*
    pAllocator)
{
    const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
}

class GraphicEngine final : public IGraphicEngine {
public:
    /**
     * \brief 
     * \param engine 
     */
    explicit GraphicEngine(Engine& engine);
    ~GraphicEngine();

    void OnEngineDestroy();

    void Render();

    void ResetRenderStages() override;

    void RecreateSwapchain() override;

    void RecreateCommandBuffers() override;

    //Setter
    void SetRenderer(std::unique_ptr<Renderer>&& renderer) override;

    //Getter
    const PhysicalDevice& GetPhysicalDevice() const override;

    const LogicalDevice& GetLogicalDevice() const override;

    const Instance& GetInstance() const override { return instance_; }

    const Surface& GetSurface() const override { return surface_; }

    const Swapchain& GetSwapchain() const override { return *swapchain_; }

    //TODO(@Nico) Use StringHash
    const IDescriptor& GetAttachment(const nonstd::string_view& name) const override;

    const CommandPool& GetCommandPool(const std::thread::id& threadId = std::this_thread::get_id()) override;

    const VkPipelineCache& GetPipelineCache() const override;

    Engine& GetEngine() const override { return engine_; }

    Renderer& GetRenderer() override;

	//--------------- PIPELINE MATERIAL MANAGER --------------------------
    const PipelineMaterialManager& GetPipelineMaterialManager() const override;

    PipelineMaterial& AddPipelineMaterial(
        const Pipeline::Stage& pipelineStage,
        const PipelineGraphicsCreateInfo&
        pipelineCreate) const override;
	//-------------------------------------------------------------------------

	//------------------------------- CAMERA ----------------------------------
    const CameraData& GetCameraData() const override;

    void SetCameraData(CameraData cameraData) override;
	//-------------------------------------------------------------------------

	//------------------------ DRAWING COMMAND BUFFERS --------------------------------------------
    ModelCommandBuffer& GetModelCommandBuffer() override;

    GizmoCommandBuffer& GetGizmoCommandBuffer() override;

    LightCommandBuffer& GetLightCommandBuffer() override;

    ParticleCommandBuffer& GetParticleCommandBuffer() override;
	//---------------------------------------------------------------------------------------------

    const RenderStage& GetRenderStage(uint32_t index) const override;

	//--------------------------------------------- SKYBOX ----------------------------------------
    void UpdateSkybox(const ImageCube& skybox) override;
	//---------------------------------------------------------------------------------------------

	//-------------------------------------------- WINDOW -----------------------------------------
    void SetTitle(const char* newTitle) override;

    void MinimizedWindow() override;

    void MaximizedWindow() override;

	const Window& GetWindow() const override { return window_; }

	void AddWindowResizeObserver(std::function<void()> observerCallback) override;
	//---------------------------------------------------------------------------------------------

	//---------------------------------------- OBSERVERS ------------------------------------------
    void RegisterObserverUpdateSkybox(std::function<void(const ImageCube&)> observer) override;
	//---------------------------------------------------------------------------------------------
private:
    Engine& engine_;

    //WINDOW
    Window window_;

    //VULKAN
    Instance instance_;
    PhysicalDevice physicalDevice_;
    Surface surface_;
    LogicalDevice logicalDevice_;

    std::unique_ptr<Swapchain> swapchain_;

	std::map<std::thread::id, std::unique_ptr<CommandPool>> commandPools_;
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers_;

    VkPipelineCache pipelineCache_{};
    std::vector<VkSemaphore> presentCompletesSemaphore_;
    std::vector<VkSemaphore> renderCompletesSemaphore_;
    std::vector<VkFence> inFlightFences_;
    size_t currentFrame_{};

    std::map<XXH64_hash_t, const IDescriptor &> attachments_;
    std::unique_ptr<Renderer> renderer_;

    //TODO(@Nico) Move outside the pipelineMaterialManager
    std::unique_ptr<PipelineMaterialManager> pipelineMaterialManager_;

    ModelCommandBuffer modelCmdBuffer_;
    GizmoCommandBuffer gizmoCmdBuffer_;
	LightCommandBuffer lightCommandBuffer_;
	ParticleCommandBuffer particleCommandBuffer_;

	bool frameBufferResized_ = false;

	CameraData cameraData_;

	RenderStage& GetRenderStage(uint32_t index);

    //Events
	observer::Subject<const ImageCube&> observersSkyboxUpdate_;

    void CreatePipelineCache();

    bool StartRenderpass(RenderStage& renderStage);

    void EndRenderpass(const RenderStage& renderStage);

	void RecreatePass(RenderStage& renderStage);

    void RecreateAttachmentsMap();
};
} // namespace graphics 
} // namespace poke
