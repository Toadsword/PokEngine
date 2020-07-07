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
// Date : 09.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <Math/vector.h>
#include <Math/math.h>
#include <GraphicsEngine/Shaders/shader_vertex_input.h>

namespace poke {
namespace graphics {
struct VertexMesh {
	VertexMesh() = default;
    /**
     * \brief 
     * \param pos 
     * \param uv 
     * \param normal 
     */
    VertexMesh(math::Vec3 pos, math::Vec2 uv, math::Vec3 normal);

    static VertexInput GetVertexInput(uint32_t binding = 0);

    bool operator==(const VertexMesh& other) const;

    bool operator!=(const VertexMesh& other) const { return !(*this == other); }

    math::Vec3 position;
    math::Vec2 uv;
    math::Vec3 normal;
};
} //namespace graphics
} //namespace poke

namespace std {
template <>
struct hash<poke::graphics::VertexMesh> {
    size_t operator()(const poke::graphics::VertexMesh& vertex) const noexcept
    {
        size_t seed = 0;
        HashCombine(seed, vertex.position);
        HashCombine(seed, vertex.uv);
        HashCombine(seed, vertex.normal);
        return seed;
    }
};
} //namespace std
