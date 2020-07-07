//----------------------------------------------------------------------------------
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
// Author : Robin Alves
// Co-Author : Nicolas Schneider
// Date : 28.02.2020
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/Entities/entity_mask.h>

//Include Components data struct files
#include <AudioEngine/audio_source.h>
#include <Ecs/Components/light.h>
#include <Ecs/Components/spline_follower.h>
#include <Ecs/Components/trail_renderer.h>
#include <Editor/Components/editor_component.h>
#include <GraphicsEngine/Models/model.h>
#include <GraphicsEngine/Particles/particle_system.h>
#include <Ecs/Components/trail_renderer.h>
#include <Ecs/Components/segment_renderer.h>
#include <Math/tranform.h>
#include <PhysicsEngine/collider.h>
#include <PhysicsEngine/rigidbody.h>

namespace poke {
namespace ecs {
/**
 * \brief An Archetypes is used to represents a certain type of entity. Its using it's own pool of entities and all entities inside an archetypes are aligned.
 */
class Archetype : public EntityMask {
public:
	Archetype();
	~Archetype() = default;

    virtual void SetFromJson(const json& archetypeJson);

	virtual json ToJson() const;

    //Component Data
	physics::Collider collider;
	graphics::Model model;
	physics::Rigidbody rigidbody;
	SplineFollower spline;
    math::Transform transform;
	Light light;
	audio::AudioSource audioSource;
	graphics::ParticleSystem particleSystem;
	TrailRenderer trailRenderer;
	SegmentRenderer segmentRenderer;
	EditorComponent editorComponent;

	size_t size;
};
}//namespace ecs
}//namespace poke