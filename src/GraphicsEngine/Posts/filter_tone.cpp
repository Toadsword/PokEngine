#include <GraphicsEngine/Posts/filter_tone.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

FilterTone::FilterTone(Pipeline::Stage pipelineStage) :
	PostFilter(pipelineStage, "filter_tone") {
}
void FilterTone::OnRender(const CommandBuffer& commandBuffer)
{
	// Updates descriptors.
	SetAttachment("SamplerColor", GraphicsEngineLocator::Get().GetAttachment("diffuse"));
	PushConditional("writeColor", "samplerColor", "resolved", "diffuse");

	if (!descriptorSet_.Update(pipeline_)) {
		return;
	}

	// Draws the object.
	pipeline_.BindPipeline(commandBuffer);

	descriptorSet_.BindDescriptor(commandBuffer, pipeline_);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
} //namespace graphics
} //namespace poke