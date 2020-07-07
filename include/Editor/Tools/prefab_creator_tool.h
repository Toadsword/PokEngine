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
// Co-Author :
// Date : 14.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Editor/Tools/tool.h> 
#include <Utility/file_system.h>

#include <Editor/component_utility.h>
#include <Editor/Ecs/editor_prefab.h>
#include <Editor/Tools/component_tool.h>

namespace poke {

class ICamera;
class Engine;

namespace ecs {
class CoreEcsManager;
}//namespace ecs

namespace editor {
class Editor;

/**
* \brief prefab creator tool
*/
class PrefabCreatorTool : public Tool
{
public:

	explicit PrefabCreatorTool(Editor& editor, bool defaultActive = false);

	void OnDrawImgui() override;

	void OnSetActive() override;

	void OnSetInactive() override;

private:
	void DisplayComponent(
		const ecs::ComponentType::ComponentType componentType, 
		const ecs::EntityIndex entityIndex) const;
	void DisplayPrefabList();
	void DisplayEntity(const ecs::EntityIndex entityIndex);
	void DisplayComponentList(ecs::EntityIndex entityIndex);
	void CreateComponent(
		const std::string& componentName,
		const ecs::ComponentType::ComponentType
		componentType)const;
	void CreatePrefabInScene( const int index);

	/**
	* \brief Save selected entity as a Json and write in prefab file
	*/
	void SavePrefab(const std::string& prefabName);

	void GetAllPrefab();

	scene::Scene prefabScene_{ 50, "PrefabCreator", "PrefabCreatorScene" };

	//prefab variables
	std::vector<std::string> prefabNames_{};
	std::string selectedPrefabName_ = "";
	std::string newPrefabName_ = "";
	std::vector<json> currentPrefabs_{};

	//Entities variables
	std::vector<std::string> entitiesNames_{};
	const size_t kEntityNameBaseSize_ = 1000;

	EditorEcsManager& ecsManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	ecs::CollidersManager& collidersManager_;
	ecs::SplineFollowersManager& splineFollowersManager_;
	ecs::ModelsManager& modelsManager_;
	ecs::LightsManager& lightManager_;
	ecs::ParticleSystemsManager& particlesManager_;
	ecs::AudioSourcesManager& audioSourceManager_;
	ecs::TrailRendererManager& trailRendererManager_;
	ecs::SegmentRendererManager& segmentRendererManager_;
	ecs::EditorComponentManager& editorComponentManager_;

	//game components//
	game::EnemiesManager& enemiesManager_;
	game::PlayerManager& playerManager_;
	game::DestructibleElementManager& destructibleElementManager_; 
	game::WeaponManager& weaponManager_;
	game::ProjectileManager& projectileManager_; 
	game::MissilesManager& missileManager_; 
	game::SpecialAttackManager& specialAttackManager_; 
	game::SplineStateManager& splineStatesManager_;
	game::JiggleManager& jiggleManager_;
	game::GameCameraManager& gameCameraManager_;
	Editor& editor_;

	//input player
	//Editor component

	unsigned int lastComponentType_;

	const char* kPrefabsName_ = "Prefabs"; 
	const char* kComponentListName_ = "Components";

	int defaultSceneIndex_;
	int prefabSceneIndex_ = 4; 
	//for game prefab and archetypes
	EditorPrefab editorPrefab_; 

	std::vector<ecs::EntityIndex> prefabEntities_;
	inline static const char* prefabCurrent = nullptr;
	std::vector<ecs::EntityIndex> entityToDestroy;
};
}//namespace editor
}//namespace poke