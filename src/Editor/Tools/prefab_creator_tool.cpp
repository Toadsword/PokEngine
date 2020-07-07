#include <Editor/Tools/prefab_creator_tool.h>
#include <imgui.h>

#include <CoreEngine/engine.h>
#include <Utility/file_system.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Editor/editor.h>

namespace poke {
namespace editor {

PrefabCreatorTool::PrefabCreatorTool(Editor& editor, const bool defaultActive)
    : Tool(defaultActive),
      ecsManager_(editor.GetEditorEcsManager()),
      transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
      rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
      collidersManager_(ecsManager_.GetComponentsManager<ecs::CollidersManager>()),
      splineFollowersManager_(ecsManager_.GetComponentsManager<ecs::SplineFollowersManager>()),
      modelsManager_(ecsManager_.GetComponentsManager<ecs::ModelsManager>()),
      lightManager_(ecsManager_.GetComponentsManager<ecs::LightsManager>()),
      particlesManager_(ecsManager_.GetComponentsManager<ecs::ParticleSystemsManager>()),
      audioSourceManager_(ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>()),
      trailRendererManager_(ecsManager_.GetComponentsManager<ecs::TrailRendererManager>()),
      editorComponentManager_(ecsManager_.GetComponentsManager<ecs::EditorComponentManager>()),
      enemiesManager_(ecsManager_.GetComponentsManager<game::EnemiesManager>()),
      playerManager_(ecsManager_.GetComponentsManager<game::PlayerManager>()),
      destructibleElementManager_(ecsManager_.GetComponentsManager<game::DestructibleElementManager>()),
      weaponManager_(ecsManager_.GetComponentsManager<game::WeaponManager>()),
      projectileManager_(ecsManager_.GetComponentsManager<game::ProjectileManager>()),
      missileManager_(ecsManager_.GetComponentsManager<game::MissilesManager>()),
      specialAttackManager_(ecsManager_.GetComponentsManager<game::SpecialAttackManager>()),
      splineStatesManager_(ecsManager_.GetComponentsManager<game::SplineStateManager>()),
      jiggleManager_(ecsManager_.GetComponentsManager<game::JiggleManager>()),
      gameCameraManager_(ecsManager_.GetComponentsManager<game::GameCameraManager>()),
      lastComponentType_(static_cast<unsigned int>(ecs::ComponentType::LENGTH)),
      segmentRendererManager_(ecsManager_.GetComponentsManager<ecs::SegmentRendererManager>()),
      editor_(editor)
{
    isActive_ = defaultActive;
    name_ = "Prefab Creator Tool";
    toolFlag_ = TOOL_FLAG_PREFAB_CREATOR;
}

void PrefabCreatorTool::OnSetActive()
{
    defaultSceneIndex_ = SceneManagerLocator::Get().GetActiveSceneIndex();
	if (defaultSceneIndex_ != prefabSceneIndex_) {
		SceneManagerLocator::Get().LoadScene(prefabSceneIndex_);
	}
    GetAllPrefab();
    entitiesNames_.resize(kEntityNameBaseSize_);
	prefabCurrent = prefabNames_[0].c_str();
}

void PrefabCreatorTool::OnSetInactive()
{
    prefabNames_.clear();
    entitiesNames_.clear();
	if (defaultSceneIndex_ != prefabSceneIndex_) {
		SceneManagerLocator::Get().LoadScene(defaultSceneIndex_);
	}
}

void PrefabCreatorTool::OnDrawImgui()
{
    //Display prefab list
    DisplayPrefabList();
    //Display component window
    DisplayComponentList(editor_.GetSelectedEntityIndex());
}

void PrefabCreatorTool::DisplayPrefabList()
{
    ImGui::Begin(kPrefabsName_, &isActive_);

    if (ImGui::Button("CreatePrefab")) {
        selectedPrefabName_ = "new prefab";

        ecsManager_.DestroyEntities(prefabEntities_);
        editorPrefab_.Clear();

        const ecs::EntityIndex newIndex = ecsManager_.AddEntity();
        ecsManager_.AddComponent(newIndex, ecs::ComponentType::TRANSFORM);
        prefabEntities_.push_back(newIndex);
    }

	if (ImGui::BeginCombo(kPrefabsName_, prefabCurrent)) {

		for (size_t i = 0; i < prefabNames_.size(); i++) {

			const bool isSelected = prefabCurrent == prefabNames_[i].c_str();

			if (ImGui::Selectable(prefabNames_[i].c_str(), isSelected)) {
				prefabCurrent = prefabNames_[i].c_str();
				CreatePrefabInScene(i);
			}
		}
		ImGui::EndCombo();
	}
	


    if (prefabEntities_.empty()) {
        ImGui::End();
        return;
    }

    for (const ecs::EntityIndex entity : prefabEntities_) {
        //Display entity only if it's not a child
        if (entity == prefabEntities_[0]) {
            DisplayEntity(entity);

            if (ImGui::Button("Save Prefab")) {
                ImGui::OpenPopup("Save Prefab");
				newPrefabName_ = editorComponentManager_.GetComponent(entity).name;
            }

            if (ImGui::BeginPopupModal(
                "Save Prefab",
                nullptr,
                ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::InputTextWithHint(
                    "Prefab Name",
                    &selectedPrefabName_[0],
                    &newPrefabName_[0],
                    64);

                if (PokFileSystem::CheckFileExists(
                    PokFileSystem::
                    GetFullPath(
                        newPrefabName_,
                        FileType::PREFAB,
                        FolderType::SAVE_IN_ROM))) {
                    if (selectedPrefabName_ == newPrefabName_) {
                        ImGui::
                            Text(
                                "File %s.pokprefab already exists, the prefab will be override.",
                                newPrefabName_.c_str());
                    }
                }
                if (ImGui::Button("Cancel"))
                    ImGui::CloseCurrentPopup();
                ImGui::SameLine();

                if (ImGui::Button("Save")) {
                    SavePrefab(newPrefabName_);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
	for (int i = 0; i < entityToDestroy.size(); i++) {

		auto it = std::find(prefabEntities_.begin(), prefabEntities_.end(), entityToDestroy[i]);
		editor_.GetEditorEcsManager().DestroyEntity(entityToDestroy[i]);
		prefabEntities_.erase(it);
	}
	entityToDestroy.clear();
    ImGui::End();
}

void PrefabCreatorTool::CreatePrefabInScene(const int index)
{
    ecsManager_.DestroyEntities(prefabEntities_);
    editorPrefab_.Clear();

    editorPrefab_.SetFromJson(currentPrefabs_[index]);
    prefabEntities_ = editorPrefab_.Instantiate();
    prefabEntities_.reserve(1000);
    editor_.SetSelectedEntityIndex(prefabEntities_[0]);
}

void PrefabCreatorTool::DisplayComponentList(const ecs::EntityIndex entityIndex)
{
    // Display Component list window
    ImGui::Begin(kComponentListName_, &isActive_);

    auto entityTag = ecsManager_.GetTag(entityIndex);
    const std::string tag = TagManagerLocator::Get().GetTagName(entityTag);
    if (ImGui::CollapsingHeader(("Tag : " + tag).c_str())) {
        const auto& tags = TagManagerLocator::Get().GetAllTagNames();
        for (auto i = 0; i < tags.size(); i++) {
            const std::string tag = tags[i] + " : " + std::to_string(i);
            if (ImGui::Selectable(&tag[0], entityTag == i)) { entityTag = i; }
        }
        ecsManager_.SetTag(entityIndex, entityTag);
    }

    for (unsigned int component = 1u << 0;
         component != lastComponentType_;
         component = component << 1) {
        DisplayComponent(static_cast<ecs::ComponentType::ComponentType>(component), entityIndex);
    }
    if (ImGui::Button("Create New Component")) { ImGui::OpenPopup("component_popup"); }
    if (ImGui::BeginPopup("component_popup")) {
        //Display new component list
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

void PrefabCreatorTool::DisplayComponent(
    const ecs::ComponentType::ComponentType componentType,
    const ecs::EntityIndex entityIndex) const
{
    if (ecsManager_.HasComponent(entityIndex, componentType)) {
        //switch begin
        switch (componentType) {
        case ecs::ComponentType::TRANSFORM: {
            const math::Transform transform = transformsManager_.GetComponent(entityIndex);
            const math::Transform newTransform = ComponentUtility::DisplayAndModifyTransform(
                transform);

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

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newRigidbody != rigidbody) {
                rigidbodyManager_.SetComponent(
                    entityIndex,
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
                editor_.GetEditorEcsManager().UpdateComponent(
                    entityIndex,
                    ecs::ComponentType::MODEL);
            }
            break;
        }
        case ecs::ComponentType::SPLINE_FOLLOWER: {
            const ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(
                entityIndex);
            const ecs::SplineFollower newSplineFollower = ComponentUtility::
                DisplayAndModifySplineFollower(splineFollower);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newSplineFollower != splineFollower) {
                splineFollowersManager_.SetComponent(entityIndex, newSplineFollower);
            }
            break;
        }
        case ecs::ComponentType::LIGHT: {
            const ecs::Light light = lightManager_.GetComponent(entityIndex);
            const ecs::Light newLight = ComponentUtility::DisplayAndModifyLight(light);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newLight != light) { lightManager_.SetComponent(entityIndex, newLight); }
            break;
        }
        case ecs::ComponentType::PARTICLE_SYSTEM: {
            const graphics::ParticleSystem particle = particlesManager_.GetComponent(entityIndex);
            const graphics::ParticleSystem newParticle = ComponentUtility::
                DisplayAndModifyParticleSystem(particle);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newParticle != particle) {
                particlesManager_.SetComponent(entityIndex, newParticle);
            }
            break;
        }
        case ecs::ComponentType::AUDIO_SOURCE: {
            const audio::AudioSource audioSource = audioSourceManager_.GetComponent(entityIndex);
            const audio::AudioSource newAudioSource = ComponentUtility::DisplayAndModifyAudioSource(
                audioSource);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newAudioSource != audioSource) {
                audioSourceManager_.SetComponent(entityIndex, newAudioSource);
            }
            break;
        }
        case ecs::ComponentType::TRAIL_RENDERER: {
            const ecs::TrailRenderer& trailRenderer = trailRendererManager_.GetComponent(
                entityIndex);
            const ecs::TrailRenderer newTrailRenderer = ComponentUtility::
                DisplayAndModifyTrailRenderer(trailRenderer);

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
		case ecs::ComponentType::EDITOR_COMPONENT: {
			const ecs::EditorComponent editorComponent = editorComponentManager_.GetComponent(entityIndex);
			const ecs::EditorComponent newEditorComponent = ComponentUtility::DisplayAndModifyEditorComponent(editorComponent);
            if (newEditorComponent != editorComponent) {
                editorComponentManager_.SetComponent(entityIndex, newEditorComponent);
            }
            break;
        }
        case ecs::ComponentType::ENEMY: {
            const game::Enemy enemy = enemiesManager_.GetComponent(entityIndex);
            const game::Enemy newEnemy = ComponentUtility::DisplayAndModifyEnemy(enemy);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newEnemy != enemy) { enemiesManager_.SetComponent(entityIndex, newEnemy); }
            break;
        }
        case ecs::ComponentType::PLAYER: {
            const game::Player player = playerManager_.GetComponent(entityIndex);
            const game::Player newPlayer = ComponentUtility::DisplayAndModifyPlayer(player);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newPlayer != player) { playerManager_.SetComponent(entityIndex, newPlayer); }
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

            if (newWeapon != weapon) { weaponManager_.SetComponent(entityIndex, newWeapon); }
            break;
        }
        case ecs::ComponentType::PROJECTILE: {
            const game::Projectile projectile = projectileManager_.GetComponent(entityIndex);
            const game::Projectile newProjectile = ComponentUtility::DisplayAndModifyProjectile(
                projectile);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newProjectile != projectile) {
                projectileManager_.SetComponent(entityIndex, newProjectile);
            }
            break;
        }
        case ecs::ComponentType::MISSILE: {
            const game::Missile missile = missileManager_.GetComponent(entityIndex);
            const game::Missile newMissile = ComponentUtility::DisplayAndModifyMissile(missile);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newMissile != missile) { missileManager_.SetComponent(entityIndex, newMissile); }
            break;
        }
        case ecs::ComponentType::SPECIAL_ATTACK: {
            const game::SpecialAttack specialAttack = specialAttackManager_.GetComponent(
                entityIndex);
            const game::SpecialAttack newSpecialAttack = ComponentUtility::
                DisplayAndModifySpecialAttack(specialAttack);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newSpecialAttack != specialAttack) {
                specialAttackManager_.SetComponent(entityIndex, newSpecialAttack);
            }
            break;
        }
        case ecs::ComponentType::SPLINE_STATES: {
            const game::SplineStates splineStates = splineStatesManager_.GetComponent(entityIndex);
            const game::SplineStates newSplineStates = ComponentUtility::
                DisplayandModifySplineStates(splineStates);

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

            if (newJiggle != jiggle) { jiggleManager_.SetComponent(entityIndex, newJiggle); }
            break;
        }
        case ecs::ComponentType::GAME_CAMERA: {
            const game::GameCamera gameCamera = gameCameraManager_.GetComponent(entityIndex);
            const game::GameCamera newGameCamera = ComponentUtility::DisplayAndModifyGameCamera(
                gameCamera);

            ComponentUtility::DisplayRemoveComponentButton(componentType);

            if (newGameCamera != gameCamera) {
                gameCameraManager_.SetComponent(entityIndex, newGameCamera);
            }
            break;
        }

        default:
            break;
            //switch end
        }
    }
}

void PrefabCreatorTool::CreateComponent(
    const std::string& componentName,
    const ecs::ComponentType::ComponentType
    componentType) const
{
    if (ImGui::MenuItem(componentName.c_str())) {
        if (!ecsManager_.HasComponent(editor_.GetSelectedEntityIndex(), componentType)) {
            ecsManager_.AddComponent(
                editor_.GetSelectedEntityIndex(),
                componentType);
        }
    }
}

void PrefabCreatorTool::DisplayEntity(const ecs::EntityIndex entityIndex)
{
	std::cout << prefabEntities_.size() << "\n";
	std::vector<ecs::EntityIndex> childrenIndexes = transformsManager_.GetChildren(entityIndex);

	const bool isParent = !childrenIndexes.empty();

	ImGuiTreeNodeFlags nodeFlags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	const std::string entityName = editorComponentManager_.GetComponent(entityIndex).name;

	if (editor_.GetSelectedEntityIndex() == entityIndex) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	if (!isParent) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf |
			ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	const bool nodeOpen = ImGui::TreeNodeEx(entityName.c_str(), nodeFlags);

	//IF CLICK ON ENTITY
	if (ImGui::IsItemClicked()) {
		editor_.SetSelectedEntityIndex(entityIndex);
	}
	//add child and set parent
	if (editor_.GetSelectedEntityIndex() == entityIndex) {
		if (ImGui::BeginPopupContextItem(entityName.c_str())) {
			if (ImGui::Button("Add Child")) {
				const ecs::EntityIndex childIndex = ecsManager_.AddEntity();
				ecsManager_.AddComponent(childIndex,ecs::ComponentType::TRANSFORM);
				prefabEntities_.push_back(childIndex);
				transformsManager_.SetParent(childIndex, entityIndex);
			}
			if (ImGui::Button("Delete entity")) {
				if (!childrenIndexes.empty()) {
					for (auto child : childrenIndexes) {
						transformsManager_.SetParent(child, prefabEntities_[0]);
					}
				}
				entityToDestroy.push_back(entityIndex);			
			}
			ImGui::EndPopup();
		}
	}
	if (nodeOpen && isParent) {
		for (const ecs::EntityIndex childIndex : childrenIndexes) {
			DisplayEntity(childIndex);
		}
		ImGui::TreePop();
	}
}

void PrefabCreatorTool::SavePrefab(
	const std::string& prefabName)
{
	std::vector<size_t> parentIndexes;
	parentIndexes.reserve(prefabEntities_.size());

	std::vector<game::GameArchetype> gameArchetypes;
	gameArchetypes.reserve(prefabEntities_.size());

	std::vector<std::string> newPrefabNames;
	newPrefabNames.reserve(prefabEntities_.size());

	std::vector<ecs::ArchetypeID> newArchetypesID;
	newArchetypesID.reserve(prefabEntities_.size());

	for (size_t i = 0; i < prefabEntities_.size(); i++) {
		game::GameArchetype newGameArchetype;
		gameArchetypes.emplace_back(newGameArchetype);

		newArchetypesID.emplace_back(ecs::defaultArchetypeID);

		if (prefabEntities_[i] == prefabEntities_[0]) { newPrefabNames.emplace_back(prefabName); }
		else {
			newPrefabNames.emplace_back(editorComponentManager_.GetComponent(prefabEntities_[i]).name);
		}

		//to repeat for set archetypes and save
		for (unsigned int component = 1u;
			component < lastComponentType_;
			component = component << 1) {
			const ecs::ComponentType::ComponentType componentType =
				static_cast<ecs::ComponentType::ComponentType>(component);

			if (ecsManager_.HasComponent(prefabEntities_[i], componentType)) {
				switch (componentType) {
				case ecs::ComponentType::TRANSFORM: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::TRANSFORM);
					gameArchetypes[i].transform = transformsManager_.
						GetComponent(prefabEntities_[i]);

					size_t parentIndex;

					if (transformsManager_.GetParent(prefabEntities_[i]) == ecs::kNoParent) {
						parentIndex = ecs::kNoParent;
					}
					else {
						for (size_t j = 0; j < prefabEntities_.size(); j++) {
							if (transformsManager_.GetParent(prefabEntities_[i]) == prefabEntities_[
								j]) {
								parentIndex = j;
							}
						}
					}
					parentIndexes.emplace_back(parentIndex);
				}
													break;
				case ecs::ComponentType::RIGIDBODY: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::RIGIDBODY);
					gameArchetypes[i].rigidbody = rigidbodyManager_.
						GetComponent(prefabEntities_[i]);
				}
													break;
				case ecs::ComponentType::COLLIDER: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::COLLIDER);
					gameArchetypes[i].collider = collidersManager_.
						GetComponent(prefabEntities_[i]);
				}
												   break;
				case ecs::ComponentType::MODEL: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::MODEL);
					gameArchetypes[i].model = modelsManager_.
						GetComponent(prefabEntities_[i]);
				}
												break;
				case ecs::ComponentType::SPLINE_FOLLOWER: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::SPLINE_FOLLOWER);
					gameArchetypes[i].spline = splineFollowersManager_.
						GetComponent(prefabEntities_[i]);
				}
														  break;
				case ecs::ComponentType::LIGHT: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::LIGHT);
					gameArchetypes[i].light = lightManager_.GetComponent(prefabEntities_[i]);
				}
												break;
				case ecs::ComponentType::PARTICLE_SYSTEM: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::PARTICLE_SYSTEM);
					gameArchetypes[i].particleSystem = particlesManager_.
						GetComponent(prefabEntities_[i]);
				}
														  break;
				case ecs::ComponentType::AUDIO_SOURCE: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::AUDIO_SOURCE);
					gameArchetypes[i].audioSource = audioSourceManager_.
						GetComponent(prefabEntities_[i]);
				}
													   break;
				case ecs::ComponentType::TRAIL_RENDERER: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::TRAIL_RENDERER);
					gameArchetypes[i].trailRenderer = trailRendererManager_.
						GetComponent(prefabEntities_[i]);
				}
														 break;
				case ecs::ComponentType::SEGMENT_RENDERER: {

					gameArchetypes[i].AddComponent(ecs::ComponentType::SEGMENT_RENDERER);
					gameArchetypes[i].segmentRenderer = segmentRendererManager_.
						GetComponent(prefabEntities_[i]);
				}
				break;

				case ecs::ComponentType::ENEMY: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::ENEMY);
					gameArchetypes[i].enemy = enemiesManager_.
						GetComponent(prefabEntities_[i]);
				}
												break;
				case ecs::ComponentType::PLAYER: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::PLAYER);
					gameArchetypes[i].player = playerManager_.
						GetComponent(prefabEntities_[i]);
				}
												 break;
				case ecs::ComponentType::DESTRUCTIBLE_ELEMENT: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
					gameArchetypes[i].destructibleElement = destructibleElementManager_.
						GetComponent(prefabEntities_[i]);
				}
															   break;
				case ecs::ComponentType::WEAPON: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::WEAPON);
					gameArchetypes[i].weapon = weaponManager_.
						GetComponent(prefabEntities_[i]);
				}
												 break;
				case ecs::ComponentType::PROJECTILE: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::PROJECTILE);
					gameArchetypes[i].projectile = projectileManager_.
						GetComponent(prefabEntities_[i]);
				}
													 break;
				case ecs::ComponentType::MISSILE: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::MISSILE);
					gameArchetypes[i].missile = missileManager_.
						GetComponent(prefabEntities_[i]);
				}
												  break;
				case ecs::ComponentType::SPECIAL_ATTACK: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::SPECIAL_ATTACK);
					gameArchetypes[i].specialAttack = specialAttackManager_.
						GetComponent(prefabEntities_[i]);
				}
														 break;
				case ecs::ComponentType::JIGGLE: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::JIGGLE);
					gameArchetypes[i].jiggle = jiggleManager_.
						GetComponent(prefabEntities_[i]);
				}
												 break;
				case ecs::ComponentType::GAME_CAMERA: {
					gameArchetypes[i].AddComponent(ecs::ComponentType::GAME_CAMERA);
					gameArchetypes[i].gameCamera = gameCameraManager_.
						GetComponent(prefabEntities_[i]);
				}
													  break;
													  break;
				default:
					break;
				}
			}
		}
	}

	//for update of archetypes
	editorPrefab_.SetArchetypesId(newArchetypesID);
	editorPrefab_.SetNames(newPrefabNames);
	editorPrefab_.SetParentIndexes(parentIndexes);

	editorPrefab_.SetArchetypes(gameArchetypes);
	const json newPrefabJson = editorPrefab_.ToJson();

	PokFileSystem::WriteFile(
		prefabName,
		newPrefabJson,
		FileType::PREFAB,
		FolderType::SAVE_IN_ROM);
	
	GetAllPrefab();
	prefabCurrent = prefabName.c_str();
}

void PrefabCreatorTool::GetAllPrefab()
{
	currentPrefabs_.clear();
	prefabNames_.clear();
    const std::string path = PokFileSystem::GetPath(FileType::PREFAB, FolderType::SAVE_IN_ROM);
    prefabNames_ = PokFileSystem::GetAllFilesInDirectories(path, true, false);

	for (int i = 0; i < prefabNames_.size(); i++) {
        const size_t lastIndex = prefabNames_[i].find_last_of('.');
        const std::string newName = prefabNames_[i].substr(0, lastIndex);
		prefabNames_[i] = newName;
        json prefabJson = PokFileSystem::ReadJsonFile(
            newName,
            FileType::PREFAB,
            FolderType::SAVE_IN_ROM);
        currentPrefabs_.push_back(prefabJson);
    }
}
} //namespace editor
} //namespace poke
