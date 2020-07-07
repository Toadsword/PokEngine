#include <GraphicsEngine/Posts/filter_default.h>
#include <Utility/log.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <iostream>

namespace poke {
namespace graphics {
FilterDefault::FilterDefault(
    const Pipeline::Stage pipelineStage,
    const bool lastFilter)
    : PostFilter(pipelineStage, "filter_default"),
      lastFilter_(lastFilter) {}

FilterDefault::~FilterDefault()
{
	std::cout << "~FilterDefault()\n";
}

void FilterDefault::OnRender(const CommandBuffer& commandBuffer)
{
    //PushConditional("writeColor", "samplerColor", "resolved", "diffuse");

	descriptorSet_.Push(math::HashString("samplerColor"), GraphicsEngineLocator::Get().GetAttachment("diffuse"));

    if (!descriptorSet_.Update(pipeline_)) {
        return;
    }

    pipeline_.BindPipeline(commandBuffer);

    descriptorSet_.BindDescriptor(commandBuffer, pipeline_);
    vkCmdDraw(
        commandBuffer,
        kVertexCount_,
        kInstanceCount_,
        kFirstVertex_,
        kFirstInstance_);

    if (lastFilter_) { globalSwitching_ = 0; }
}
} //namespace graphics
} //namespace poke
