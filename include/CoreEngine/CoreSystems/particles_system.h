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
// Date : 02.03.2020
//-----------------------------------------------------------------------------
#pragma once
#include <Ecs/system.h>

#include <Ecs/ComponentManagers/particle_systems_manager.h>
#include <GraphicsEngine/Particles/particle_command_buffer.h>
#include <Editor/ResourcesManagers/editor_materials_manager.h>
#include <CoreEngine/Camera/interface_camera.h>
#include <Ecs/Utility/entity_vector.h>

namespace poke {
class ParticlesSystem final : public ecs::System {
public:
    explicit ParticlesSystem(Engine& engine);
    ~ParticlesSystem() = default;

    void OnEntitySetActive(ecs::EntityIndex entityIndex) override;
    
    void OnEntitySetInactive(ecs::EntityIndex entityIndex) override;

private:
    void OnUpdate();

    void OnDraw();

    void OnCulling();

    void OnEndOfFrame();

    void OnEntityAddComponent(
        ecs::EntityIndex entityIndex,
        ecs::ComponentMask component) override;

    void OnEntityRemoveComponent(
        ecs::EntityIndex entityIndex,
        ecs::ComponentMask component) override;

	void OnEntityUpdateComponent(
		ecs::EntityIndex entityIndex,
		ecs::ComponentMask component);

    void OnUnloadScene();

    //Entities
	ecs::EntityVector newEntities_;
	ecs::EntityVector destroyedEntities_;
	ecs::EntityVector particleSystems_;

    //Managers
	ecs::ParticleSystemsManager& particleSystemsManager_;
	ecs::TransformsManager& transformsManager_;

    //Locators
	IMaterialsManager& materialManager_;
	ICamera& camera_;
	graphics::ParticleCommandBuffer& particleCommandBuffer_;

    //Drawing data
	std::vector<std::vector<graphics::ParticleDrawInfo>> particlesDrawing_;
	std::vector<std::vector<graphics::ParticleDrawInfo>> particlesRendering_;

    //Index of particle instance
	std::vector<int> particleInstanceIndexDrawing_;
	std::vector<int> particleInstanceIndexRendering_;

    //Particles
	struct Particle {
		int nbParticles;

		std::vector<uint32_t> numberOfRows;
		std::vector<Color> originalColor;
		std::vector<Color> colorOffset;

		std::vector<ColorGradient> colorOverLifetime;

		std::vector<math::Vec3> position;
		std::vector<math::Vec3> velocity;

		std::vector<math::Vec2> imageOffset1;
		std::vector<math::Vec2> imageOffset2;

		std::vector<float> lifetime;
		std::vector<float> scale;

		std::vector<float> elapsedTime;
		std::vector<float> transparency;
		std::vector<float> imageBlendFactor;
	};
    //Current particles
	std::vector<Particle> particles_;
};
} //namespace poke
