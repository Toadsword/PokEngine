//-----------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author :
// Date : 29.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include <optional_custom.h>

#include <GraphicsEngine/Commands/command_buffer.h>
#include <GraphicsEngine/Buffers/buffer.h>
#include <Math/vector.h>
#include <GraphicsEngine/vertex_mesh.h>

namespace poke {
namespace graphics {

/**
 * \brief A mesh contains all vertex and indexes used to draw a object.
 */
class Mesh {
public:
    /**
     * \brief 
     */
    Mesh() = default;

    virtual ~Mesh() = default;

    template <typename T>
    explicit Mesh(
        const std::vector<T>& vertices,
        const std::vector<uint32_t>& indices = {}
    ) : Mesh() { Initialize(vertices, indices); }

    /**
     * \brief This function load the mesh in the GPU
     * \details Is should only be called when the mesh is in its final memory location.
     */
    virtual void Load();

    bool CmdRender(
        const CommandBuffer& commandBuffer,
        uint32_t instance = 1
    ) const;

    template <typename T>
    std::vector<T> GetVertices(const size_t offset) const
    {
		if (!vertexBuffer_) return {};
        const Buffer vertexStaging(
            vertexBuffer_->GetSize(),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        CommandBuffer commandBuffer;

        VkBufferCopy copyRegion = {};
        copyRegion.size = vertexStaging.GetSize();
        vkCmdCopyBuffer(
            commandBuffer,
            vertexBuffer_->GetBuffer(),
            vertexStaging.GetBuffer(),
            1,
            &copyRegion);

        commandBuffer.SubmitIdle();

        char* verticesMemory;
        vertexStaging.MapMemory(&verticesMemory);
        std::vector<T> vertices(vertexCount_);

        const auto sizeOfSrcT = vertexStaging.GetSize() / vertexCount_;

        for (uint32_t i = 0; i < vertexCount_; i++) {
            std::memcpy(
                &vertices[i],
                static_cast<char *>(verticesMemory) + (i * sizeOfSrcT) + offset,
                sizeof(T));
        }

        vertexStaging.UnmapMemory();
        return vertices;
    }

    template <typename T>
    void SetVertices(const std::vector<T>& vertices)
    {
		vertexBuffer_.reset();
		vertexCount_ = static_cast<uint32_t>(vertices.size());

        if (vertices.empty())
            return;
		const auto vertexStaging = Buffer(
			sizeof(T) * vertices.size(),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			vertices.data());

		vertexBuffer_.emplace(
			vertexStaging.GetSize(), 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CommandBuffer commandBuffer;

		VkBufferCopy copyRegion = {};
		copyRegion.size = vertexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, vertexStaging.GetBuffer(), vertexBuffer_->GetBuffer(), 1, &copyRegion);

		commandBuffer.SubmitIdle();
    }


    std::vector<uint32_t> GetIndices(size_t offset) const;

    void SetIndices(const std::vector<uint32_t>& indices);

    const Buffer& GetVertexBuffer() const { return *vertexBuffer_; }

    const Buffer& GetIndexBuffer() const { return *indexBuffer_; }

    uint32_t GetVertexCount() const { return vertexCount_; }

    uint32_t GetIndexCount() const { return indexCount_; }

    /**
     * \brief Return the radius of a sphere that contain the mesh
     * \return 
     */
    float GetRadius() const { return radius_; }

    static VkIndexType GetIndexType() { return VK_INDEX_TYPE_UINT32; }

    math::Vec3 GetExtent() const;

    math::Vec3 GetPositionOffset() const;

    template <typename T>
    void Initialize(
        const std::vector<T>& vertices,
        const std::vector<uint32_t>& indices)
    {
        static_assert(std::is_base_of<VertexMesh, T>::value,
            "T must derive from MeshVertex");

		SetVertices(vertices);
		SetIndices(indices);

        minExtents_ = math::Vec3(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max());
        maxExtents_ = math::Vec3(
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min());

        for (const auto& vertex : vertices) {
            minExtents_ = math::Vec3(
                std::min(minExtents_.x, vertex.position.x),
                std::min(minExtents_.y, vertex.position.y),
                std::min(minExtents_.z, vertex.position.z));
            maxExtents_ = math::Vec3(
                std::max(maxExtents_.x, vertex.position.x),
                std::max(maxExtents_.y, vertex.position.y),
                std::max(maxExtents_.z, vertex.position.z));
            positionOffset_ += vertex.position;
        }

        positionOffset_.x = 0;
        positionOffset_.y = 0;
        positionOffset_.z = 0;

        radius_ = std::max(
            minExtents_.GetMagnitude(),
            maxExtents_.GetMagnitude());
    }

private:
    std::experimental::optional<Buffer> vertexBuffer_;
    std::experimental::optional<Buffer> indexBuffer_;

    math::Vec3 positionOffset_{};

    math::Vec3 minExtents_;
    uint32_t vertexCount_ = 0;

    math::Vec3 maxExtents_;
    uint32_t indexCount_ = 0;

    float radius_ = 0;
};
} //namespace graphics
} //namespace poke
