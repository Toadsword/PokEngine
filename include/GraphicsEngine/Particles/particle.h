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
// Date : 25.02.20
//-----------------------------------------------------------------------------
#pragma once
#include <Math/vector.h>
#include <Utility/color_gradient.h>

namespace poke {
namespace graphics {

struct ParticleDrawInfo {
    bool operator==(const ParticleDrawInfo& other) const;

    bool operator!=(const ParticleDrawInfo& other) const;

	math::Vec3 position; // 12 
	Color colorOffset; // 12 -> 24

	math::Vec2 imageOffset1; // 8 -> 32
	float imageBlendFactor = 0.0f; // 4 -> 36

	math::Vec2 imageOffset2; // 8 -> 44
	float transparency = 1.0f; // 4 -> 48
	float scale; //4 -> 52 + offset -> 60
};

struct Particle {
    /**
     * \brief Creates a new particle.
     * \param position The particles initial position.
     * \param velocity The particles initial velocity.
     * \param lifeLength The particles life length.
     * \param scale The particles scale.
     * \param gravityEffect The particles gravity effect.
     * \param colorOffset
     * \param colorOverLifetime
     */
    Particle(
        math::Vec3 position,
        math::Vec3 velocity,
        float lifeLength,
        float scale,
        float gravityEffect,
		Color colorOffset,
	    ColorGradient colorOverLifetime);

	uint32_t numberOfRows = 1;
	Color originalColor;
	Color colorOffset;

	ColorGradient colorOverLifetime;

    math::Vec3 position;
    math::Vec3 velocity;
    math::Vec3 change;

    math::Vec2 imageOffset1;
    math::Vec2 imageOffset2;

    float lifetime;
    float scale;
    float gravityEffect;

    float elapsedTime = 0.0f;
    float transparency = 1.0f;
    float imageBlendFactor = 0.0f;
    float distanceToCamera = 0.0f;
};
} //namespace graphics
} //namespace poke
