#include <Editor/Tools/convertible_scene_to_pokscene_tool.h>

#include <vector>
#include <algorithm>
#include <cassert>

#include <Math/hash.h>
#include <Utility/log.h>
#include <CoreEngine/engine.h>
#include <Utility/json_utility.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Editor/Tools/resource_accessor_tool.h>
#include <GraphicsEngine/Lights/light_type.h>
#include <Editor/component_utility.h>

// TODO(@All) make google test to ensure proper conversion.

namespace poke {
namespace editor {
ConvertibleSceneToPokSceneTool::ConvertibleSceneToPokSceneTool(
	Editor& editor,
    const bool defaultActive)
    : Tool(defaultActive)
{
	isActive_ = defaultActive;
	name_ = "Convertible Scene to PokScene Tool";
	toolFlag_ = TOOL_FLAG_CONVERTIBLE_SCENE;
}

void ConvertibleSceneToPokSceneTool::OnSetActive() {}

void ConvertibleSceneToPokSceneTool::OnSetInactive() { }
// Not used but mandatory.

void ConvertibleSceneToPokSceneTool::OnDrawImgui()
{
    ImGui::Begin("Create PokScene from ConvertibleScene", &isActive_);

	ImGui::Columns(2);
    ImGui::InputText(
		"Input filename...", 
		inputFileName_, 
		kFileNameLength);
	ImGui::NextColumn();

	const Resource outResource = ResourceAccessorTool::DisplayGetResource(
		"Select scene", 
		ResourceAccessorTool::DisplayType::BUTTON, 
		FileType::POKCONVERTIBLESCENE, 
		FolderType::SAVE); 
	if (ResourceAccessorTool::IsResourceValid(outResource)) {
		outResource.name.copy(inputFileName_, outResource.name.size(), 0);
	}

	ImGui::NextColumn();
    ImGui::InputText(
		"Output filename...", 
		outputFileName_, 
		kFileNameLength);
	ImGui::NextColumn();
    if(ImGui::Button("Copy above name")) {
        for(unsigned i = 0; i < kFileNameLength; i++) {
			outputFileName_[i] = inputFileName_[i];
        }
    }

	ImGui::Columns(1);
    if (ImGui::Button("Convert")) {
        if (std::strcmp(inputFileName_, "") != 0 && 
			std::strcmp(outputFileName_, "") != 0
			) {
            if (PokFileSystem::CheckFileExists(
                PokFileSystem::GetFullPath(
                    inputFileName_,
                    FileType::POKCONVERTIBLESCENE,
                    FolderType::SAVE))) {
                if (Convert(inputFileName_, outputFileName_)) {
                    LogDebug("PokScene successfully created!", LogType::EDITOR_LOG);
                } else {
                    LogWarning("Failed to create scene file!", LogType::EDITOR_LOG);
                }
            } else {
                LogWarning(
                    "Input file with name " + std::string(inputFileName_) +
                    " does not exist!", LogType::EDITOR_LOG);
            }
        } else {
            LogDebug(
				"Please enter valid names for input and output files.", 
				LogType::EDITOR_LOG
			);
        }
    }
    ImGui::End();
}

bool ConvertibleSceneToPokSceneTool::Convert(
	const char* inputFileName,
	const char* outputFileName) const
{
	// Try to load the input file.
	json input = json();
	input = LoadJson(
		inputFileName,
		FileType::POKCONVERTIBLESCENE,
		FolderType::SAVE);
	if (input.is_null()) {
		LogError(
			"ConvertibleSceneToPokSceneTool: Cound not find file: " +
			std::string(inputFileName),
			LogType::EDITOR_LOG
		);
		return false;
	}

	ConvertibleScene scene = ConvertibleScene(outputFileName);
	ConversionLists lists;

#pragma region BuildGameObjects
	{
		cassert(
			!input.is_null(),
			"ConvertibleSceneToPokSceneTool::BuildGameObjectsFor(): input is null!");

		// Early exit if there are no game objects to convert.
		cassert(
			CheckJsonExists(input, "gameObjects"),
			"ConvertibleSceneToPokSceneTool: No game objects found in .pokconvertiblescene!");

		// Init variables.
		const json gameObjects = input["gameObjects"];
		json tags = TagManagerLocator::Get().GetAllTagNames();
		tags[0] = "Default";

		const size_t gameObjectsCount = gameObjects.size();
		auto parentIds = std::vector<int>();
		parentIds.reserve(gameObjectsCount);

		// First pass on game objects.
		for (ecs::EntityIndex i = 0; i < gameObjectsCount; i++) {
			// Set current game object we're working with.
			const json singleGameObject = gameObjects[i];
			json outputSingleGameObject = json();
			cassert(
				!singleGameObject.is_null(),
				"ConvertibleSceneToPokSceneTool::BuildGameObjectsFor(): singleGameObject retrieved from gameObjects[i] is null!");

			// Retrieve instanceIds for next pass.
			cassert(
				CheckJsonExists(singleGameObject, "instanceId"),
				"ConvertibleSceneToPokSceneTool::BuildGameObjectsFor(): singleGameObject does not have an instanceId!");
			const unsigned instanceId = singleGameObject["instanceId"];

			parentIds.emplace_back(
				CheckJsonExists(singleGameObject, "parentInstanceId") ?
				int(singleGameObject["parentInstanceId"]) :
				0);

			lists.unityIdToPokSceneId.insert({ instanceId, i });
			// Indices in pokscene arrays begin with 1. 0 is reserved for defaut value.

			//-------------------- Set fields. ------------------------//
			// Retrieve IsActive
			bool isActive = false;
			if (CheckJsonExists(singleGameObject, "isActive")) {
				isActive = singleGameObject["isActive"];
			}

			outputSingleGameObject["isActive"] = isActive;

			// Retrieve Tag
			if (CheckJsonExists(singleGameObject, "tag")) {
				std::string objectTag = singleGameObject["tag"];

				// Add to the tag manager if found
				bool found = false;
				for (size_t tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
					if (tags[tagIndex] == objectTag) {
						outputSingleGameObject["tag"] = tagIndex;
						found = true;
						break;
					}
				}
				if (!found) {
					outputSingleGameObject["tag"] = tags.size();
					tags[tags.size()] = objectTag;
				}
			}
			else {
				outputSingleGameObject["tag"] = 0; // Default
			}

			// Retrieve PrefabName if existing 
			if (CheckJsonExists(singleGameObject, "prefabName")) {
                //TODO (@Duncan) Reference prefabName in the resourceList of prefabNames
				outputSingleGameObject["prefabName"] = singleGameObject["prefabName"];
			}

			// Retrieve Name
			if (CheckJsonExists(singleGameObject, "gameObjectName")) {
				outputSingleGameObject["name"] = singleGameObject["gameObjectName"].get<std::string>();
			}

			// Retrieve Transform if existing
			outputSingleGameObject = SetComponentFor(
				ecs::ComponentType::TRANSFORM,
				outputSingleGameObject,
				RetrieveTransformFrom(singleGameObject)
			);
			// Retrieve Model if existing
			json modelObject;
			if (CheckJsonExists(singleGameObject, "model")) {
				const json inputModel = singleGameObject["model"];

				//Load mesh
				if (CheckJsonExists(inputModel, "meshName")) {
					const std::string name = inputModel["meshName"];

					modelObject["meshHash"] = math::HashString(name);
					if (lists.meshNames.empty() ||
						std::find(
							lists.meshNames.begin(),
							lists.meshNames.end(),
							name) == lists.meshNames.end()
						) {
						lists.meshNames.push_back(name);
					}
				}
				else {
					modelObject["meshHash"] = MeshManagerLocator::Get().GetSphereID();
				}

				if (CheckJsonExists(inputModel, "materialName")) {
					const std::string name = inputModel["materialName"];

					modelObject["materialHash"] = math::HashString(name);
					if (lists.materialNames.empty() ||
						std::find(
							lists.materialNames.begin(),
							lists.materialNames.end(),
							name) == lists.materialNames.end()
						) {
						lists.materialNames.push_back(name);
					}
				}
				else {
					modelObject["materialHash"] = MaterialsManagerLocator::Get().
						GetDefaultMaterialID();
				}
			}
			outputSingleGameObject = SetComponentFor(
				ecs::ComponentType::MODEL,
				outputSingleGameObject,
				modelObject
			);

			// Retrieve Collider if existing
			outputSingleGameObject = SetComponentFor(
				ecs::ComponentType::COLLIDER,
				outputSingleGameObject,
				RetrieveColliderFrom(singleGameObject));
			// Retrieve Spline if existing
			outputSingleGameObject = SetComponentFor(
				ecs::ComponentType::SPLINE_FOLLOWER,
				outputSingleGameObject,
				RetrieveSplineFrom(singleGameObject)
			);
			// Retrieve Light if existing
			outputSingleGameObject = SetComponentFor(
				ecs::ComponentType::LIGHT,
				outputSingleGameObject,
				RetrieveLightFrom(singleGameObject)
			);
			// Retrieve Light if existing
			json audioSourceObject = json();

			if (CheckJsonExists(singleGameObject, "audioSource")) {

				json inputAudioSource = singleGameObject["audioSource"];

				audioSourceObject = inputAudioSource;

				const std::string name = inputAudioSource["audioClipName"];
				audioSourceObject["audioClipHash"] = math::HashString(name);

				if (lists.audioNames.empty()) {
					lists.audioNames.push_back(name);
				}
				else {
					if (std::find(
						lists.audioNames.begin(),
						lists.audioNames.end(),
						name) == lists.audioNames.end())
					{
						lists.audioNames.push_back(name);
					}
				}
			}
			outputSingleGameObject = SetComponentFor(
				ecs::ComponentType::AUDIO_SOURCE,
				outputSingleGameObject,
				audioSourceObject
			);

			// Add game object to output.
			scene.gameObjects.push_back(outputSingleGameObject);
		}

		//Register all the tags of the scene in a separate file
		TagManagerLocator::Get().SetFromJson(tags);

		// Second pass on game objects for parent - child relations.
		for (size_t i = 0; i < gameObjectsCount; i++) {
			json& singleGameObject = scene.gameObjects[i];

			if (parentIds[i] != 0) {
				singleGameObject["transform"]["parent"] = lists.unityIdToPokSceneId[
					parentIds[i]];
			}
			else {
				singleGameObject["transform"]["parent"] = ecs::kNoParent;
				//We were asked to divide the scale by 100 from Unity to Pok !
				const float divider = 1.0f;
				const float x = singleGameObject["transform"]["scale"]["x"];
				const float y = singleGameObject["transform"]["scale"]["y"];
				const float z = singleGameObject["transform"]["scale"]["z"];
				singleGameObject["transform"]["scale"]["x"] = x / divider;
				singleGameObject["transform"]["scale"]["y"] = y / divider;
				singleGameObject["transform"]["scale"]["z"] = z / divider;
			}
		}
	}
#pragma endregion Build Gameobjects

#pragma region Build Skybox
	{
		json skyboxEntity;
		if (CheckJsonExists(input, "skybox") &&
			CheckJsonExists(input["skybox"], "materialName"))
		{
			ResourceID resourceId = math::HashString(input["skybox"]["materialName"].get<std::string>());
			lists.materialNames.push_back(input["skybox"]["materialName"].get<std::string>());
			skyboxEntity["model"]["materialHash"] = resourceId;
		}
		else {
			skyboxEntity["model"]["materialHash"] =
				MaterialsManagerLocator::Get().GetDefaultSkyboxMaterialID();
		}

		skyboxEntity["model"]["meshHash"] = MeshManagerLocator::Get().GetSphereID();

		skyboxEntity["isActive"] = true;
		skyboxEntity["name"] = "skybox";
		scene.gameObjects.push_back(skyboxEntity);
	}
#pragma endregion Build Skybox

#pragma region Build Resources
	{
		cassert(
			!input.is_null(),
			"ConvertibleSceneToPokSceneTool::BuildResourcesFor(): arguments passed are invalid!");

		// Init variables.
		auto materialResources = json::array_t();
		auto meshResources = json::array_t();
		auto audioResources = json::array_t();
		//const size_t textureCount = conversionLists.texturesNames.size();
		const size_t materialCount = lists.materialNames.size();
		const size_t meshCount = lists.meshNames.size();
		const size_t audioCount = lists.audioNames.size();

		// Fill out materials.
		for (size_t i = 0; i < materialCount; i++) {
			materialResources.push_back(lists.materialNames[i]);
		}
		// Fill out meshes.
		for (size_t i = 0; i < meshCount; i++) {
			meshResources.push_back(lists.meshNames[i]);
		}
		// Fill out audios.
		for (size_t i = 0; i < audioCount; i++) {
			audioResources.push_back(lists.audioNames[i]);
		}

		// Set resources field.
		if (materialCount > 0) {
			scene.resources["materialNames"] = materialResources;
		}
		if (meshCount > 0) {
			scene.resources["mesheNames"] = meshResources;
		}
		if (audioCount > 0) {
			scene.resources["audioNames"] = audioResources;
		}
	}
#pragma endregion Build Resources

#pragma region Build Chunks
	{
		cassert(
			!input.is_null(),
			"ConvertibleSceneToPokSceneTool::BuildChunksFor(): input provided is null!");

		if (CheckJsonExists(input, "chunks")) {
			const json chunks = input["chunks"];
			cassert(
				chunks.is_array(),
				"ConvertibleSceneToPokSceneTool::BuildChunksFor(): chunks retrieved from input[chunks] is not an array!");
			const size_t chunkCount = chunks.size();
			cassert(
				chunkCount > 0,
				"ConvertibleSceneToPokSceneTool::BuildChunksFor(): chunks' array length is smaller than 1!");

			for (size_t i = 0; i < chunkCount; i++) {
				const json chunk = chunks[i];
				json outputChunk = json();

				if (CheckJsonExists(chunk, "chunkPosition")) {
					const json position = chunk["chunkPosition"];

					if (CheckJsonExists(position, "x") ||
						CheckJsonExists(position, "y") ||
						CheckJsonExists(position, "z")) {
						outputChunk["position"] = position;
					}
				}
				if (CheckJsonExists(chunk, "chunkSize")) {
					const json size = chunk["chunkSize"];

					if (CheckJsonExists(size, "x") ||
						CheckJsonExists(size, "y") ||
						CheckJsonExists(size, "z")) {
						outputChunk["size"] = size;
					}
				}

				if (CheckJsonExists(chunk, "gameObjectsID")) {
					const json gameObjects = chunk["gameObjectsID"];
					cassert(
						gameObjects.is_array(),
						"ConvertibleSceneToPokSceneTool::BuildChunksFor(): gameObjects retrieved from chunk[gameObjectsID] is not an array!");
					const size_t gameObjectsCount = gameObjects.size();
					cassert(
						gameObjectsCount > 0,
						"ConvertibleSceneToPokSceneTool::BuildChunksFor(): gameObjects array's size is smaller than 1!");

					json outputGameObjects = json();

					for (size_t j = 0; j < gameObjectsCount; j++) {
						unsigned retrievedId = lists.unityIdToPokSceneId[gameObjects[j]];
						if (retrievedId > 0) {
							outputGameObjects.push_back(retrievedId);
						}
					}

					outputChunk["gameObjects"] = outputGameObjects;
				}

				//TriggerEvents
				if (CheckJsonExists(input, "chunkTriggerEvents")) {
					const json chunkTriggerEvents = input["chunkTriggerEvents"];

					json currentTriggerEvent;
					for (size_t j = 0; j < chunkTriggerEvents.size(); j++) {
						if (i == chunkTriggerEvents[j]["chunkLinkedToEvent"]) {
							currentTriggerEvent = chunkTriggerEvents[i];
							break;
						}
					}
					if (CheckJsonExists(currentTriggerEvent, "chunksToShow")) {
						outputChunk["chunksToShow"] = currentTriggerEvent["chunksToShow"];
					}
					else {
						outputChunk["chunksToShow"] = nlohmann::detail::value_t::array;
					}

					outputChunk["entitiesIdToActivate"] = nlohmann::detail::value_t::array;
					if (CheckJsonExists(currentTriggerEvent, "entitiesIdToActivate")) {
						const size_t entitiesIdToActivateCount = currentTriggerEvent["entitiesIdToActivate"].size();
						for (size_t j = 0; j < entitiesIdToActivateCount; j++) {
							unsigned entity = currentTriggerEvent["entitiesIdToActivate"][j];
							unsigned retrievedId = lists.unityIdToPokSceneId[entity];
							if (retrievedId > 0) {
								outputChunk["entitiesIdToActivate"][j] = retrievedId;
							}
						}
					}

					if (CheckJsonExists(currentTriggerEvent, "chunksToHide")) {
						outputChunk["chunksToHide"] = currentTriggerEvent["chunksToHide"];
					}
					else {
						outputChunk["chunksToHide"] = nlohmann::detail::value_t::array;
					}

					if (CheckJsonExists(currentTriggerEvent, "chunksToDestroy")) {
						outputChunk["chunksToDestroy"] = currentTriggerEvent["chunksToDestroy"];
					}
					else {
						outputChunk["chunksToDestroy"] = nlohmann::detail::value_t::array;
					}
				}
				scene.chunks.push_back(outputChunk);
			}
		}
	}
#pragma endregion Build Chunks

#pragma region Build ChunkTimeEvents
	{
		cassert(
			!input.is_null(),
			"ConvertibleSceneToPokSceneTool::BuildChunkTimeEventsFor(): input is null!");

		if (CheckJsonExists(input, "chunkTimeEvents")) {
			const json chunkTimeEvents = input["chunkTimeEvents"];
			cassert(
				chunkTimeEvents.is_array(),
				"ConvertibleSceneToPokSceneTool::BuildChunkTimeEventsFor(): chunkTimeEvents retreived from input[chunkTimeEvents] is not an array!");

			cassert(
				chunkTimeEvents.empty(),
				"ConvertibleSceneToPokSceneTool::BuildChunkTimeEventsFor(): chunkTimeEvents retreived from input[chunkTimeEvents]'s size is less than 1!");
			const size_t chunkTimeEventsCount = chunkTimeEvents.size();

			json outputChunkTimeEvents = json();
			for (size_t i = 0; i < chunkTimeEventsCount; i++) {
				const json chunkTimeEvent = chunkTimeEvents[i];
				json outputChunkTimeEvent = json();

				if (CheckJsonExists(chunkTimeEvent, "triggerTime")) {
					float triggerTime = chunkTimeEvent["triggerTime"];
					if (triggerTime > 0.0f) {
						outputChunkTimeEvent["time"] = triggerTime;
					}
				}

				if (CheckJsonExists(chunkTimeEvent, "chunksToShow")) {
					const json inputChunksToShow = chunkTimeEvent["chunksToShow"];
					const size_t chunksToShowCount = inputChunksToShow.size();
					assert(chunksToShowCount > 0);

					for (size_t j = 0; j < chunksToShowCount; j++) {
						outputChunkTimeEvent["chunksToShow"].push_back(j + 1);
					}
				}
				if (CheckJsonExists(chunkTimeEvent, "chunksToHide")) {
					const json inputChunksToHide = chunkTimeEvent["chunksToHide"];
					const size_t chunksToHideCount = inputChunksToHide.size();
					assert(chunksToHideCount > 0);

					for (size_t j = 0; j < chunksToHideCount; j++) {
						outputChunkTimeEvent["chunksToHide"].push_back(j + 1);
					}
				}
				if (CheckJsonExists(chunkTimeEvent, "chunksToActivate")) {
					const json inputChunksToActivate = chunkTimeEvent[
						"chunksToActivate"];
					const size_t chunksToActivateCount = inputChunksToActivate.
						size();
					assert(chunksToActivateCount > 0);

					for (size_t j = 0; j < chunksToActivateCount; j++) {
						outputChunkTimeEvent["chunksToActivate"].push_back(j + 1);
					}
				}
				if (CheckJsonExists(chunkTimeEvent, "chunksToDestroy")) {
					const json inputChunksToDestroy = chunkTimeEvent[
						"chunksToDestroy"];
					const size_t chunksToDestroyCount = inputChunksToDestroy.
						size();
					assert(chunksToDestroyCount > 0);

					for (size_t j = 0; j < chunksToDestroyCount; j++) {
						outputChunkTimeEvent["chunksToDestroy"].push_back(j + 1);
					}
				}

				outputChunkTimeEvents.push_back(outputChunkTimeEvent);
			}

			scene.chunkTimeEvents = outputChunkTimeEvents;
		}
	}
#pragma endregion Build ChunkTimeEvents

    return WriteOutput(scene);
}

bool ConvertibleSceneToPokSceneTool::WriteOutput(
    const ConvertibleScene& convertibleScene) const
{
    cassert(
        convertibleScene.fileName != "",
        "ConvertibleSceneToPokSceneTool::WriteOutput(): fileName provided is empty!");

    json output = json();
    if (!convertibleScene.resources.is_null()) {
        output["resources"] = convertibleScene.resources;
    }
    if (!convertibleScene.gameObjects.is_null()) {
        output["gameObjects"] = convertibleScene.gameObjects;
    }
    if (!convertibleScene.chunks.is_null()) {
        output["chunks"] = convertibleScene.chunks;
    }
    if (!convertibleScene.chunkTimeEvents.is_null()) {
        output["chunkTimeEvents"] = convertibleScene.chunkTimeEvents;
    }
    if (!convertibleScene.chunkTriggerEvents.is_null()) {
        output["chunkTriggerEvents"] = convertibleScene.chunkTriggerEvents;
    }

    return PokFileSystem::WriteFile(
        convertibleScene.fileName,
        output,
        FileType::SCENE);
}


json ConvertibleSceneToPokSceneTool::RetrieveTransformFrom(
    const json& gameObject) const
{
    // Create output json.
    json outputTransform = json();

    if (CheckJsonExists(gameObject, "transform")) {
        // Retrieve input transform.
        json inputTransform = gameObject["transform"];

        //Quality insurance to be sure everything is correctly exported
        json outputPosition = json();
        outputPosition["x"] = inputTransform["position"]["x"];
        outputPosition["y"] = inputTransform["position"]["y"];
        outputPosition["z"] = inputTransform["position"]["z"];

        json outputRotation = json();
		float rotationX = inputTransform["rotation"]["x"];
		float rotationY = inputTransform["rotation"]["y"];
		float rotationZ = inputTransform["rotation"]["z"];
		rotationX *= math::kDeg2Rad;
		rotationY *= math::kDeg2Rad;
		rotationZ *= math::kDeg2Rad;
        outputRotation["x"] = rotationX;
        outputRotation["y"] = rotationY;
        outputRotation["z"] = rotationZ;

		//Quality insurance to be sure everything is correctly exported
        json outputScale = json();
		outputScale["x"] = inputTransform["scale"]["x"];
		outputScale["y"] = inputTransform["scale"]["y"];
		outputScale["z"] = inputTransform["scale"]["z"];

        outputTransform["position"] = outputPosition;
        outputTransform["rotation"] = outputRotation;
        outputTransform["scale"] = outputScale;
    }

    // Set retrieved values in the output.
    return outputTransform;
}

json ConvertibleSceneToPokSceneTool::RetrieveColliderFrom(
    const json& gameObject) const
{
    json outputCollider = json();

    if (CheckJsonExists(gameObject, "colliders") && 
		CheckJsonParameter(gameObject, "colliders", nlohmann::detail::value_t::array)) {
        // We still take the array, but export only the first collider found
        json inputCollider = gameObject["colliders"][0]; 
        
        if(CheckJsonExists(inputCollider, "isTrigger")) {
			outputCollider["isTrigger"] = inputCollider["isTrigger"];
        }
		if (CheckJsonExists(inputCollider, "type")) {
			const std::string shapeName = inputCollider["type"];
			if (shapeName == "boxCollider") {
				outputCollider["shapeType"] = physics::ShapeType::BOX;
				if (CheckJsonExists(inputCollider, "size")) {
					outputCollider["shape"]["extent"] = inputCollider["size"];
				}
				if (CheckJsonExists(inputCollider, "offset")) {
					outputCollider["shape"]["positionOffset"] = inputCollider["offset"];
				}
			}
			else if (shapeName == "sphereCollider") {
				outputCollider["shapeType"] = physics::ShapeType::SPHERE;
				if (CheckJsonExists(inputCollider, "radius")) {
					outputCollider["shape"]["radius"] = inputCollider["radius"];
				}
				if (CheckJsonExists(inputCollider, "offset")) {
					outputCollider["shape"]["positionOffset"] = inputCollider["offset"];
				}
			}
			else if (shapeName == "meshCollider") {
				outputCollider["shapeType"] = physics::ShapeType::MESH;
			}
		}
        /*
        else if (shapeName == "rectCollider2D") {
			outputCollider[i]["shapeType"] = physics::ShapeType::RECT;
		}*/

    }

    return outputCollider;
}

json ConvertibleSceneToPokSceneTool::RetrieveSplineFrom(
    const json& gameObject) const
{
    json outputSpline = json();

	//It's a quality insurance to ensure there are every parameters in the original scene
    if (CheckJsonExists(gameObject, "spline")) {
		json inputSpline = gameObject["spline"];

		if (CheckJsonExists(inputSpline, "points")) {
			json inputPoints = inputSpline["points"];
			const size_t pointsCount = inputPoints.size();

			if (pointsCount > 0) {

				json outputPoints = json();
				for (size_t i = 0; i < pointsCount; i++) {
					json outputSinglePoint = json();
					//It's a quality insurance to ensure there are every parameters in the original scene
					outputSinglePoint["x"] = inputPoints[i]["x"];
					outputSinglePoint["y"] = inputPoints[i]["y"];
					outputSinglePoint["z"] = inputPoints[i]["z"];

					if (!outputSinglePoint.is_null()) {
						outputPoints.push_back(outputSinglePoint);
					}
				}

				if (!outputPoints.is_null()) {
					outputSpline["points"] = outputPoints;
				}
			}
		}
    }

    return outputSpline;
}

json ConvertibleSceneToPokSceneTool::RetrieveLightFrom(
	const json& gameObject) const
{
	json outputLight = json();

	if (CheckJsonExists(gameObject, "light")) {
		json inputLight = gameObject["light"];

		if (CheckJsonExists(inputLight, "type")) {
            if(inputLight["type"] == "Point") {
				outputLight["type"] = graphics::LightType::POINT;
            }else if(inputLight["type"] == "Spot") {
				outputLight["type"] = graphics::LightType::SPOT;
            }else if(inputLight["type"] == "Directional") {
				outputLight["type"] = graphics::LightType::DIRECTIONAL;
            }

			if (CheckJsonExists(inputLight, "color")) {
				outputLight["color"] = inputLight["color"];
			}

			if (CheckJsonExists(inputLight, "intensity")) {
				outputLight["intensity"] = inputLight["intensity"];
			}

			//Direction of directionnal light
			if ((inputLight["type"] == "Directional" || inputLight["type"] == "spot") &&
				CheckJsonExists(gameObject, "transform")
				) {
				json inputTransform = gameObject["transform"]; // Direction is stored in the transform !
				json outputRotation = json();
				outputRotation["x"] = inputTransform["rotation"]["x"];
				outputRotation["y"] = inputTransform["rotation"]["y"];
				outputRotation["z"] = inputTransform["rotation"]["z"];

				outputLight["direction"] = outputRotation;
			}
			//Direction of spot light
			if (inputLight["type"] == "Spot" &&
				CheckJsonExists(inputLight, "spotAngle")
				) {
				outputLight["spotAngle"] = inputLight["spotAngle"];
			}

			if (inputLight["type"] == "Point" || inputLight["type"] == "Spot" ) {
				if (CheckJsonExists(inputLight, "areaSize")) {
					outputLight["areaSize"] = inputLight["areaSize"];
				}
			}
		}
	}
	return outputLight;
}

json ConvertibleSceneToPokSceneTool::SetComponentFor(
	const ecs::ComponentType::ComponentType componentType,
    const json& gameObject,
    const json& component) const
{
	json outputJson = gameObject;
    if (!component.is_null()) {
		outputJson[ComponentUtility::GetComponentName(componentType, true)] = component;
    }
	return outputJson;
}
}
} //namespace poke::editor
