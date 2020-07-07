#include <Editor/Tools/component_tool.h>

#include <vector>
#include <string>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <CoreEngine/engine.h>
#include <Utility/json_utility.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Editor/component_utility.h>
#include <Editor/editor.h>
#include <Utility/profiler.h>
#include <Utility/log.h>
#include <AudioEngine/audio_source.h>

namespace poke::editor {

ComponentTool::ComponentTool(Editor& editor, const bool defaultActive)
	: Tool(defaultActive),
    editorEcsManager_(editor.GetEditorEcsManager()),
    transformsManager_(editorEcsManager_.GetComponentsManager<ecs::TransformsManager>()),
    rigidbodyManager_(editorEcsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
    collidersManager_(editorEcsManager_.GetComponentsManager<ecs::CollidersManager>()),
	splineFollowersManager_(editorEcsManager_.GetComponentsManager<ecs::SplineFollowersManager>()),
    modelsManager_(editorEcsManager_.GetComponentsManager<ecs::ModelsManager>()),
    lightsManager_(editorEcsManager_.GetComponentsManager<ecs::LightsManager>()),
    particleSystemsManager_(editorEcsManager_.GetComponentsManager<ecs::ParticleSystemsManager>()),
	audioSourcesManager_(editorEcsManager_.GetComponentsManager<ecs::AudioSourcesManager>()),
	editorComponentManager_(editorEcsManager_.GetComponentsManager<ecs::EditorComponentManager>()),
    trailRendererManager_(editorEcsManager_.GetComponentsManager<ecs::TrailRendererManager>()),
    segmentRendererManager_(editorEcsManager_.GetComponentsManager<ecs::SegmentRendererManager>()),

    // Game components
	enemiesManager_(editorEcsManager_.GetComponentsManager<game::EnemiesManager>()),
	playerManager_(editorEcsManager_.GetComponentsManager<game::PlayerManager>()),
	destructibleElementManager_(editorEcsManager_.GetComponentsManager<game::DestructibleElementManager>()),
	weaponManager_(editorEcsManager_.GetComponentsManager<game::WeaponManager>()),
	projectileManager_(editorEcsManager_.GetComponentsManager<game::ProjectileManager>()),
	missilesManager_(editorEcsManager_.GetComponentsManager<game::MissilesManager>()),
    specialAttackManager_(editorEcsManager_.GetComponentsManager<game::SpecialAttackManager>()),
    splineStatesManager_(editorEcsManager_.GetComponentsManager<game::SplineStateManager>()),
    jiggleManager_(editorEcsManager_.GetComponentsManager<game::JiggleManager>()),
	gameCameraManager_(editorEcsManager_.GetComponentsManager<game::GameCameraManager>()),
	// End Game components
    lastComponentType_(static_cast<unsigned int>(ecs::ComponentType::LENGTH)),
	editor_(editor)
{
    isActive_ = defaultActive;
    name_ = "Component Tool";
    toolFlag_ = TOOL_FLAG_COMPONENT;
}

void ComponentTool::OnSetInactive()
{
    prefabNames_.clear();
}

void ComponentTool::OnSetActive()
{
    std::string path = PokFileSystem::GetPath(
        FileType::PREFAB,
        FolderType::SAVE_IN_ROM);
    prefabNames_ = PokFileSystem::GetAllFilesInDirectories(
        path,
        true,
        false
    );
}

void ComponentTool::OnDrawImgui()
{
	pok_BeginProfiling(OnDrawComponentTool);

    DisplayEntitiesList();
    DisplayComponentList();

	pok_EndProfiling(OnDrawComponentTool);
}

void ComponentTool::DisplayEntitiesList()
{
    //Display Entities List Window
    std::vector<ecs::EntityIndex> entities = editorEcsManager_.GetNotEmptyEntities();
	if (entities.empty()) return;
    ImGui::Begin(entitiesListName_, &isActive_);

    //Check if selected entity exist
    if (std::find(entities.begin(), entities.end(), editor_.GetSelectedEntityIndex()) == entities.end()) {
		editor_.SetSelectedEntityIndex(entities[0]);
    }
	
	if (ImGui::Button("Create New Entity")) {
		ImGui::OpenPopup("new_entity_popup");
	}
	if (ImGui::BeginPopup("new_entity_popup")) {
		if (ImGui::MenuItem("Empty")) {
			const ecs::EntityIndex entityIndex = editorEcsManager_.AddEntity();
			editor_.SetSelectedEntityIndex(entityIndex);
		}
		for (const std::string& prefabName : prefabNames_) {
			// Remove extensionla 
			const size_t lastIndex = prefabName.find_last_of('.');
			const std::string newName = prefabName.substr(0, lastIndex);
			if (ImGui::MenuItem(newName.c_str())) { CreatePrefab(newName); }
		}
		ImGui::EndPopup();
	}

	const std::vector<ecs::EntityPool>& pools = editorEcsManager_.GetPools();
	size_t currentPoolNum = 0;
	bool lastTreeOpened = ImGui::TreeNodeEx("Entities");
    for (ecs::EntityIndex entity : entities) {
        // Check if we need to get to the next pool
		if (pools[currentPoolNum].lastEntity < entity) {
			if (lastTreeOpened) { ImGui::TreePop(); }

			currentPoolNum++;

			lastTreeOpened = ImGui::TreeNodeEx((
				std::to_string(currentPoolNum) + " : " + 
				editorEcsManager_.GetEntityName(entity).substr(
					0, 
					editorEcsManager_.GetEntityName(entity).find(':'))
				).c_str()
			);
		}

        //Display entity only if it's not a child
        if (transformsManager_.GetParent(entity) == ecs::kNoParent && lastTreeOpened) {
            DisplayEntity(entity);
        }
    }
	if (lastTreeOpened) { ImGui::TreePop(); }
    
    ImGui::End();
}

void ComponentTool::DisplayEntity(const ecs::EntityIndex entityIndex)
{
	pok_BeginProfiling(Get_All_Children, 0);
    std::vector<ecs::EntityIndex> childrenIndexes = GetAllChildren(entityIndex);
	pok_EndProfiling(Get_All_Children);
    const bool isParent = !childrenIndexes.empty();
    //Check if entity is a parent
    ImGuiTreeNodeFlags nodeFlags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (editor_.GetSelectedEntityIndex() == entityIndex) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }
    if (!isParent) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf |
            ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    Color textColor = kEntityColor_;
    if (CheckEntityIsPrefab(entityIndex)) {
        //Color blue if it's a prefab
        textColor = kPrefabColor_;
    }
    if (!editorEcsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) {
        //Decreases opacity if it's deactivate
        textColor.a = kInactiveEntityOpacity_;
    }
    ImGui::PushStyleColor(ImGuiCol_Text, PokToImGuiColor(textColor));

	std::string entityName = editorEcsManager_.GetEntityName(entityIndex);
	entityName.append("##EntityNum").append(std::to_string(entityIndex));

    //Display Object
    const bool nodeOpen = ImGui::TreeNodeEx(entityName.c_str(), nodeFlags);

    ImGui::PopStyleColor();
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("Entity", &entityIndex, sizeof(int));
        ImGui::Text(entityName.c_str());
        ImGui::EndDragDropSource();
    }
    // On DragDrop an Entity
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity") ) {
            IM_ASSERT(payload->DataSize == sizeof(int));
            const int payloadN = *static_cast<const int*>(payload->Data);
            const ecs::EntityIndex parentIndex =
                transformsManager_.GetParent(payloadN);

            //If child drop on his parent, remove child from parent
            if (transformsManager_.GetParent(payloadN) != ecs::kNoParent &&
                payloadN == entityIndex) {
                //Remove child from parent
                transformsManager_.SetParent(payloadN, ecs::kNoParent);
                transformsManager_.SetParent(entityIndex, ecs::kNoEntity);

                // If entity isn't drop on his children
            } else if (!CheckIsMyChildren(payloadN, entityIndex)) {
                //Set child in parent
                transformsManager_.SetParent(payloadN, entityIndex);
                transformsManager_.SetParent(payloadN, entityIndex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked()) {
		editor_.SetSelectedEntityIndex(entityIndex);
    }
    //On right-click
    if (ImGui::BeginPopupContextItem(entityName.c_str())) {
        if (ImGui::Button("Rename Entity")) {
            ImGui::OpenPopup("Rename");
            newEntityName_ = editorEcsManager_.GetEntityName(entityIndex);;
        }
        if (ImGui::BeginPopup("Rename")) {
            if (ImGui::InputTextWithHint(
                "Entity Name",
                "Enter entity name",
                &newEntityName_)) { }
            if (ImGui::Button("Rename")) {
                editorEcsManager_.SetEntityName(entityIndex, newEntityName_);
            }
            ImGui::EndPopup();
        }
        if (ImGui::Button("Delete Entity")) {
            editorEcsManager_.DestroyEntity(entityIndex);
            ImGui::CloseCurrentPopup();
        }
        if (CheckEntityIsPrefab(entityIndex)) {
            if (ImGui::Button("Unlink Prefab")) {
				ecs::EditorComponent editorComponent = editorEcsManager_.GetComponentsManager<ecs::EditorComponentManager>().GetComponent(entityIndex);
				editorComponent.prefabName = "";
				editorEcsManager_.GetComponentsManager<ecs::EditorComponentManager>().SetComponent(entityIndex, editorComponent);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    if (nodeOpen && isParent) {
        for (const ecs::EntityIndex childIndex : childrenIndexes) {
            DisplayEntity(childIndex); //Display child
        }
        ImGui::TreePop();
    }
}

const std::vector<ecs::EntityIndex>& ComponentTool::GetAllChildren(
    const ecs::EntityIndex entityIndex) const
{
    return transformsManager_.GetChildren(entityIndex);
}

bool ComponentTool::CheckIsMyChildren(
    const ecs::EntityIndex entityIndex,
    const ecs::EntityIndex childIndex) const
{
    if (transformsManager_.GetParent(childIndex) != ecs::kNoParent) {
        const ecs::EntityIndex parentIndex = transformsManager_.GetParent(childIndex);

		if (parentIndex == entityIndex) {
			return true;
		}

        return CheckIsMyChildren(childIndex, parentIndex);
    }
    return false;
}

void ComponentTool::DisplayComponentList()
{
    // Display Component list window
    ImGui::Begin(componentListName_, &isActive_);
    if (!editorEcsManager_.GetNotEmptyEntities().empty()) {
        //Active Flag
        bool isEntityActive =
            editorEcsManager_.HasComponent(
                editor_.GetSelectedEntityIndex(),
                ecs::EntityFlag::IS_ACTIVE);
        if (ImGui::Checkbox("IsActive", &isEntityActive)) {
            SetActiveEntity(editor_.GetSelectedEntityIndex(), isEntityActive);
        }

        bool isGraphicsActive = editorEcsManager_.IsEntityVisible(
			editor_.GetSelectedEntityIndex());
        ImGui::SameLine();
        ImGui::Checkbox("Graphics", &isGraphicsActive);

        //Tag
        auto entityTag = editorEcsManager_.GetTag(editor_.GetSelectedEntityIndex());
		const std::string tagName = TagManagerLocator::Get().GetTagName(entityTag);
		if (ImGui::CollapsingHeader(("Tag : " + tagName).c_str())) {
			const auto& tags = TagManagerLocator::Get().GetAllTagNames();
			for (auto i = 0; i < tags.size(); i++) {
				const std::string tag = tags[i] + " : " + std::to_string(i);
				if (ImGui::Selectable(&tag[0], entityTag == i)) {
					entityTag = i;
				}
			}
			editorEcsManager_.SetTag(editor_.GetSelectedEntityIndex(), entityTag);
		}
    }

    //Display all components
    for (unsigned int component = 1u << 0;
         component != lastComponentType_;
         component = component << 1) {
        DisplayComponent(
            static_cast<ecs::ComponentType::ComponentType>(component), 
			editor_.GetSelectedEntityIndex());
    }
	ImGui::NewLine();

    if (ImGui::Button("Create New Component")) {
        ImGui::OpenPopup("component_popup");
    }
    if (ImGui::BeginPopup("component_popup"))
        //Display new component list
    {
        for (unsigned int component = 1u << 0;
             component != lastComponentType_;
             component = component << 1) {
            const ecs::ComponentType::ComponentType componentType =
                static_cast<ecs::ComponentType::ComponentType>(component);
            const std::string componentName =
                ComponentUtility::GetComponentName(componentType);
            CreateComponent(componentName, componentType);
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}

void ComponentTool::SetActiveEntity(
    const ecs::EntityIndex entityIndex,
    bool value) const
{
    if (value) {
        editorEcsManager_.SetActive(entityIndex, ecs::EntityStatus::ACTIVE);
    } else {
        editorEcsManager_.SetActive(entityIndex, ecs::EntityStatus::INACTIVE);
    }
    std::vector<ecs::EntityIndex> childrenIndexes =
        GetAllChildren(entityIndex);
    for (ecs::EntityIndex childIndex : childrenIndexes) {
        SetActiveEntity(childIndex, value);
    }
}

void ComponentTool::DisplayComponent(
    ecs::ComponentType::ComponentType componentType,
	const ecs::EntityIndex entityIndex) const
{
	if (editorEcsManager_.HasComponent(entityIndex, componentType)) {
		//switch begin
		switch (componentType) {
		case ecs::ComponentType::EDITOR_COMPONENT: {
			const ecs::EditorComponent editorComponent = editorComponentManager_.GetComponent(entityIndex);
			const ecs::EditorComponent newEditorComponent = ComponentUtility::DisplayAndModifyEditorComponent(editorComponent);

			if (newEditorComponent != editorComponent) {
				editorComponentManager_.SetComponent(entityIndex, newEditorComponent);
			}
			break;
		}
		case ecs::ComponentType::TRANSFORM: {
			const math::Transform transform = transformsManager_.GetComponent(entityIndex);
			const math::Transform newTransform = ComponentUtility::DisplayAndModifyTransform(transform);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newTransform != transform) {
				transformsManager_.SetComponent(entityIndex, newTransform);
			}
			break;
		}
		case ecs::ComponentType::RIGIDBODY: {
			const auto rigidbody = rigidbodyManager_.GetComponent(entityIndex);

			const physics::Rigidbody newRigidbody =
				ComponentUtility::DisplayAndModifyRigidbody(rigidbody);

			if (newRigidbody != rigidbody) {
				rigidbodyManager_.SetComponent(entityIndex,
					newRigidbody);
			}
			break;
		}
		case ecs::ComponentType::COLLIDER: {
			const auto collider = collidersManager_.GetComponent(
				entityIndex);
			const physics::Collider newCollider =
				ComponentUtility::DisplayAndModifyCollider(collider);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newCollider != collider) {
				collidersManager_.SetComponent(
					entityIndex,
					newCollider);
			}
			break;
		}
		case ecs::ComponentType::MODEL: {
			auto model = modelsManager_.GetComponent(entityIndex);
			auto newModel = ComponentUtility::DisplayAndModifyModel(model);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newModel != model) {
				modelsManager_.SetComponent(entityIndex, newModel);
				editorEcsManager_.UpdateComponent(entityIndex, ecs::ComponentType::MODEL);
			}
			break;
		}
		case ecs::ComponentType::SPLINE_FOLLOWER: {
			const ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(entityIndex);
			const ecs::SplineFollower newSplineFollower = ComponentUtility::DisplayAndModifySplineFollower(splineFollower);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newSplineFollower != splineFollower) {
				splineFollowersManager_.SetComponent(entityIndex, newSplineFollower);
			}
			break;
		}
		case ecs::ComponentType::LIGHT: {
			const ecs::Light light = lightsManager_.GetComponent(entityIndex);
			const ecs::Light newLight = ComponentUtility::DisplayAndModifyLight(light);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newLight != light) {
				lightsManager_.SetComponent(entityIndex, newLight);
				editorEcsManager_.UpdateComponent(editor_.GetSelectedEntityIndex(), ecs::ComponentType::LIGHT);
			}
			break;
		}
		case ecs::ComponentType::PARTICLE_SYSTEM: {
			const graphics::ParticleSystem particle = particleSystemsManager_.GetComponent(entityIndex);
			const graphics::ParticleSystem newParticle = ComponentUtility::DisplayAndModifyParticleSystem(particle);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newParticle != particle) {
				particleSystemsManager_.SetComponent(entityIndex, newParticle);
			}
			break;
		}
		case ecs::ComponentType::AUDIO_SOURCE: {
			const audio::AudioSource audioSource = audioSourcesManager_.GetComponent(editor_.GetSelectedEntityIndex());
			const audio::AudioSource newAudioSource = ComponentUtility::DisplayAndModifyAudioSource(audioSource);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newAudioSource != audioSource) {
				audioSourcesManager_.SetComponent(editor_.GetSelectedEntityIndex(), newAudioSource);
			}
			break;
		}
		case ecs::ComponentType::TRAIL_RENDERER: {
			const ecs::TrailRenderer& trailRenderer = trailRendererManager_.GetComponent(entityIndex);
			const ecs::TrailRenderer newTrailRenderer = ComponentUtility::DisplayAndModifyTrailRenderer(trailRenderer);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newTrailRenderer != trailRenderer) {
				trailRendererManager_.SetComponent(entityIndex, newTrailRenderer);
			}
			break;
		}
		case ecs::ComponentType::SEGMENT_RENDERER: {
			const ecs::SegmentRenderer& segmentRenderer = segmentRendererManager_.GetComponent(entityIndex);
			const ecs::SegmentRenderer newSegmentRenderer = ComponentUtility::DisplayAndModifySegmentRenderer(segmentRenderer);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newSegmentRenderer != segmentRenderer) {
				segmentRendererManager_.SetComponent(entityIndex, newSegmentRenderer);
			}
			break;
		}

		case ecs::ComponentType::ENEMY: {
			const game::Enemy enemy = enemiesManager_.GetComponent(entityIndex);
			const game::Enemy newEnemy = ComponentUtility::DisplayAndModifyEnemy(enemy);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newEnemy != enemy) {
				enemiesManager_.SetComponent(entityIndex, newEnemy);
			}
			break;
			//switch end
		}
		case ecs::ComponentType::PLAYER: {
			const game::Player player = playerManager_.GetComponent(entityIndex);
			const game::Player newPlayer = ComponentUtility::DisplayAndModifyPlayer(player);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newPlayer != player) {
				playerManager_.SetComponent(entityIndex, newPlayer);
			}
			break;
		}
		case ecs::ComponentType::DESTRUCTIBLE_ELEMENT: {
			const game::DestructibleElement destructibleElement = destructibleElementManager_.
				GetComponent(entityIndex);
			const game::DestructibleElement newDestructibleElement =
				ComponentUtility::DisplayAndModifyDestructibleElement(destructibleElement);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newDestructibleElement != destructibleElement) {
				destructibleElementManager_.SetComponent(entityIndex, newDestructibleElement);
			}
			break;
		}
		case ecs::ComponentType::WEAPON: {
			const game::Weapon weapon = weaponManager_.GetComponent(entityIndex);
			const game::Weapon newWeapon = ComponentUtility::DisplayAndModifyWeapon(weapon);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newWeapon != weapon) {
				weaponManager_.SetComponent(entityIndex, newWeapon);
			}
			break;
		}
		case ecs::ComponentType::PROJECTILE: {
			const game::Projectile projectile = projectileManager_.GetComponent(entityIndex);
			const game::Projectile newProjectile = ComponentUtility::DisplayAndModifyProjectile(projectile);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newProjectile != projectile) {
				projectileManager_.SetComponent(entityIndex, newProjectile);
			}
			break;
		}
		case ecs::ComponentType::MISSILE: {
			const game::Missile missile = missilesManager_.GetComponent(entityIndex);
			const game::Missile newMissile = ComponentUtility::DisplayAndModifyMissile(missile);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newMissile != missile) {
				missilesManager_.SetComponent(entityIndex, newMissile);
			}
			break;
		}
		case ecs::ComponentType::SPECIAL_ATTACK: {
			const game::SpecialAttack specialAttack = specialAttackManager_.GetComponent(entityIndex);
			const game::SpecialAttack newSpecialAttack = ComponentUtility::DisplayAndModifySpecialAttack(specialAttack);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newSpecialAttack != specialAttack) {
				specialAttackManager_.SetComponent(entityIndex, newSpecialAttack);
			}
			break;
		}
		case ecs::ComponentType::SPLINE_STATES: {
			const game::SplineStates splineStates = splineStatesManager_.GetComponent(entityIndex);
			const game::SplineStates newSplineStates = ComponentUtility::DisplayandModifySplineStates(splineStates);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newSplineStates != splineStates) {
				splineStatesManager_.SetComponent(entityIndex, newSplineStates);
			}
			break;
		}
		case ecs::ComponentType::JIGGLE: {
			const game::Jiggle jiggle = jiggleManager_.GetComponent(entityIndex);
			const game::Jiggle newJiggle = ComponentUtility::DisplayAndModifyJiggle(jiggle);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newJiggle != jiggle) {
				jiggleManager_.SetComponent(entityIndex, newJiggle);
			}
			break;
		}
		case ecs::ComponentType::GAME_CAMERA: {
			const game::GameCamera gameCamera = gameCameraManager_.GetComponent(entityIndex);
			const game::GameCamera newGameCamera = ComponentUtility::DisplayAndModifyGameCamera(gameCamera);

			ComponentUtility::DisplayRemoveComponentButton(componentType);

			if (newGameCamera != gameCamera) {
				gameCameraManager_.SetComponent(entityIndex, newGameCamera);
			}
			break;
		}

		default: {
			break;
			//switch end
		}
		}
	}
}

void ComponentTool::CreateComponent(
    const std::string& componentName,
    const ecs::ComponentType::ComponentType
    componentType) const
{
    if (!editorEcsManager_.HasComponent(editor_.GetSelectedEntityIndex(), componentType)) {
        if (ImGui::MenuItem(componentName.c_str())) {
            editorEcsManager_.AddComponent(
				editor_.GetSelectedEntityIndex(),
                componentType);
        }
    }
}

void ComponentTool::CreatePrefab(const std::string& prefabName) const
{
	PrefabsManagerLocator::Get().AddPrefab(prefabName);
	std::vector<ecs::EntityIndex> entities = PrefabsManagerLocator::Get().Instantiate(prefabName);

	editor_.SetSelectedEntityIndex(entities[0]);

    if(!editorEcsManager_.HasComponent(entities[0], ecs::ComponentType::EDITOR_COMPONENT)) {
		editorEcsManager_.AddComponent(entities[0], ecs::ComponentType::EDITOR_COMPONENT);
    }
	ecs::EditorComponent editorComponent = editorEcsManager_.GetComponentsManager<ecs::EditorComponentManager>().GetComponent(entities[0]);
	editorComponent.name = prefabName;
	editorComponent.prefabName = prefabName;
	editorEcsManager_.GetComponentsManager<ecs::EditorComponentManager>().SetComponent(entities[0], editorComponent);
}

bool ComponentTool::CheckEntityIsPrefab(
	const ecs::EntityIndex entityIndex) const
{
	if (editorEcsManager_.HasComponent(entityIndex, ecs::ComponentType::EDITOR_COMPONENT)) {
		return !editorEcsManager_
	        .GetComponentsManager<ecs::EditorComponentManager>()
	        .GetComponent(entityIndex)
	        .prefabName.empty();
    }
	return false;
}
} //namespace poke
