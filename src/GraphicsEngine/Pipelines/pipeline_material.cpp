#include <GraphicsEngine/Pipelines/pipeline_material.h>

#include <utility>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
PipelineMaterial& PipelineMaterial::Create(
    const Pipeline::Stage pipelineStage,
    const PipelineGraphicsCreateInfo& pipelineCreate)
{
    return GraphicsEngineLocator::Get().AddPipelineMaterial(
        pipelineStage,
        pipelineCreate);
}

PipelineMaterial::PipelineMaterial(
    const Pipeline::Stage pipelineStage,
    const PipelineGraphicsCreateInfo& pipelineCreate)
    : pipelineStage_(pipelineStage),
      pipelineGraphicsCreate_(pipelineCreate),
      renderStage_(std::experimental::nullopt),
      pipeline_(nullptr) { }

bool PipelineMaterial::BindPipeline(const CommandBuffer& commandBuffer)
{
    const auto& renderStage = poke::GraphicsEngineLocator::Get().GetRenderStage(
        pipelineStage_.first);

    if (!renderStage_ || &renderStage_.value() != &renderStage) {
        renderStage_.emplace(renderStage);

        pipeline_ = std::make_unique<PipelineGraphics>(
            pipelineStage_,
            pipelineGraphicsCreate_);
    }

    pipeline_->BindPipeline(commandBuffer);
    return true;
}

const PipelineGraphicsCreateInfo& PipelineMaterial::GetPipelineGraphics() const
{
    return pipelineGraphicsCreate_;
}
} //namespace graphics
} //namespace poke
