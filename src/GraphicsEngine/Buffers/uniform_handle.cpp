#include <GraphicsEngine/Buffers/uniform_handle.h>

namespace poke {
namespace graphics {
UniformHandle::UniformHandle(
    const bool multiPipeline) : multiPipeline_(multiPipeline) { }

UniformHandle::UniformHandle(
    const UniformBlock& uniformBlock,
    const bool multiPipeline)
    : multiPipeline_(multiPipeline),
      uniformBlock_(uniformBlock),
      size_(uniformBlock_->GetSize()),
      arbitraryUniformData_(std::vector<char>(size_)),
      uniformBuffer_(
          std::make_unique<UniformBuffer>(
              static_cast<VkDeviceSize>(
                  size_))) { }

UniformHandle::UniformHandle(const UniformHandle& other)
    : multiPipeline_(other.multiPipeline_),
      uniformBlock_(other.uniformBlock_),
      size_(other.size_),
      arbitraryUniformData_(other.arbitraryUniformData_),
      uniformBuffer_(nullptr),
      handleStatus_(other.handleStatus_)
{
    if (other.uniformBuffer_) {
        uniformBuffer_ = std::make_unique<UniformBuffer>(
            *other.uniformBuffer_.get());
    }
}

UniformHandle::UniformHandle(UniformHandle&& other) noexcept
    : multiPipeline_(other.multiPipeline_),
      uniformBlock_(std::move(other.uniformBlock_)),
      size_(other.size_),
      arbitraryUniformData_(std::move(other.arbitraryUniformData_)),
      uniformBuffer_(std::move(other.uniformBuffer_)),
      handleStatus_(other.handleStatus_) {}

UniformHandle& UniformHandle::operator=(const UniformHandle& other)
{
    if (this == &other)
        return *this;
    multiPipeline_ = other.multiPipeline_;
    uniformBlock_ = other.uniformBlock_;
    size_ = other.size_;
    arbitraryUniformData_ = other.arbitraryUniformData_;
	if (uniformBuffer_) {
		uniformBuffer_ = std::make_unique<UniformBuffer>(
			*other.uniformBuffer_);
	}
    handleStatus_ = other.handleStatus_;
    return *this;
}

UniformHandle& UniformHandle::operator=(UniformHandle&& other) noexcept
{
    if (this == &other)
        return *this;
    multiPipeline_ = other.multiPipeline_;
    uniformBlock_ = std::move(other.uniformBlock_);
    size_ = other.size_;
    arbitraryUniformData_ = std::move(other.arbitraryUniformData_);
    uniformBuffer_ = std::move(other.uniformBuffer_);
    handleStatus_ = other.handleStatus_;
    return *this;
}


void UniformHandle::PushUniformData(
    const Material::PushDataContainer& dataContainer)
{
    for (const auto& data : dataContainer) {
        Push(data.first, data.second);
    }
}

bool UniformHandle::Update(const UniformBlock& uniformBlock)
{
    if (handleStatus_ == Buffer::Status::RESET || multiPipeline_ && !
        uniformBlock_ || !multiPipeline_ && (!uniformBlock_
                                             || *uniformBlock_ != uniformBlock)
    ) {
        if (size_ == 0 && !uniformBlock_ || uniformBlock_ && *uniformBlock_ !=
            uniformBlock && uniformBlock_->GetSize() == size_) {
            size_ = static_cast<uint32_t>(uniformBlock.GetSize());
        }

        uniformBlock_.emplace(uniformBlock);
        arbitraryUniformData_ = std::vector<char>(size_);
        uniformBuffer_ = std::make_unique<UniformBuffer>(
            static_cast<
                VkDeviceSize>(size_
            ));
        handleStatus_ = Buffer::Status::CHANGED;

        return false;
    }

    if (handleStatus_ != Buffer::Status::NORMAL) {
        uniformBuffer_->Update(arbitraryUniformData_);
        handleStatus_ = Buffer::Status::NORMAL;
    }

    return true;
}
} //namespace graphics
} //namespace poke
