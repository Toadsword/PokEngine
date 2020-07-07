#include <GraphicsEngine/Renderpass/framebuffers.h>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
Framebuffers::Framebuffers(
    const uint32_t width,
    const uint32_t height,
    const RenderStage& renderStage,
    const RenderPass& renderpass,
    const Swapchain& swapchain,
    const ImageDepth& depthStencil,
    const VkSampleCountFlagBits samples)
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    for (const auto& attachment : renderStage.GetAttachments()) {
        auto attachmentSamples = attachment.IsMultisampled() ? samples : VK_SAMPLE_COUNT_1_BIT;

        switch (attachment.GetType()) {
        case Attachment::Type::IMAGE:
            imageAttachments_.emplace_back(
                std::make_unique<Image2d>(
                    width,
                    height,
                    std::vector<char>(),
                    attachment.GetFormat(),
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                    VK_FILTER_LINEAR,
                    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                    attachmentSamples));
            break;
        case Attachment::Type::DEPTH:
            imageAttachments_.emplace_back(nullptr);
            break;
        case Attachment::Type::SWAPCHAIN:
            imageAttachments_.emplace_back(nullptr);
            break;
        case Attachment::Type::NONE:
            break;
        default: ;
        }
    }

    framebuffers_.resize(swapchain.GetImageCount());

    for (uint32_t i = 0; i < swapchain.GetImageCount(); i++) {
        std::vector<VkImageView> attachments;

        for (const auto& attachment : renderStage.GetAttachments()) {
            switch (attachment.GetType()) {
            case Attachment::Type::IMAGE:
                attachments.emplace_back(GetAttachment(attachment.GetBinding()).GetView());
                break;
            case Attachment::Type::DEPTH:
                attachments.emplace_back(depthStencil.GetView());
                break;
            case Attachment::Type::SWAPCHAIN:
                attachments.emplace_back(swapchain.GetImageViews().at(i));
                break;
            case Attachment::Type::NONE:
                break;
            default: ;
            }
        }

        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderpass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = width;
        framebufferCreateInfo.height = height;
        framebufferCreateInfo.layers = 1;
        CheckVk(
            vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &framebuffers_[i]));
    }
}

const Image2d& Framebuffers::GetAttachment(const uint32_t index) const
{
    return *imageAttachments_[index];
}

const std::vector<VkFramebuffer>& Framebuffers::GetFramebuffers() const { return framebuffers_; }

Framebuffers::~Framebuffers()
{
	const auto &logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

	imageAttachments_.clear();

    for (const auto& framebuffer : framebuffers_) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }
}
} //namespace graphics
} //namespace poke
