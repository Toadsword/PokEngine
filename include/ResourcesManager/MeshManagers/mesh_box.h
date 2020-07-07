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
 * \brief Mesh that represents a cube with faces
 */
class MeshBox final : public graphics::Mesh {
public:
    /**
     * \brief Factory to create a MeshBox
     * \param extents 
     * \return 
     */
    static std::unique_ptr<MeshBox> Create(
        math::Vec3 extents = math::Vec3(1.0f, 1.0f, 1.0f)
    );

    /**
     * \brief 
     * \param load Only set true when the object is not movable/copyable (like a std::unique_ptr)
     * \param extents 
     */
    explicit MeshBox(
        bool load,
        math::Vec3 extents = math::Vec3(1.0f, 1.0f, 1.0f)
    );

    void Load() override;
private:
    math::Vec3 extents_;

    inline static const std::vector<graphics::VertexMesh> kVertices = {
        graphics::VertexMesh({-0.5f, -0.5f, 0.5f}, {0.375f, 1.0f}, {-1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, 0.5f, 0.5f}, {0.625f, 1.0f}, {-1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, -0.5f, -0.5f}, {0.375f, 0.75f}, {-1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, 0.5f, -0.5f}, {0.625f, 0.75f}, {0.0f, 0.0f, -1.0f}),
        graphics::VertexMesh({0.5f, -0.5f, 0.5f}, {0.375f, 0.25f}, {1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({0.5f, 0.5f, 0.5f}, {0.625f, 0.25f}, {0.0f, 0.0f, 1.0f}),
        graphics::VertexMesh({0.5f, -0.5f, -0.5f}, {0.375f, 0.5f}, {0.0f, 0.0f, -1.0f}),
        graphics::VertexMesh({0.5f, 0.5f, -0.5f}, {0.625f, 0.5f}, {1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, -0.5f, -0.5f}, {0.375f, 0.75f}, {0.0f, 0.0f, -1.0f}),
        graphics::VertexMesh({0.5f, -0.5f, -0.5f}, {0.375f, 0.5f}, {1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, -0.5f, 0.5f}, {0.375f, 0.0f}, {0.0f, 0.0f, 1.0f}),
        graphics::VertexMesh({0.5f, -0.5f, 0.5f}, {0.375f, 0.25f}, {0.0f, 0.0f, 1.0f}),
        graphics::VertexMesh({0.5f, -0.5f, -0.5f}, {0.375f, 0.5f}, {0.0f, -1.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, -0.5f, 0.5f}, {0.125f, 0.25f}, {0.0f, -1.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, -0.5f, -0.5f}, {0.125f, 0.5f}, {0.0f, -1.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, 0.5f, -0.5f}, {0.875f, 0.5f}, {0.0f, 1.0f, 0.0f}),
        graphics::VertexMesh({0.5f, 0.5f, 0.5f}, {0.625f, 0.25f}, {0.0f, 1.0f, 0.0f}),
        graphics::VertexMesh({0.5f, 0.5f, -0.5f}, {0.625f, 0.5f}, {0.0f, 1.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, 0.5f, -0.5f}, {0.625f, 0.75f}, {-1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({0.5f, 0.5f, -0.5f}, {0.625f, 0.5f}, {0.0f, 0.0f, -1.0f}),
        graphics::VertexMesh({0.5f, 0.5f, 0.5f}, {0.625f, 0.25f}, {1.0f, 0.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, 0.5f, 0.5f}, {0.625f, 0.0f}, {0.0f, 0.0f, 1.0f}),
        graphics::VertexMesh({0.5f, -0.5f, 0.5f}, {0.375f, 0.25f}, {0.0f, -1.0f, 0.0f}),
        graphics::VertexMesh({-0.5f, 0.5f, 0.5f}, {0.875f, 0.25f}, {0.0f, 1.0f, 0.0f}),
    };

    inline const static std::vector<uint32_t> kIndices = {
        1,
        2,
        0,
        // Front
        3,
        6,
        8,
        7,
        4,
        9,
        // Back
        5,
        10,
        11,
        12,
        13,
        14,
        // Top
        15,
        16,
        17,
        1,
        18,
        2,
        // Bottom
        3,
        19,
        6,
        7,
        20,
        4,
        // Left
        5,
        21,
        10,
        12,
        22,
        13,
        // Right
        15,
        23,
        16,
    };
};
} //namespace poke
