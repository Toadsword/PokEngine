#include <GraphicsEngine/Posts/filter_bloom.h>
#include <Math/hash.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

FilterBloom::FilterBloom(Pipeline::Stage pipelineStage) :
PostFilter(pipelineStage, "filter_bloom")
{}

FilterBloom::~FilterBloom()
{
	std::cout << "~FilteBloom();\n";
}

void FilterBloom::OnRender(const CommandBuffer& commandBuffer)
{
	// Updates uniforms.
	descriptorSet_.Push(math::HashString("brightColor"), GraphicsEngineLocator::Get().GetAttachment("resolved"));
	SetAttachment("writeColor", GraphicsEngineLocator::Get().GetAttachment("diffuse"));
	PushConditional("writeColor", "samplerColor", "diffuse", "resolved");
	globalSwitching_++;

	if (!descriptorSet_.Update(pipeline_)) {
		return;
	}

	// Binds the pipeline.
	pipeline_.BindPipeline(commandBuffer);

	// Draws the object.
	descriptorSet_.BindDescriptor(commandBuffer, pipeline_);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
} //namespace graphics
} //namespace poke