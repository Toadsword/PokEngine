#include <GraphicsEngine/Renderpass/renderpass.h>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
RenderPass::RenderPass(
    const RenderStage& renderStage,
    VkFormat depthFormat,
    VkFormat surfaceFormat,
    VkSampleCountFlagBits samples)
{


	// Creates the render passes attachment descriptions,
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	attachmentDescriptions.reserve(renderStage.GetAttachments().size());

	for (const auto& attachment : renderStage.GetAttachments()) {
		auto attachmentSamples = attachment.IsMultisampled() ?
			samples :
			VK_SAMPLE_COUNT_1_BIT;
		VkAttachmentDescription attachmentDescription = {};
		attachmentDescription.samples = attachmentSamples;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// Clear at beginning of the render pass.
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// The image can be read from so it's important to store the attachment results
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// We don't care about initial layout of the attachment.

		switch (attachment.GetType()) {
		case Attachment::Type::IMAGE:
			attachmentDescription.finalLayout =
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentDescription.format = attachment.GetFormat();
			break;
		case Attachment::Type::DEPTH:
			attachmentDescription.finalLayout =
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachmentDescription.format = depthFormat;
			break;
		case Attachment::Type::SWAPCHAIN:
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			attachmentDescription.format = surfaceFormat;
			break;
		case Attachment::Type::NONE:
			break;
		default:;
		}

		attachmentDescriptions.emplace_back(attachmentDescription);
	}

	// Creates each subpass and its dependencies.
	std::vector<std::unique_ptr<SubpassDescription>> subpasses;
	subpasses.reserve(renderStage.GetSubpasses().size());
	std::vector<VkSubpassDependency> dependencies;
	dependencies.reserve(renderStage.GetSubpasses().size());

	for (const auto& subpassType : renderStage.GetSubpasses()) {
		// Attachments.
		std::vector<VkAttachmentReference> subpassColorAttachments;
		subpassColorAttachments.
			reserve(subpassType.GetAttachmentBindings().size());

		uint32_t depthAttachment;

		for (const auto& attachmentBinding : subpassType.GetAttachmentBindings()
			) {
			const auto& attachment = renderStage.
				GetAttachment(attachmentBinding);


			if (attachment.GetType() == Attachment::Type::DEPTH) {
				depthAttachment = attachment.GetBinding();
				continue;
			}

			VkAttachmentReference attachmentReference = {};
			attachmentReference.attachment = attachment.GetBinding();
			attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			subpassColorAttachments.emplace_back(attachmentReference);
		}

		// Subpass description.
		subpasses.emplace_back(
			std::make_unique<SubpassDescription
			>(
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				subpassColorAttachments,
				depthAttachment));

		// Subpass dependencies.
		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		if (subpassType.GetBinding() == renderStage.GetSubpasses().size()) {
			subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.dstStageMask =
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.srcAccessMask =
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		}
		else { subpassDependency.dstSubpass = subpassType.GetBinding(); }

		if (subpassType.GetBinding() == 0) {
			subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.srcStageMask =
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.dstStageMask =
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependency.dstAccessMask =
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}
		else { subpassDependency.srcSubpass = subpassType.GetBinding() - 1; }

		dependencies.emplace_back(subpassDependency);
	}

	std::vector<VkSubpassDescription> subpassDescriptions;
	subpassDescriptions.reserve(subpasses.size());

	for (const auto& subpass : subpasses) {
		subpassDescriptions.emplace_back(subpass->GetSubpassDescription());
	}

	// Creates the render pass.
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(
		attachmentDescriptions.size());
	renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = static_cast<uint32_t>(
		subpassDescriptions.size());
	renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.
		size());
	renderPassCreateInfo.pDependencies = dependencies.data();
	CheckVk(
		vkCreateRenderPass(
			GraphicsEngineLocator::Get().GetLogicalDevice(),
			&renderPassCreateInfo,
			nullptr,
			&renderPass_));
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(
        GraphicsEngineLocator::Get().GetLogicalDevice(),
        renderPass_,
        nullptr);
}
} //namespace graphics
} //namespace poke
