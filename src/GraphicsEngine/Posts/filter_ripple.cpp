#include <GraphicsEngine/Posts/filter_ripple.h>

#include <Math/hash.h>
#include <Utility/time_custom.h>
#include <GraphicsEngine/Buffers/uniform_handle.h>

namespace poke {
namespace graphics {

FilterRipple::FilterRipple(const Pipeline::Stage pipelineStage) :
	PostFilter(pipelineStage, "filter_ripple"){}

void FilterRipple::OnRender(const CommandBuffer& commandBuffer)
{
	uniformScene_.Push(math::HashString("time"), Time::Get().GetTime());
	descriptorSet_.Push(math::HashString("UniformScene"), uniformScene_);
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