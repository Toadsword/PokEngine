#include <GraphicsEngine/Buffers/push_handle.h>

#include <GraphicsEngine/Pipelines/pipeline.h>

namespace poke {
namespace graphics {
PushHandle::PushHandle(const UniformBlock& uniformBlock)
    : uniformBlock_(uniformBlock),
      data_(std::vector<char>(uniformBlock.GetSize())) {}

bool PushHandle::Update(const UniformBlock& uniformBlock)
{
    if (!uniformBlock_ || *uniformBlock_ != uniformBlock) {
        uniformBlock_ = std::experimental::optional<const UniformBlock&>(
            uniformBlock);
        data_ = std::vector<char>(uniformBlock.GetSize());
        return false;
    }

    return true;
}

void PushHandle::BindPush(
    const CommandBuffer& commandBuffer,
    const Pipeline& pipeline) const
{
    vkCmdPushConstants(
        commandBuffer,
        pipeline.GetPipelineLayout(),
        uniformBlock_->GetStageFlags(),
        0,
        static_cast<uint32_t>(uniformBlock_->GetSize()),
        &data_[0]);
}
} //namespace graphics
} //namespace poke
