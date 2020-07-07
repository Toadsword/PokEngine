#include <GraphicsEngine/Subrenderers/subrenderer_particles.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
SubrendererParticles::SubrendererParticles(const Pipeline::Stage& pipelineStage)
    : RenderPipeline(pipelineStage) {}

SubrendererParticles::~SubrendererParticles()
{
	//std::cout << "~SubrendererParticles();\n";
}

void SubrendererParticles::OnRender(const CommandBuffer& commandBuffer)
{
    const auto camera = GraphicsEngineLocator::Get().GetCameraData();
    uniformScene_.Push(kProjectionHash, camera.projectionMatrix);
    uniformScene_.Push(kViewHash, camera.viewMatrix);

    auto& particleCommandBuffer = GraphicsEngineLocator::Get().GetParticleCommandBuffer();
    for (auto& particle : particleCommandBuffer.GetParticleDrawCommands()) {
        particle->CmdRender(commandBuffer, uniformScene_);
    }
}
} //namespace graphics
} //namespace poke
