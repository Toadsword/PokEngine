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
// Co-Author :
// Date : 31.01.20
//----------------------------------------------------------------------------------

#pragma once
#include <optional_custom.h>

// core systems part
#include <CoreEngine/CoreSystems/spline_follower_system.h>
#include <CoreEngine/CoreSystems/draw_system.h>
#include <CoreEngine/CoreSystems/phyics_system.h>
#include <CoreEngine/CoreSystems/chunks_system.h>
#include <CoreEngine/CoreSystems/audio_system.h>
#include <CoreEngine/CoreSystems/light_system.h>
#include <CoreEngine/CoreSystems/particles_system.h>
#include <CoreEngine/CoreSystems/trail_renderer_system.h>
#include <CoreEngine/CoreSystems/segment_renderer_system.h>

namespace poke {
struct CoreSystemsContainer {
	CoreSystemsContainer(Engine& engine);
	void OnEngineInit();

	std::experimental::optional<SplineFollowerSystem> splineFollower = std::experimental::nullopt;
	std::experimental::optional<DrawSystem> drawSystem = std::experimental::nullopt;
	std::experimental::optional<PhysicsSystem> physicsSystem = std::experimental::nullopt;
	std::experimental::optional<ChunksSystem> chunksSystem = std::experimental::nullopt;
	std::experimental::optional<AudioSystem> audioSystem = std::experimental::nullopt;
	std::experimental::optional<LightSystem> lightSystem = std::experimental::nullopt;
	std::experimental::optional<ParticlesSystem> particlesSystem = std::experimental::nullopt;
	std::experimental::optional<TrailRendererSystem> trailRendererSystem = std::experimental::nullopt;
	std::experimental::optional<SegmentRendererSystem> segmentRendererSystem = std::experimental::nullopt;

private:
	Engine& engine_;
};
}//namespace poke