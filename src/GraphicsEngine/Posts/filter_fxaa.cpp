#include <GraphicsEngine/Posts/filter_fxaa.h>

namespace poke {
namespace graphics {

FilterFxaa::FilterFxaa(Pipeline::Stage pipelineStage) :
	PostFilter(pipelineStage, "filter_fxaa") {}

void FilterFxaa::OnRender(const CommandBuffer& commandBuffer)
{
	PushConditional("writeColor", "samplerColor", "resolved", "diffuse");

	const auto updateSuccess = descriptorSet_.Update(pipeline_);

	if (!updateSuccess)
	{
		return;
	}

	pipeline_.BindPipeline(commandBuffer);

	descriptorSet_.BindDescriptor(commandBuffer, pipeline_);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
} //namespace graphics
} //namespace poke