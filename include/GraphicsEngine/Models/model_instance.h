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
// Date : 21.03.2020
//-----------------------------------------------------------------------------
#pragma once

#include <Math/matrix.h>
#include <GraphicsEngine/Models/mesh.h>
#include <GraphicsEngine/Descriptors/descriptor_handle.h>
#include <GraphicsEngine/Buffers/instance_buffer.h>
#include <GraphicsEngine/Buffers/uniform_handle.h>
#include <Math/hash.h>

namespace poke {
namespace graphics {
class PipelineGraphics;

struct InstancingDrawCmd {
	math::Matrix4 worldMatrix;
};

class ModelInstance {
public:
    class Instance {
    public:
        static VertexInput GetVertexInput(uint32_t baseBinding = 0);

        math::Matrix4 modelMatrix = math::Matrix4::Identity();
    };

    /**
     * \brief Factory to create an instance for GPU Instancing
     * \param mesh 
     * \param material 
     * \return 
     */
    static std::unique_ptr<ModelInstance> Create(
        const Mesh& mesh,
        const Material& material
    );

    /**
     * \brief 
     * \param mesh 
     * \param material 
     */
    explicit ModelInstance(
        const Mesh& mesh,
        const Material& material
    );

    /**
     * \brief Update instance's data to be sent to the GPU.
     * \param models 
     */
    void Update(std::vector<InstancingDrawCmd>& models);

    /**
     * \brief Bind Pipeline, Descriptor, Vertex, Index and Draw
     * \param commandBuffer 
     * \param uniformScene 
     * \return 
     */
    bool CmdRender(
        const CommandBuffer& commandBuffer,
        UniformHandle& uniformScene
    );

    const Mesh& GetMesh() const { return kMesh_; }

    const Material& GetMaterial() const { return kMaterial_; }
private:
    const Mesh& kMesh_;
    const Material& kMaterial_;

    uint32_t maxInstances_ = 0;
    uint32_t instances_ = 0;

    DescriptorHandle descriptorSet_;
    InstanceBuffer instanceBuffer_;
	UniformHandle uniformObject_;

	inline static const math::StringHash kUniformSceneHash =
		math::HashString("UniformScene");

	inline static const math::StringHash kUniformObjectHash =
		math::HashString("UniformObject");

    static const uint32_t kMaxInstances = 512;
};
} //namespace graphics
} //namespace poke
