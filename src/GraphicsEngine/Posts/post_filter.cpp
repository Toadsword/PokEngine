#include <GraphicsEngine/Posts/post_filter.h>

#include <GraphicsEngine/graphic_engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
PostFilter::PostFilter(
    const Pipeline::Stage pipelineStage,
    const nonstd::string_view& shaderFileName)
    : RenderPipeline(pipelineStage),
      pipeline_(
          pipelineStage,
          static_cast<std::string>(shaderFileName),
          {},
          PipelineGraphics::Mode::POLYGON,
          PipelineGraphics::Depth::NONE,
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
          VK_POLYGON_MODE_FILL,
          0,
          VK_FRONT_FACE_CLOCKWISE,
          false
      ) { }

PostFilter::~PostFilter()
{
    //std::cout << "~PostFilter\n";
}

const IDescriptor& PostFilter::GetAttachment(
    const nonstd::string_view& descriptorName,
    const IDescriptor& descriptor) const
{
    const auto it = attachments_.find(math::HashString(descriptorName));

    if (it == attachments_.end()) { return descriptor; }

    return it->second;
}

const IDescriptor& PostFilter::GetAttachment(
    const nonstd::string_view& descriptorName,
    const nonstd::string_view& rendererAttachment) const
{
    const auto it = attachments_.find(math::HashString(descriptorName));

    if (it == attachments_.end()) {
        return GraphicsEngineLocator::Get().GetAttachment(
            static_cast<std::string>(rendererAttachment));
    }

    return it->second;
}

void PostFilter::SetAttachment(
    const nonstd::string_view& descriptorName,
    const IDescriptor& descriptor)
{
	const auto hash = math::HashString(descriptorName);
    const auto it = attachments_.find(hash);

    if (it == attachments_.end()) {
        attachments_.emplace(hash, descriptor);
        return;
    }

    attachments_.erase(it);
    attachments_.emplace(hash, descriptor);
}

bool PostFilter::RemoveAttachment(const nonstd::string_view& descriptorName)
{
    const auto hash = math::HashString(descriptorName);
    const auto it = attachments_.find(hash);

    if (it != attachments_.end()) {
        attachments_.erase(it);
        return true;
    }

    return false;
}

void PostFilter::PushConditional(
    const nonstd::string_view& descriptorName1,
    const nonstd::string_view& descriptorName2,
    const nonstd::string_view& rendererAttachment1,
    const nonstd::string_view& rendererAttachment2)
{
	const auto hash1 = math::HashString(descriptorName1);
    const auto it1 = attachments_.find(hash1);
    const auto hash2 = math::HashString(descriptorName2);
    const auto it2 = attachments_.find(hash2);

    if (it1 != attachments_.end() || it2 != attachments_.end()) {
        descriptorSet_.Push(
			hash1,
            GetAttachment(
                descriptorName1,
                rendererAttachment1));
        descriptorSet_.Push(
			hash2,
            GetAttachment(
                descriptorName2,
                rendererAttachment1));
        return;
    }
    if (it1 == attachments_.end() && it2 != attachments_.end()) {
        descriptorSet_.Push(
			hash1,
            GraphicsEngineLocator::Get().GetAttachment(
                globalSwitching_ % 2 == 1 ?
                    rendererAttachment1 :
                    rendererAttachment2));
        descriptorSet_.Push(
			hash2,
            GetAttachment(descriptorName2, rendererAttachment1));
        return;
    }
    if (it1 != attachments_.end() && it2 == attachments_.end()) {
        descriptorSet_.Push(
			hash1,
            GetAttachment(
                descriptorName1,
                rendererAttachment1));
        descriptorSet_.Push(
			hash2,
            GraphicsEngineLocator::Get().GetAttachment(
                globalSwitching_ % 2
                == 1 ?
                    rendererAttachment1 :
                    rendererAttachment2));
        return;
    }

    if (globalSwitching_ % 2 == 1) {
        descriptorSet_.Push(
			hash1,
            GraphicsEngineLocator::Get().GetAttachment(rendererAttachment1));
        descriptorSet_.Push(
			hash2,
            GraphicsEngineLocator::Get().GetAttachment(rendererAttachment2));
    } else {
        descriptorSet_.Push(
			hash1,
            GraphicsEngineLocator::Get().GetAttachment(rendererAttachment2));
        descriptorSet_.Push(
			hash2,
            GraphicsEngineLocator::Get().GetAttachment(rendererAttachment1));
    }

    globalSwitching_++;
}
} //namespace graphics
} //namespace poke
