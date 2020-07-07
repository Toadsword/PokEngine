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
// Author : Duncan Bourquard
// Co-Author :
// Date : 05.03.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Editor/Tools/tool.h>
#include <imgui.h>
#include <Utility/color.h>
#include <CoreEngine/engine.h>
#include <Editor/editor.h>

namespace poke
{
namespace editor
{
class Showroom : public Tool 
{
public:
	explicit Showroom(Editor& editor, bool defaultActive = false);

	void OnDrawImgui() override;

	void OnSetInactive() override;

	void OnSetActive() override;
	
private:
	void LoadMesh(const std::string& meshName) const;
	void SetMesh(const std::string& meshName) const;
	void LoadMaterial(const std::string& materialName) const ;
	void SetMaterial(const std::string& materialName) const;
	void SetSkybox(const std::string& skyboxName);

	void CreateDefaultMaterial(
		const std::string& newMaterialName, 
		bool setToActualObject) const;

    ecs::EntityIndex AddModelEntity();
    int UpdateIndex(
		const std::vector<std::string>& list, 
		XXH64_hash_t baseHash);

	void DisplayEntities();
	void DisplayEntity(const ecs::EntityIndex entityIndex);
	void DisplayComponentWindow() const;
	int DisplayList(
		std::vector<std::string>& filepathList,
		const FileType fileType,
		const char* label,
		int index,
		XXH64_hash_t baseHash) const;

	void RefreshFilePaths();

	scene::Scene showroomScene_{ 51, "ShowroomScene", "ShowroomScene" };
	scene::SceneIndex fromSceneIndex_;

	Editor& editor_;
	ecs::ModelsManager& modelsManager_;

	ecs::EntityIndex selectedEntityIndex_ = ecs::EntityIndex();
	std::vector<ecs::EntityIndex> showroomEntities_ = std::vector<ecs::EntityIndex>();

	std::vector<std::string> meshFilePaths_;
	std::vector<std::string> skyboxFilePaths_;
	std::vector<std::string> materialDefaultFilePaths_;
	std::vector<std::string> materialForwardFilePaths_;

	int meshIndex_ = -1;
	int materialDefaultIndex_ = -1;
	int skyboxIndex_ = -1;

	XXH64_hash_t currentSkyboxHash_;
    
	std::vector<std::string> entitiesNames_{};
	std::string newName_ = "";
	bool setWithNew_ = false;
	bool isDirty_ = false;

	const size_t kEntityNameBaseSize_ = 10;
	const float kDragSpeed_ = 0.1f;
	static const size_t first_entity_index = 2;
};
} // namespace editor
} // namespace poke
