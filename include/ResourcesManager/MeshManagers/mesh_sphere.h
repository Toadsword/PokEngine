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
 * \brief A mesh to represents a sphere.
 */
class MeshSphere final : public graphics::Mesh {
public:
    /**
     * \brief Factory to create a sphere.
     * \param radius
     * \param latitudeBands
     * \param longitudeBands 
     * \return 
     */
    static std::unique_ptr<MeshSphere> Create(
        float radius = kDefaultRadius,
        int latitudeBands = kDefaultLatitudeBands,
        int longitudeBands = kDefaultLongitudeBands
    );

    /**
     * \brief 
     * \param load Only set true when the object is not movable/copyable (like a std::unique_ptr)
     * \param radius 
     * \param latitudeBands
     * \param longitudeBands 
     */
    explicit MeshSphere(
        bool load,
        float radius = kDefaultRadius,
        int latitudeBands = kDefaultLatitudeBands,
        int longitudeBands = kDefaultLongitudeBands
    );

    void Load() override;
private:
    float radius_;

    uint32_t latitudeBands_;
    uint32_t longitudeBands_;

    inline static const uint32_t kDefaultLatitudeBands = 40;
    inline static const uint32_t kDefaultLongitudeBands = 40;
    inline static const float kDefaultRadius = 1.0f;
};
} //namespace graphics