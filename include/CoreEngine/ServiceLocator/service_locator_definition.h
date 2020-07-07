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
// Date : 28.01.20
//-----------------------------------------------------------------------------
#pragma once
#include <CoreEngine/ServiceLocator/service_locator.h>
#include <GraphicsEngine/null_graphic_engine.h>
#include <ResourcesManager/TextureManagers/null_texture_manager.h>
#include <ResourcesManager/MeshManagers/null_mesh_manager.h>
#include <Ecs/null_ecs_manager.h>
#include <Scenes/null_scene_manager.h>
#include <Inputs/null_input_manager.h>
#include <Chunks/null_chunk_manager.h>
#include <CoreEngine/Camera/null_camera.h>
#include <ResourcesManager/MaterialsManager/null_materials_manager.h>
#include <CoreEngine/World/null_world.h>
#include <PhysicsEngine/null_physics_engine.h>
#include <Ecs/Entities/null_tag_manager.h>
#include <Ecs/Archetypes/null_archetypes_manager.h>
#include <AudioEngine/null_audio_engine.h>
#include <ResourcesManager/SoundsManagers/null_sounds_manager.h>
#include <ResourcesManager/PrefabsManager/null_prefab_manager.h>

namespace poke {

using GraphicsEngineLocator = ServiceLocator<graphics::IGraphicEngine, graphics::NullGraphicEngine>;

using TextureManagerLocator = ServiceLocator<ITextureManager, NullTextureManager>;

using MeshManagerLocator = ServiceLocator<IMeshManager, NullMeshManager>;

using EcsManagerLocator = ServiceLocator<ecs::IEcsManager, ecs::NullEcsManager>;

using InputManagerLocator = ServiceLocator<input::IInputManager, input::NullInputManager>;

using SceneManagerLocator = ServiceLocator<scene::ISceneManager, scene::NullSceneManager>;

using ChunksManagerLocator = ServiceLocator<chunk::IChunksManager, chunk::NullChunksManager>;

using CameraLocator = ServiceLocator<ICamera, NullCamera>;

using MaterialsManagerLocator = ServiceLocator<IMaterialsManager, NullMaterialsManager>;

using WorldLocator = ServiceLocator<IWorld, NullWorld>;

using PhysicsEngineLocator = ServiceLocator<physics::IPhysicsEngine, physics::NullPhysicsEngine>;

using TagManagerLocator = ServiceLocator<ecs::ITagManager, ecs::NullTagManager>;

using ArchetypesManagerLocator = ServiceLocator<ecs::IArchetypesManager, ecs::NullArchetypesManager>;

using AudioEngineLocator = ServiceLocator<audio::IAudioEngine, audio::NullAudioEngine>;

using SoundsManagerLocator = ServiceLocator<ISoundsManager, NullSoundsManager>;

using PrefabsManagerLocator = ServiceLocator<IPrefabsManager, NullPrefabsManager>;
} //namespace poke
