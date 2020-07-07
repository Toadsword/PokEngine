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
// Co-Author : Duncan Bourquard
// Date : 28.03.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Game/ResourcesManager/PrefabsManager/game_prefabs_manager.h>
#include <Editor/Ecs/editor_prefab.h>

namespace poke {
namespace editor {

class EditorPrefabsManager : public IPrefabsManager {
public:
	//------------------------------ OVERRIDE ---------------------------------
	EditorPrefabsManager();
	virtual ~EditorPrefabsManager() = default;

	std::vector<ecs::EntityIndex> Instantiate(
		const nonstd::string_view& prefabName) const override;

    void AddPrefab(const std::string& prefabName) override;

	const ecs::Prefab& GetPrefab(
		const nonstd::string_view& prefabName) const override;

	const std::vector<ResourceID>& GetPrefabsIDs() const override;

    void Clear() override;

	void Resize(const size_t newSize) override;

	json ToJson() override;

	void SetFromJson(const json& prefabsJson) override;
	//-------------------------------------------------------------------------


	//------------------------- EDITOR SPECIFIC -------------------------------
    /**
     * \brief Get all registered prefab names.
     * \return 
     */
    const std::vector<std::string>& GetAllPrefabsNames() const;

    /**
     * \brief Set new values for a prefab. WARNING it will also write in the json file.
     * \param prefab 
     * \param name 
     */
    void SetPrefab(const editor::EditorPrefab& prefab, const std::string& name);

	/**
	 * \brief Set new values for a prefab. WARNING it will also write in the json file.
	 * \param prefab
	 * \param prefabID
	 */
    void SetPrefab(const editor::EditorPrefab& prefab, ResourceID prefabID);

    /**
     * \brief Add a new prefab to the manager. WARNING It will create a new json file for it. If the name is already taken no new prefab will be created
     * \param prefab 
     * \param name 
     */
    void AddPrefab(const editor::EditorPrefab& prefab, const std::string& name);
	//-------------------------------------------------------------------------
private:
	std::vector<ResourceID> prefabsID_;
	std::vector<editor::EditorPrefab> prefabs_;
	std::vector<std::string> prefabsNames_;
};
} //namespace editor
} //namespace poke
