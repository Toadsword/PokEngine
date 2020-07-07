
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
// Author : Guillaume Volygesi
// Date : 09.03.20
//----------------------------------------------------------------------------------
#pragma once
#include <Editor/Tools/tool.h> 
#include <Utility/file_system.h>
#include <Editor/component_utility.h>
#include <Editor/Ecs/editor_ecs_manager.h>

namespace poke::editor{


class ParticleTool : public Tool
{
public:

	explicit ParticleTool(Editor& editor, const bool defaultActive = false);

	void OnDrawImgui() override;

	void OnSetActive() override;

	void OnSetInactive() override;


private:

	void SaveParticle(const ecs::EntityIndex entityIndex, 
		const json& particleJson, const std::string& newParticleName);

	json WriteParticle(const ecs::EntityIndex entityIndex,
		const std::string& particleName)const;

	void GetAllParticleSystems();

	void DisplayParticlesSystems();

	std::vector<std::string> particlesNames_;

	const char* kParticlesName_ = "Particles";

	EditorEcsManager& ecsManager_;
	ecs::ParticleSystemsManager& particleManager_;

	std::vector<json> currentParticles_;

	void CreateNewParticleSystem(const ecs::EntityIndex entityIndex);

	bool CheckIfEntityIsParticle(
		const ecs::EntityIndex entityIndex);

	void DisplayParticleComponent(const ecs::EntityIndex entityIndex);
	ecs::EntityIndex selectedEntityIndex_;

	std::string newParticleName_ = "";
	std::string entityName_ = "";

	std::vector<ecs::EntityIndex> entities_;

	void LoadParticle(const std::string& particleName);

	void LoadParticleFromJson(
		const json& prefabJson,
		const ecs::EntityIndex
		entityIndex);

};
}//namespace poke::editor
