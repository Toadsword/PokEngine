#include <GraphicsEngine/RenderStage/render_stage.h>
#include <GraphicsEngine/graphic_engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
RenderStage::RenderStage(
    const std::vector<Attachment>& images,
    const std::vector<SubpassType>& subpasses,
    const Viewport& viewport)
    : attachments_(images),
      subpasses_(subpasses),
      viewport_(viewport),
      subpassAttachmentCount_(subpasses_.size()),
      size_(0, 0),
      aspectRatio_(1.0f),
      outOfDate_(false)
{
    for (const auto& image : attachments_) {
        VkClearValue clearValue = {};

        switch (image.GetType()) {
        case Attachment::Type::IMAGE:
            clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

            for (auto& subpass : subpasses_) {
                auto subpassBinding = subpass.GetAttachmentBindings();

                if (std::find(
                        subpassBinding.begin(),
                        subpassBinding.end(),
                        image.GetBinding()) != subpassBinding.end()) {
                    subpassAttachmentCount_[subpass.GetBinding()]++;

                    if (image.IsMultisampled()) {
                        subpass.SetIsMultisampled(true);
                    }
                }
            }
            break;
        case Attachment::Type::DEPTH:
            clearValue.depthStencil = {
                static_cast<uint32_t>(1.0f),
                static_cast<uint32_t>(0.0f)
            };
            depthAttachment_ = Attachment(image);
            break;
        case Attachment::Type::SWAPCHAIN:
            clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
            swapchainAttachment_ = Attachment(image);
            break;
        default: ;
        }

        clearValues_.emplace_back(clearValue);
    }
}

void RenderStage::Update()
{
    const auto lastSize = size_;

    if (viewport_.HasSize()) { size_ = viewport_.GetSize(); } else {
        size_ = GraphicsEngineLocator::Get().GetWindow().GetSize();
    }

    size_ = math::Vec2Int(
        size_.x * viewport_.GetScale().x,
        size_.y * viewport_.GetScale().y);
    aspectRatio_ = static_cast<float>(size_.x) / static_cast<float>(size_.y);

    outOfDate_ = size_ != lastSize;
}

void RenderStage::Rebuild(const Swapchain& swapchain)
{
    Update();

    const auto& physicalDevice = GraphicsEngineLocator::Get().GetPhysicalDevice();
    const auto& surface = GraphicsEngineLocator::Get().GetSurface();

    const auto msaaSamples = physicalDevice.GetMsaaSamples();

    if (depthAttachment_.GetType() != Attachment::Type::NONE) {
		depthStencil_ = std::make_unique<ImageDepth>(size_, depthAttachment_.IsMultisampled() ? msaaSamples : VK_SAMPLE_COUNT_1_BIT);
    }

    if (!renderPass_) {
		renderPass_ = std::make_unique<RenderPass>(*this, depthStencil_ ? depthStencil_->GetFormat() : VK_FORMAT_UNDEFINED, surface.GetFormat().format, msaaSamples);
    }

    framebuffers_ = std::make_unique<Framebuffers>(size_.x, size_.y, *this, *renderPass_, swapchain, *depthStencil_, msaaSamples);
    outOfDate_ = false;

    descriptors_.clear();

    for (const auto& image : attachments_) {
        const auto hash = math::HashString(image.GetName());

        if (image.GetType() == Attachment::Type::DEPTH) {
            descriptors_.emplace(hash, *depthStencil_);
        } else if (image.GetType() != Attachment::Type::SWAPCHAIN) {
            descriptors_.emplace(
                hash,
                framebuffers_->GetAttachment(image.GetBinding()));
        }
    }
}

const Attachment& RenderStage::GetAttachment(
    const nonstd::string_view& name) const
{
    for (const auto& attachment : attachments_) {
        if (attachment.GetName() == name) { return attachment; }
    }

    cassert(false, "Attachment with name " << name << " doesn't exist");
}

const Attachment& RenderStage::GetAttachment(const uint32_t binding) const
{
    for (const auto& attachment : attachments_) {
        if (attachment.GetBinding() == binding) {
            return attachment;
        }
    }

    cassert(false, "Attachment with binding" << binding << " doesn't exist");
	return attachments_[0];
}

const std::vector<Attachment>& RenderStage::GetAttachments() const
{
    return attachments_;
}

const IDescriptor& RenderStage::GetDescriptor(
    const nonstd::string_view& name) const
{
    const auto it = descriptors_.find(math::HashString(name));

    cassert(
        it != descriptors_.end(),
        "descriptor with name" << name << " doesn't exist");

    return it->second;
}

const VkFramebuffer& RenderStage::GetActiveFramebuffer(
    const uint32_t& activeSwapchainImage) const
{
    if (activeSwapchainImage > framebuffers_->GetFramebuffers().size()) {
        return framebuffers_->GetFramebuffers().at(0);
    }

    return framebuffers_->GetFramebuffers().at(activeSwapchainImage);
}

const std::vector<VkClearValue>& RenderStage::GetClearValues() const
{
    return clearValues_;
}

uint32_t RenderStage::GetAttachmentCount(const uint32_t subpass) const
{
    return subpassAttachmentCount_[subpass];
}

bool RenderStage::HasDepth() const
{
    return depthAttachment_.GetType() != Attachment::Type::NONE;
}

bool RenderStage::HasSwapchain() const
{
    return swapchainAttachment_.GetType() != Attachment::Type::NONE;
}

bool RenderStage::IsMultisampled(const uint32_t subpass) const
{
    return subpasses_[subpass].IsMultisampled();
}
} //namespace graphics
} //namespace poke
