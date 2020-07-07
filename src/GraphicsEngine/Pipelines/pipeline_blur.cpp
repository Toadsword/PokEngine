#include <GraphicsEngine/Pipelines/pipeline_blur.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

PipelineBlur::PipelineBlur(
    const Pipeline::Stage& pipelineStage,
    const float blur)
    : RenderPipeline(pipelineStage),
      kWindow_(GraphicsEngineLocator::Get().GetWindow()),
      filterBlurVertical_(pipelineStage, {0.0f, blur}),
      filterBlurHorizontal_(pipelineStage, {blur, 0.0f}),
      bloom_(pipelineStage),
      output_(std::make_unique<Image2d>(kWindow_.GetSize(), VK_FORMAT_R16G16B16A16_SFLOAT)),
      output1_(std::make_unique<Image2d>(kWindow_.GetSize(), VK_FORMAT_R16G16B16A16_SFLOAT)),
      blur_(blur)
{
    const auto size = kWindow_.GetSize();

    filterBlurVertical_.SetAttachment("writeColor", static_cast<const IDescriptor&>(*output_));
    filterBlurHorizontal_.SetAttachment("writeColor", static_cast<const IDescriptor&>(*output1_));

    GraphicsEngineLocator::Get().AddWindowResizeObserver([this]() { OnWindowResize(); });
}

void PipelineBlur::OnRender(const CommandBuffer& commandBuffer)
{
    filterBlurVertical_.SetAttachment(
        "samplerColor",
        GraphicsEngineLocator::Get().GetAttachment("bright"));
    filterBlurVertical_.SetDirection({0.0f, blur_});
    filterBlurHorizontal_.SetAttachment("samplerColor", static_cast<const IDescriptor&>(*output_));
    filterBlurHorizontal_.SetDirection({blur_, 0.0f});

    filterBlurVertical_.OnRender(commandBuffer);
    filterBlurHorizontal_.OnRender(commandBuffer);

    bloom_.SetAttachment("samplerColor", static_cast<const IDescriptor&>(*output1_));
    bloom_.OnRender(commandBuffer);
}

void PipelineBlur::OnWindowResize()
{
    const auto size = kWindow_.GetSize();
    output_ = std::make_unique<Image2d>(size, VK_FORMAT_R16G16B16A16_SFLOAT);
    output1_ = std::make_unique<Image2d>(size, VK_FORMAT_R16G16B16A16_SFLOAT);

    filterBlurVertical_.SetAttachment("writeColor", static_cast<const IDescriptor&>(*output_));
    filterBlurHorizontal_.SetAttachment("writeColor", static_cast<const IDescriptor&>(*output1_));
}
} //namespace graphics
} //namespace poke
