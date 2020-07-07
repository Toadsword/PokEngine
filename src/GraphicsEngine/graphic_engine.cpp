#include <GraphicsEngine/graphic_engine.h>

#include <CoreEngine/engine.h>
#include <GraphicsEngine/vulkan_error_handler.h>
#include <Utility/log.h>
#include <GraphicsEngine/Subrenderers/subrenderer_gizmo.h>

namespace poke {
namespace graphics {
GraphicEngine::GraphicEngine(Engine& engine)
    : IGraphicEngine(),
      engine_(engine),
	  window_({ engine.GetEngineSettings().GetWindowWidth(),engine.GetEngineSettings().GetWindowHeight() }, engine.GetEngineSettings().GetWindowName()),
      instance_(window_.GetWindow()),
      physicalDevice_(instance_),
      surface_(instance_, physicalDevice_, window_),
      logicalDevice_(instance_, physicalDevice_, surface_),
      currentFrame_(0),
      pipelineMaterialManager_(std::make_unique<PipelineMaterialManager>()),
      modelCmdBuffer_(engine),
      gizmoCmdBuffer_(engine),
      lightCommandBuffer_(engine),
      particleCommandBuffer_(engine)
{
    engine.AddObserver(
        observer::MainLoopSubject::ENGINE_DESTROY,
        [this]() { OnEngineDestroy(); });

    engine.AddObserver(
	    observer::MainLoopSubject::APP_INIT,
		[this]() {
		if (!renderer_->HasStarted()) {
			ResetRenderStages();
			renderer_->Start();
		}
	});

    CreatePipelineCache();
}

GraphicEngine::~GraphicEngine()
{
    const auto graphicsQueue = logicalDevice_.GetGraphicsQueue();

    vkQueueWaitIdle(graphicsQueue);

    vkDestroyPipelineCache(logicalDevice_, pipelineCache_, nullptr);

    for (size_t i = 0; i < inFlightFences_.size(); i++) {
        vkDestroyFence(logicalDevice_, inFlightFences_[i], nullptr);
        vkDestroySemaphore(
            logicalDevice_,
            renderCompletesSemaphore_[i],
            nullptr);
        vkDestroySemaphore(
            logicalDevice_,
            presentCompletesSemaphore_[i],
            nullptr);
    }
}

void GraphicEngine::Render()
{
	if (renderer_ == nullptr || window_.IsMinimized()) { return; }

	if (!renderer_->HasStarted()) {
		ResetRenderStages();
		renderer_->Start();
	}

    const auto acquireResult = swapchain_->
        AcquireNextImage(
            presentCompletesSemaphore_[currentFrame_],
            inFlightFences_[currentFrame_]);

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapchain();
        return;
    }

    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        return;
    }

	Pipeline::Stage stage;

    for (auto& renderStage : renderer_->GetRenderStages()) {
		renderStage->Update();
		if (!StartRenderpass(*renderStage)) {
		    return;
		}

		auto &commandBuffer = commandBuffers_[swapchain_->GetActiveImageIndex()];

		for (const auto &subpass : renderStage->GetSubpasses()) {
			stage.second = subpass.GetBinding();

			// Renders subpass subrender pipelines.
			renderer_->GetRendererContainer().RenderStage(stage, *commandBuffer);

			if (subpass.GetBinding() != renderStage->GetSubpasses().back().GetBinding()) {
				vkCmdNextSubpass(*commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
			}
		}

		EndRenderpass(*renderStage);
		stage.first++;
    }
}

void GraphicEngine::OnEngineDestroy()
{
	//Clear drawing command buffer
	gizmoCmdBuffer_.Clear();
	modelCmdBuffer_.Clear();

    CheckVk(vkQueueWaitIdle(logicalDevice_.GetGraphicsQueue()));
}

void GraphicEngine::ResetRenderStages()
{
    RecreateSwapchain();

    if (inFlightFences_.size() != swapchain_->GetImageCount()) {
        RecreateCommandBuffers();
    }

    for (auto& renderStage : renderer_->GetRenderStages()) {
        renderStage->Rebuild(*swapchain_);
    }

    RecreateAttachmentsMap();
}

void GraphicEngine::RecreateSwapchain()
{
    vkDeviceWaitIdle(logicalDevice_);

    const math::Vec2Int size = window_.GetSize();
    VkExtent2D displayExtent{static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
    swapchain_ = std::make_unique<Swapchain>(displayExtent, *swapchain_);

    RecreateCommandBuffers();
}

void GraphicEngine::RecreateCommandBuffers()
{
    for (size_t i = 0; i < inFlightFences_.size(); i++) {
        vkDestroyFence(logicalDevice_, inFlightFences_[i], nullptr);
        vkDestroySemaphore(logicalDevice_, presentCompletesSemaphore_[i], nullptr);
        vkDestroySemaphore(logicalDevice_, renderCompletesSemaphore_[i], nullptr);
    }

    presentCompletesSemaphore_.resize(swapchain_->GetImageCount());
    renderCompletesSemaphore_.resize(swapchain_->GetImageCount());
    inFlightFences_.resize(swapchain_->GetImageCount());
	commandBuffers_.clear();
    commandBuffers_.resize(swapchain_->GetImageCount());

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < inFlightFences_.size(); i++) {
        CheckVk(
            vkCreateSemaphore(
                logicalDevice_,
                &semaphoreCreateInfo,
                nullptr,
                &presentCompletesSemaphore_[i]));

        CheckVk(
            vkCreateSemaphore(
                logicalDevice_,
                &semaphoreCreateInfo,
                nullptr,
                &renderCompletesSemaphore_[i]));

        CheckVk(
            vkCreateFence(
                logicalDevice_,
                &fenceCreateInfo,
                nullptr,
                &inFlightFences_[i]));

        commandBuffers_[i] = std::make_unique<CommandBuffer>(false);
    }
}

void GraphicEngine::SetRenderer(std::unique_ptr<Renderer>&& renderer)
{
    renderer_ = std::move(renderer);
}

const LogicalDevice& GraphicEngine::GetLogicalDevice() const
{
    return logicalDevice_;
}

const PhysicalDevice& GraphicEngine::GetPhysicalDevice() const
{
    return physicalDevice_;
}

void GraphicEngine::CreatePipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    vkCreatePipelineCache(
        logicalDevice_,
        &pipelineCacheCreateInfo,
        nullptr,
        &pipelineCache_);
}

const IDescriptor& GraphicEngine::GetAttachment(const nonstd::string_view& name) const
{
	const auto hash = math::HashString(name);
    const auto it = attachments_.find(hash);

    cassert(
        it != attachments_.end(),
        "Attachment " << name << " doesn't exist");

    return it->second;
}

const CommandPool& GraphicEngine::GetCommandPool(const std::thread::id& threadId)
{
	auto it = commandPools_.find(threadId);

	if (it != commandPools_.end()) {
		return *it->second;
	}
	commandPools_.emplace(threadId, std::make_unique<CommandPool>( logicalDevice_, threadId ));

	it = commandPools_.find(threadId);
	return *it->second;
}

RenderStage& GraphicEngine::GetRenderStage(const uint32_t index)
{
    return renderer_->GetRenderStage(index);
}

const VkPipelineCache& GraphicEngine::GetPipelineCache() const
{
    return pipelineCache_;
}

void GraphicEngine::AddWindowResizeObserver(
    const std::function<void()> observerCallback
) { window_.AddResizeObserver(observerCallback); }

void GraphicEngine::RegisterObserverUpdateSkybox(std::function<void(const ImageCube&)> observer)
{
    observersSkyboxUpdate_.AddObserver(observer);
}

Renderer& GraphicEngine::GetRenderer() { return *renderer_; }

const PipelineMaterialManager& GraphicEngine::
GetPipelineMaterialManager() const { return *pipelineMaterialManager_; }

PipelineMaterial& GraphicEngine::AddPipelineMaterial(
    const Pipeline::Stage& pipelineStage,
    const PipelineGraphicsCreateInfo& pipelineCreate
) const
{
    return pipelineMaterialManager_->AddMaterial(pipelineStage, pipelineCreate);
}

const CameraData& GraphicEngine::GetCameraData() const { return cameraData_; }

void GraphicEngine::SetCameraData(const CameraData cameraData)
{
    cameraData_ = cameraData;
}

ModelCommandBuffer& GraphicEngine::GetModelCommandBuffer()
{
    return modelCmdBuffer_;
}

GizmoCommandBuffer& GraphicEngine::GetGizmoCommandBuffer()
{
    return gizmoCmdBuffer_;
}

LightCommandBuffer& GraphicEngine::GetLightCommandBuffer()
{
    return lightCommandBuffer_;
}

ParticleCommandBuffer& GraphicEngine::GetParticleCommandBuffer()
{
    return particleCommandBuffer_;
}

const RenderStage& GraphicEngine::GetRenderStage(const uint32_t index) const
{
	cassert(renderer_, "The rendere cannot be nullptr");
	return renderer_->GetRenderStage(index);
}

void GraphicEngine::UpdateSkybox(const ImageCube& skybox)
{
	observersSkyboxUpdate_.Notify(skybox);
}

void GraphicEngine::SetTitle(const char* newTitle)
{
    window_.SetTitle(newTitle);
}

void GraphicEngine::MinimizedWindow() { window_.SetMinimized(true); }
void GraphicEngine::MaximizedWindow() { window_.SetMinimized(false); }

void GraphicEngine::RecreatePass(RenderStage& renderStage)
{
    const auto graphicQueue = logicalDevice_.GetGraphicsQueue();

    const VkExtent2D displayExtent = {
        static_cast<uint32_t>(window_.GetSize().x),
        static_cast<uint32_t>(window_.GetSize().y)
    };

    CheckVk(vkQueueWaitIdle(graphicQueue));

    if (renderStage.HasSwapchain() && (frameBufferResized_ || !swapchain_->IsSameExtent(displayExtent)
    )) {
		RecreateSwapchain();
	}

    renderStage.Rebuild(*swapchain_);
    RecreateAttachmentsMap();
}

void GraphicEngine::RecreateAttachmentsMap()
{
    attachments_.clear();

    for (const auto& renderStage : renderer_->GetRenderStages()) {
        attachments_.insert(
            renderStage->descriptors_.begin(),
            renderStage->descriptors_.end());
    }
}

bool GraphicEngine::StartRenderpass(RenderStage& renderStage)
{
    if (renderStage.IsOutOfDate()) {
        RecreatePass(renderStage);
        return false;
    }

    if (!commandBuffers_[swapchain_->GetActiveImageIndex()]->IsRunning()) {
        CheckVk(
            vkWaitForFences(
                logicalDevice_,
                1,
                &inFlightFences_[currentFrame_],
                VK_TRUE,
                std::numeric_limits<uint64_t>::max()));
        commandBuffers_[swapchain_->GetActiveImageIndex()]->
            Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    }

    VkRect2D renderArea = {};
    renderArea.offset = {0, 0};
    renderArea.extent = {
        static_cast<uint32_t>(renderStage.GetSize().x),
        static_cast<uint32_t>(renderStage.GetSize().y)
    };

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(renderArea.extent.height);
    viewport.width = static_cast<float>(renderArea.extent.width);
    viewport.height = -static_cast<float>(renderArea.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(
        *commandBuffers_[swapchain_->GetActiveImageIndex()],
        0,
        1,
        &viewport);

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = renderArea.extent;
    vkCmdSetScissor(
        *commandBuffers_[swapchain_->GetActiveImageIndex()],
        0,
        1,
        &scissor);

    auto clearValues = renderStage.GetClearValues();

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = *renderStage.GetRenderPass();
    renderPassBeginInfo.framebuffer = renderStage.
        GetActiveFramebuffer(swapchain_->GetActiveImageIndex());
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.
        size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(
        *commandBuffers_[swapchain_->GetActiveImageIndex()],
        &renderPassBeginInfo,
        VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

void GraphicEngine::EndRenderpass(const RenderStage& renderStage)
{
    const auto presentQueue = logicalDevice_.GetPresentQueue();

    vkCmdEndRenderPass(*commandBuffers_[swapchain_->GetActiveImageIndex()]);

    if (!renderStage.HasSwapchain()) { return; }

    commandBuffers_[swapchain_->GetActiveImageIndex()]->End();
    commandBuffers_[swapchain_->GetActiveImageIndex()]->
        Submit(
            presentCompletesSemaphore_[currentFrame_],
            renderCompletesSemaphore_[currentFrame_],
            inFlightFences_[currentFrame_]);

    const auto presentResult = swapchain_->QueuePresent(
        presentQueue,
        renderCompletesSemaphore_[currentFrame_]);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
    {
		frameBufferResized_ = true;
    }else if(presentResult != VK_SUCCESS) {
        CheckVk(presentResult);
		LogError("Failed to presents swapchain image");
    }

    currentFrame_ = (currentFrame_ + 1) % swapchain_->GetImageCount();
}
} //namespace graphics
} //namespace poke
