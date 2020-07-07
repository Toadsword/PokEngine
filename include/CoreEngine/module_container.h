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
// Author : Nicolas Schneider
// Co-Author :
// Date : 18.11.19
//----------------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/graphic_engine.h>

#include <Inputs/input_manager.h>
#include <Scenes/scene_manager.h>
#include <Ecs/core_ecs_manager.h>
#include <PhysicsEngine/physics_engine.h>
#include <ResourcesManager/resources_manager_container.h>
#include <Chunks/chunks_manager.h>
#include <Ecs/Entities/tag_manager.h>
#include <Ecs/Archetypes/core_archetypes_manager.h>
#include <AudioEngine/audio_engine.h>

namespace poke {
//-----------------------------FORWARD DECLARATION----------------------------------
class Engine;

//----------------------------------------------------------------------------------

/**
 * \brief Contains all main modules
 */
struct ModuleContainer {
	ModuleContainer() = delete;
	explicit ModuleContainer(Engine& engine);
	graphics::GraphicEngine graphicsEngine;
	input::InputManager inputManager;
	scene::InnerSceneManager sceneManager;
	audio::AudioEngine audioEngine;
	//ecs::CoreEcsManager ecsManager;
	physics::PhysicsEngine physicsEngine;
	ResourcesManagerContainer resourcesManagerContainer;
	chunk::ChunksManager chunksManager;

	ecs::TagManager tagManager;
	ecs::CoreArchetypesManager archetypesManager;
};

} //namespace poke
