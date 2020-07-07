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
// Date : 28.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <Math/matrix.h>
#include <Utility/color.h>
#include <GraphicsEngine/Shaders/shader_vertex_input.h>
#include <GraphicsEngine/Descriptors/descriptor_handle.h>
#include <GraphicsEngine/Buffers/instance_buffer.h>

namespace poke {
namespace graphics {
class GraphicEngine;
class UniformHandle;
class PipelineGraphics;
class CommandBuffer;
class Mesh;

struct GizmoDrawCommand {
	math::Matrix4 worldMatrix;
    Color c;
};

/**
 * \brief It's an object to be used for GPU Instancing when drawing gizmo
 */
class GizmoInstances {
public:
    /**
     * \brief Represents ONE instance to render
     */
    class Instance {
    public:
        static VertexInput GetVertexInput(uint32_t baseBinding = 0);

        math::Matrix4 modelMatrix = math::Matrix4::Identity();
        Color color;
    };

    /**
     * \brief Create a unique_ptr of GizmoInstance and return it
     * \param mesh 
     * \param lineThickness 
     * \return 
     */
    static std::unique_ptr<GizmoInstances> Create(
        const Mesh& mesh,
        float lineThickness = 1.0f);

    /**
     * \brief 
     * \param mesh 
     * \param lineThickness 
     */
    explicit GizmoInstances(
        const Mesh& mesh,
        float lineThickness = 1.0f);

    ~GizmoInstances();

    /**
     * \brief Update the object with a list of DrawCommand and create instances.
     * \param gizmos 
     */
    void Update(std::vector<GizmoDrawCommand>& gizmos);

    /**
     * \brief Render all instance to this GizmoInstance
     * \param commandBuffer 
     * \param pipeline 
     * \param uniformScene 
     * \return 
     */
    bool CmdRender(
        const CommandBuffer& commandBuffer,
        const PipelineGraphics& pipeline,
        UniformHandle& uniformScene);

    const Mesh& GetMesh() const { return kMesh_; }

    const float& GetLineThickness() const { return lineThickness_; }

    void SetLineThickness(const float& lineThickness);

private:
    const Mesh& kMesh_;
    float lineThickness_;

    uint32_t maxInstances_;
    uint32_t instances_;

    DescriptorHandle descriptorSet_;
    InstanceBuffer instanceBuffer_;

    static const uint32_t kMaxInstances = 512;
};
} //namespace graphics
} //namespace poke
