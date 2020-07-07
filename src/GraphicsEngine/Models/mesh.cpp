#include <GraphicsEngine/Models/mesh.h>

namespace poke {
namespace graphics {
void Mesh::Load()
{
	Initialize(GetVertices<VertexMesh>(0), GetIndices(0));
}

bool Mesh::CmdRender(
    const CommandBuffer& commandBuffer,
    const uint32_t instance) const
{
	if (!vertexBuffer_) return false;
    VkBuffer vertexBuffers[] = {vertexBuffer_->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(
        commandBuffer,
        indexBuffer_->GetBuffer(),
        0,
        GetIndexType());
    vkCmdDrawIndexed(commandBuffer, indexCount_, instance, 0, 0, 0);

    return true;
}

math::Vec3 Mesh::GetExtent() const
{
    const math::Vec3 extent = {
        abs(maxExtents_.x) + abs(minExtents_.x),
        abs(maxExtents_.y) + abs(minExtents_.y),
        abs(maxExtents_.z) + abs(minExtents_.z)
    };
    return extent;
}

math::Vec3 Mesh::GetPositionOffset() const { return positionOffset_; }

std::vector<uint32_t> Mesh::GetIndices(const size_t offset) const {
	if (!indexBuffer_) return {};

	const Buffer indexStaging(indexBuffer_->GetSize(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	CommandBuffer commandBuffer;

	VkBufferCopy copyRegion = {};
	copyRegion.size = indexStaging.GetSize();
	vkCmdCopyBuffer(commandBuffer, indexBuffer_->GetBuffer(), indexStaging.GetBuffer(), 1, &copyRegion);

	commandBuffer.SubmitIdle();

	char *indicesMemory;
	indexStaging.MapMemory(&indicesMemory);
	std::vector<uint32_t> indices(indexCount_);

	const auto sizeOfSrcT = indexStaging.GetSize() / indexCount_;

	for (uint32_t i = 0; i < indexCount_; i++) {
		std::memcpy(&indices[i], static_cast<char *>(indicesMemory) + (i * sizeOfSrcT) + offset, sizeof(uint32_t));
	}

	indexStaging.UnmapMemory();
	return indices;
}

void Mesh::SetIndices(const std::vector<uint32_t> &indices) {
	indexBuffer_.reset();
	indexCount_ = static_cast<uint32_t>(indices.size());

	if (indices.empty())
		return;

	const auto indexStaging = Buffer(
		sizeof(uint32_t) * indices.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		indices.data());

	indexBuffer_.emplace(
		sizeof(uint32_t) * indexStaging.GetSize(),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	auto commandBuffer = CommandBuffer();

	VkBufferCopy copyRegion = {};
	copyRegion.size = indexStaging.GetSize();
	vkCmdCopyBuffer(
		commandBuffer,
		indexStaging.GetBuffer(),
		indexBuffer_->GetBuffer(),
		1,
		&copyRegion);

	commandBuffer.SubmitIdle();
}
} //namespace graphics
} //namespace poke
