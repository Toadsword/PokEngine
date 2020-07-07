#include <GraphicsEngine/Posts/filter_blur.h>
#include <Math/hash.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

FilterBlur::FilterBlur(Pipeline::Stage pipelineStage, const math::Vec2 direction) :
	PostFilter(pipelineStage, "filter_blur"),
	    direction_(direction){}

FilterBlur::~FilterBlur()
{
	std::cout << "~FilterBlur();\n";
}

void FilterBlur::OnRender(const CommandBuffer& commandBuffer)
{
    // Updates uniforms.
	pushScene_.Push(math::HashString("direction"), direction_);

	// Updates descriptors.
	descriptorSet_.Push(math::HashString("PushScene"), pushScene_);
	PushConditional("writeColor", "samplerColor", "resolved", "diffuse");

	if (!descriptorSet_.Update(pipeline_)) {
		return;
	}

	// Binds the pipeline.
	pipeline_.BindPipeline(commandBuffer);

	// Draws the object.
	descriptorSet_.BindDescriptor(commandBuffer, pipeline_);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void FilterBlur::InvalidPipeline()
{
	skipFrame_ = true;
}
} //namespace graphics
} //namespace poke