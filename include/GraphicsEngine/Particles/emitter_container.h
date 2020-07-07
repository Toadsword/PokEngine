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
// Date : 09.04.2020
//-----------------------------------------------------------------------------
#pragma once
#include <GraphicsEngine/Particles/Emitters/emitter.h>
#include <GraphicsEngine/Particles/Emitters/emitter_line.h>
#include <GraphicsEngine/Particles/Emitters/emitter_circle.h>
#include <GraphicsEngine/Particles/Emitters/emitter_point.h>
#include <GraphicsEngine/Particles/Emitters/emitter_sphere.h>
#include <GraphicsEngine/Particles/Emitters/emitter_cone.h>

namespace poke {
namespace graphics {

struct EmitterContainer {
    EmitterContainer();
    EmitterContainer(Emitter& emitter, EmitterType type);
    ~EmitterContainer();

    bool operator==(const EmitterContainer& other) const;
    bool operator!=(const EmitterContainer& other) const;

    EmitterContainer(const EmitterContainer& other);
    EmitterContainer(EmitterContainer&& other) noexcept;
    EmitterContainer& operator=(const EmitterContainer& other);
    EmitterContainer& operator=(EmitterContainer&& other) noexcept;

    math::Vec3 GetSpawnPos() const;

	math::Vec3 GetDirection(const math::Vec3& spawnPos) const;

	void SetFromJson(const json& emitterJson);

	json ToJson() const;

    EmitterType type;

    union {
        EmitterLine line{};
        EmitterPoint point;
        EmitterSphere sphere;
        EmitterCircle circle;
		EmitterCone cone;
    };
};
} //namespace graphics
} //namespace poke
