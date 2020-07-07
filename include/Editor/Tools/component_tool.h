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
// Author : Luca Floreau
// Co-Author :
// Date : 11.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <string>
#include <map>

#include <Editor/Tools/tool.h>
#include <Ecs/ecs_utility.h>
#include <Editor/Ecs/editor_ecs_manager.h>

#include <Editor/ComponentManagers/editor_component_manager.h>
#include <Ecs/ComponentManagers/trail_renderer_manager.h>
#include "Ecs/ComponentManagers/segment_renderer_manager.h"

#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/spline_state_manager.h>
#include <Game/ComponentManagers/special_attack_manager.h>
#include <Game/ComponentManagers/jiggle_manager.h>
#include <Game/ComponentManagers/game_camera_manager.h>

namespace poke {
class ICamera;
class Engine;

namespace ecs {
class CoreEcsManager;
} //namespace ecs

namespace editor {
class Editor;

/**
 * \brief Tool that displays entities and components attached to each entity
 */
class ComponentTool : public Tool {
public:
    explicit ComponentTool(Editor& editor, bool defaultActive = false);

    void OnDrawImgui() override;

    void OnSetInactive() override;
    void OnSetActive() override;
private:
    /**
     * \brief Display all entities active or inactive with components
     */
    void DisplayEntitiesList();
    /**
     * \brief Display an entity and his children
     */
    void DisplayEntity(const ecs::EntityIndex entityIndex);

    /**
     * \brief Get all children of an entity
     */
    const std::vector<ecs::EntityIndex>& GetAllChildren(
        const ecs::EntityIndex entityIndex) const;

    bool CheckIsMyChildren(
        const ecs::EntityIndex entityIndex,
        const ecs::EntityIndex childIndex) const;

    /**
     * \brief Display all components of the selected entity
     */
    void DisplayComponentList();
    void SetActiveEntity(const ecs::EntityIndex entityIndex, bool value) const;
    void DisplayComponent(
		ecs::ComponentType::ComponentType componentType,
		const ecs::EntityIndex entityIndex) const;
    
    /**
     * \brief Create new component in the selected entity
     */
    void CreateComponent(
		const std::string& componentName,
        ecs::ComponentType::ComponentType componentType) const;

    /**
     * \brief Read prefab file and instantiate a new entity with all prefab component
     */
    void CreatePrefab(const std::string& prefabName) const;

    /**
     * \brief Check if current prefabs vector contains entityIndex
     */
    bool CheckEntityIsPrefab(ecs::EntityIndex entityIndex) const;

	Editor& editor_;

    EditorEcsManager& editorEcsManager_;
	ecs::TransformsManager& transformsManager_;
	ecs::RigidbodyManager& rigidbodyManager_;
	ecs::CollidersManager& collidersManager_;
	ecs::SplineFollowersManager& splineFollowersManager_;
	ecs::ModelsManager& modelsManager_;
	ecs::LightsManager& lightsManager_;
	ecs::ParticleSystemsManager& particleSystemsManager_;
	ecs::AudioSourcesManager& audioSourcesManager_;
	ecs::EditorComponentManager& editorComponentManager_;
	ecs::TrailRendererManager& trailRendererManager_;
	ecs::SegmentRendererManager& segmentRendererManager_;

    //Components game
	game::EnemiesManager& enemiesManager_;
	game::PlayerManager& playerManager_;
	game::DestructibleElementManager& destructibleElementManager_;
	game::WeaponManager& weaponManager_;
	game::ProjectileManager& projectileManager_;
	game::MissilesManager& missilesManager_;
    game::SpecialAttackManager& specialAttackManager_;
	game::SplineStateManager& splineStatesManager_;
    game::JiggleManager& jiggleManager_;
    game::GameCameraManager& gameCameraManager_;
    //!Components game

    const char* entitiesListName_ = "Hierarchy";
    const char* componentListName_ = "Inspector";

    unsigned int lastComponentType_;

    std::string newEntityName_ = "";
    const size_t kEntityNameBaseSize_ = 1500;

	std::vector<std::string> prefabNames_;
    
	const Color kPrefabColor_ = Color::aqua;
    const Color kEntityColor_ = Color::white;
    const float kInactiveEntityOpacity_ = 0.5f;

};
} //namespace editor
} //namespace poke
