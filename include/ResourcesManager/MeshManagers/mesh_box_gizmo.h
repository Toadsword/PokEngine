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

#include <GraphicsEngine/Models/mesh.h>

namespace poke {
/**
 * \brief A mesh that represents a cube, this mesh must only be used when drawing gizmos
 */
class MeshBoxGizmo final : public graphics::Mesh {
public:
    /**
     * \brief Factory to create a Mesh cube specific for gizmo
     * \return 
     */
    static std::unique_ptr<MeshBoxGizmo> Create();

    /**
     * \brief 
     * \param load Only set true when the object is not movable/copyable (like a std::unique_ptr)
     */
    explicit MeshBoxGizmo(bool load);

    void Load() override;

private:
    inline static const std::vector<graphics::VertexMesh> kVertices = {
        graphics::VertexMesh(
            math::Vec3(-0.5f, -0.5f, 0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //left down front 
        graphics::VertexMesh(
            math::Vec3(0.5f, -0.5f, 0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //right down front 
        graphics::VertexMesh(
            math::Vec3(0.5f, 0.5f, 0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //right up front
        graphics::VertexMesh(
            math::Vec3(-0.5f, 0.5f, 0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //left up front
        graphics::VertexMesh(
            math::Vec3(-0.5f, -0.5f, -0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //left down back
        graphics::VertexMesh(
            math::Vec3(0.5f, -0.5f, -0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //right down back 
        graphics::VertexMesh(
            math::Vec3(0.5f, 0.5f, -0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        //right up back
        graphics::VertexMesh(
            math::Vec3(-0.5f, 0.5f, -0.5f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)) //left up back
    };

    inline static const std::vector<uint32_t> kIndices = {
        0,
        1,
        2,
        3,
        0,
        4,
        5,
        1,
        5,
        6,
        2,
        6,
        7,
        3,
        7,
        4
    };
};
} //namespace poke
