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
// Date : 26.02.20
//-----------------------------------------------------------------------------
#pragma once
#include <GraphicsEngine/Particles/emitter_container.h>
#include <Utility/color_gradient.h>
#include <GraphicsEngine/Particles/particle.h>
#include <ResourcesManager/resource_type.h>


namespace poke {
namespace graphics {
class ParticleSystem {
public:
    ParticleSystem();

    ~ParticleSystem() = default;

    bool operator==(const ParticleSystem& other) const;

    bool operator!=(const ParticleSystem& other) const;

    /**
     * \brief Generate a normalized random vector.
     * \param coneDirection 
     * \param angle 
     * \return 
     */
    math::Vec3 RandomUnitVectorWithinCone(
        math::Vec3 coneDirection,
        float angle) const;

    /**
     * \brief Emit one particle from.
     * \param worldPos 
     * \return 
     */
    Particle EmitParticle(math::Vec3 worldPos) const;

    /**
     * \brief Update the lifetime of the system
     * \param dt 
     * \return true if the system is still alive
     */
    bool UpdateLifetime(float dt);

    /**
     * \brief Generate a random value
     * \param average 
     * \param errorPercent 
     * \return 
     */
    static float GenerateValue(float average, float errorPercent);

    math::Vec3 GenerateRandomUnitVector() const;

    void SetFromJson(const json& particleSystemJson);
    json ToJson() const;

    //Control
    float timeEmit = 0.0f;
    float lifetime = 0.0f;
	float distanceEmit = 0.0f;
	math::Vec3 previousPos;

    //Particle System
    float duration = 5.0f;           
    bool looping = true;             
    float startDelay = 0.0f;         
    float minLifetime = 1.0f;      
    float maxLifetime = 1.0f;
    float minSpeed = 1.0f;
	float maxSpeed = 1.0f;
    float minSize = 1.0f;          
	float maxSize = 1.0f;
    float startRotation = 0.0f;      //TODO(@Nico) implement this.
    ColorGradient startColor; 
    float gravityModifier = 0.0f;    
    int maxParticles = 1000;    

    //Emission
    float rateOverTime = 5.0f;     
    float rateOverDistance = 0.0f; 

    //Shape
	EmitterContainer emitter;
	float randomizeDirection = 0.0f;
	float randomizePosition = 0.0f;

    //Color over lifetime
	ColorGradient colorOverLifetime;

    //Material
	ResourceID materialID = math::HashString("default_particle");
};
} //namespace graphics
} //namespace poke
