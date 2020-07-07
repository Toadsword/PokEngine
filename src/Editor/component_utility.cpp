#include <Editor/component_utility.h>

#include <algorithm>

#include <ImGuizmo.h>

#include <misc/cpp/imgui_stdlib.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <PhysicsEngine/Shapes/sphere_shape.h>
#include <PhysicsEngine/Shapes/ellipsoid_shape.h>
#include <PhysicsEngine/Shapes/mesh_shape.h>
#include <PhysicsEngine/Shapes/box_shape.h>
#include <Editor/Tools/resource_accessor_tool.h>
#include <Editor/ResourcesManagers/editor_meshes_manager.h>
#include <Editor/ResourcesManagers/editor_materials_manager.h>
#include <Editor/imgui_gradient.h>
#include <Utility/log.h>

namespace poke::editor {

// Display components
const physics::Rigidbody ComponentUtility::DisplayAndModifyRigidbody(
    const physics::Rigidbody& rigidbody)
{
    physics::Rigidbody newRigidBody = rigidbody;

    if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::RIGIDBODY).c_str())) {
        //BodyType
        const physics::RigidbodyType bodyType = rigidbody.type;
        const char* items[] = {"Dynamic", "Kinematic", "Static"};
        int itemCurrent = static_cast<int>(bodyType);

        if (ImGui::BeginCombo("Body Type", items[static_cast<int>(bodyType)])) {
            for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
                const bool isSelected = itemCurrent == i;

                if (ImGui::Selectable(items[i], isSelected)) { itemCurrent = i; }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }
            newRigidBody.type = static_cast<physics::RigidbodyType>(itemCurrent);
            ImGui::EndCombo();
        }
        //Linear drag
        float linearDrag = rigidbody.linearDrag;
        if (ImGui::DragFloat("Linear Drag", &linearDrag, kDragSpeed, 0)) {
            newRigidBody.linearDrag = linearDrag;
        }
        //Angular drag
        float angularDrag = rigidbody.angularDrag;
        if (ImGui::DragFloat("Angular Drag", &angularDrag, kDragSpeed, 0)) {
            newRigidBody.angularDrag = angularDrag;
        }

        const ImVec2 padding{10, 10};
        const ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));
        ImGui::BeginGroup();
        ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "Infos");
        ImGui::TextColored(
            {0.5f, 0.5f, 0.5f, 1.0f},
            "Linear velocity : (%.2f, %.2f, %.2f)",
            rigidbody.linearVelocity.x,
            rigidbody.linearVelocity.y,
            rigidbody.linearVelocity.z);
        ImGui::TextColored(
            {0.5f, 0.5f, 0.5f, 1.0f},
            "Angular velocity : (%.2f, %.2f, %.2f)",
            rigidbody.angularVelocity.x,
            rigidbody.angularVelocity.y,
            rigidbody.angularVelocity.z);
        ImGui::EndGroup();
        ImVec2 p1 = ImVec2(
            ImGui::GetItemRectMax().x + padding.x,
            ImGui::GetItemRectMax().y + padding.y);
        ImGui::Dummy(ImVec2(0.0f, padding.y));
        ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(120, 120, 120, 255), 10.0f);
    }
    return newRigidBody;
}

const physics::Collider ComponentUtility::DisplayAndModifyCollider(
    const physics::Collider& collider)
{
    physics::Collider newCollider = collider;
    if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::COLLIDER).c_str())) {
        //Is Trigger
        bool isTrigger = collider.isTrigger;
        if (ImGui::Checkbox("IsTrigger", &isTrigger)) { newCollider.isTrigger = isTrigger; }

        //Shape type
        physics::ShapeType shapeType = collider.shapeType;
        const char* items[] = {"Box", "Sphere", "Ellipsoid", "Mesh", "Rect"};
        int itemCurrent = static_cast<int>(shapeType);

        if (ImGui::BeginCombo("Shape Type", items[static_cast<int>(shapeType)])) {
            for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
                const bool isSelected = itemCurrent == i;

                if (ImGui::Selectable(items[i], isSelected)) { itemCurrent = i; }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }

            //Change shape if needed
            if (itemCurrent != static_cast<int>(shapeType)) {
                shapeType = static_cast<physics::ShapeType>(itemCurrent);

                switch (shapeType) {
                case physics::ShapeType::BOX:
                    newCollider.SetShape(physics::BoxShape());
                    break;
                case physics::ShapeType::SPHERE:
                    newCollider.SetShape(physics::SphereShape());
                    break;
                case physics::ShapeType::ELLIPSOID:
                    newCollider.SetShape(physics::EllipsoidShape());
                    break;
                case physics::ShapeType::MESH:
                    newCollider.SetShape(physics::MeshShape());
                    break;
                case physics::ShapeType::RECT:
                    break;
                default: ;
                }
            }
            ImGui::EndCombo();
        }

        //Shapes
        switch (shapeType) {
        case physics::ShapeType::BOX: {
            physics::BoxShape box = collider.boxShape;

            float center[3] = {
                box.GetCenter().x,
                box.GetCenter().y,
                box.GetCenter().z
            };
            if (ImGui::DragFloat3("Center", center, kDragSpeed, 0)) {
                box.SetCenter(math::Vec3(center[0], center[1], center[2]));
            }

            float extent[3] = {
                box.GetExtent().x,
                box.GetExtent().y,
                box.GetExtent().z
            };
            if (ImGui::DragFloat3("Extent", extent, kDragSpeed, 0)) {
                box.SetExtent(math::Vec3(extent[0], extent[1], extent[2]));
            }

            newCollider.SetShape(box);
        }
        break;
        case physics::ShapeType::SPHERE: {
            physics::SphereShape sphere = collider.sphereShape;

            float center[3] = {
                sphere.GetCenter().x,
                sphere.GetCenter().y,
                sphere.GetCenter().z
            };
            if (ImGui::DragFloat3("Center", center, kDragSpeed, 0)) {
                sphere.SetCenter(math::Vec3(center[0], center[1], center[2]));
            }

            float radius = sphere.GetRadius();
            if (ImGui::DragFloat("Radius", &radius, kDragSpeed, 0)) { sphere.SetRadius(radius); }

            newCollider.SetShape(sphere);
        }
        break;
        case physics::ShapeType::ELLIPSOID: {
            physics::EllipsoidShape ellipsoid = collider.ellipsoidShape;

            float center[3] = {
                ellipsoid.GetCenter().x,
                ellipsoid.GetCenter().y,
                ellipsoid.GetCenter().z
            };
            if (ImGui::DragFloat3("Center", center, kDragSpeed)) {
                ellipsoid.SetCenter(math::Vec3(center[0], center[1], center[2]));
            }

            float radius = ellipsoid.GetRadius();
            if (ImGui::DragFloat("Radius", &radius, kDragSpeed)) { ellipsoid.SetRadius(radius); }

            newCollider.SetShape(ellipsoid);
        }
        break;
        case physics::ShapeType::MESH: {
            physics::MeshShape meshShape = collider.meshShape;

            float extent[3] = {
                meshShape.GetMeshExtent().x,
                meshShape.GetMeshExtent().y,
                meshShape.GetMeshExtent().z
            };
            if (ImGui::DragFloat3("Mesh Extent", extent, kDragSpeed, 0)) {
                meshShape.SetMeshExtent(math::Vec3(extent[0], extent[1], extent[2]));
            }

            float offset[3] = {
                meshShape.GetOffset().x,
                meshShape.GetOffset().y,
                meshShape.GetOffset().z
            };
            if (ImGui::DragFloat3("Offset", offset, kDragSpeed, 0)) {
                meshShape.SetOffset(math::Vec3(offset[0], offset[1], offset[2]));
            }

            newCollider.SetShape(meshShape);
        }
        break;
        case physics::ShapeType::RECT: { ImGui::Text("Not yet implemented\n"); }
        break;
        default:
            break;
        }
    }
    return newCollider;
}

const math::Transform ComponentUtility::DisplayAndModifyTransform(
    const math::Transform& transform)
{
    math::Transform newTransform = transform;
    
    if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::TRANSFORM).c_str())) {

		float matrixTranslation[3]{
	        newTransform.GetLocalPosition().x,
	        newTransform.GetLocalPosition().y,
	        newTransform.GetLocalPosition().z
		};
        if(ImGui::DragFloat3("Position", matrixTranslation, kDragSpeed)) {
			newTransform.SetLocalPosition(
			    {
				    matrixTranslation[0],
				    matrixTranslation[1],
				    matrixTranslation[2]
			    });
        }

		float matrixRotation[3]{
	        newTransform.GetLocalRotation().x,
	        newTransform.GetLocalRotation().y,
	        newTransform.GetLocalRotation().z
		};

        matrixRotation[0] *= math::kRad2Deg;
        matrixRotation[1] *= math::kRad2Deg;
        matrixRotation[2] *= math::kRad2Deg;

        if(ImGui::DragFloat3("Rotation", matrixRotation, kDragSpeed)) {

			matrixRotation[0] *= math::kDeg2Rad;
			matrixRotation[1] *= math::kDeg2Rad;
			matrixRotation[2] *= math::kDeg2Rad;

			newTransform.SetLocalRotation(
				{
					matrixRotation[0],
					matrixRotation[1],
					matrixRotation[2]
				});
        }

		float matrixScale[3]{
	        newTransform.GetLocalScale().x,
	        newTransform.GetLocalScale().y,
	        newTransform.GetLocalScale().z
		};
        if(ImGui::DragFloat3("Scale", matrixScale, kDragSpeed)) {
			newTransform.SetLocalScale(
				{
					matrixScale[0],
					matrixScale[1],
					matrixScale[2]
				});
        }
    }

    return newTransform;
}

const ecs::SplineFollower ComponentUtility::DisplayAndModifySplineFollower(
    const ecs::SplineFollower& splineFollower)
{
    ecs::SplineFollower newSplineFollower = splineFollower;

    if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::SPLINE_FOLLOWER).c_str())) {
        //Spline speed
        float speed = newSplineFollower.speed;
        if (ImGui::DragFloat("Speed", &speed, kDragSpeed)) { newSplineFollower.speed = speed; }

        //Use to display spline's points
        if (ImGui::CollapsingHeader("Spline points")) {
            std::vector<math::Vec3> points = newSplineFollower.spline.GetPoints();
            std::vector<math::Vec3> newPoints;
            for (int i = 0; i < points.size(); i++) {
                ImGui::DragFloat3(
                    ("##splinePoint" + std::to_string(i)).c_str(),
                    &points[i][0],
                    kDragSpeed);
                ImGui::SameLine();
                if (ImGui::Button(("X##deleteSplinePoint" + std::to_string(i)).c_str())) {
                    std::cout << i << "\n";
                } else { newPoints.push_back(points[i]); }
            }

            if (newSplineFollower.spline.GetSize() < newSplineFollower.spline.GetMaxSize()) {
                if (ImGui::Button("Add point##SplineFollower")) {
                    newPoints.emplace_back(0, 0, 0);
                }
            }

            if (newSplineFollower.spline.GetSize() < 4) {
                ImGui::TextColored(
                    {1, 0, 0, 1},
                    "If the number of points if smaller\n than 4, the spline cannot work");
            }
			newSplineFollower.spline.SetPoints(newPoints);
        }

        const ImVec2 padding{10, 10};
        const ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));
        ImGui::BeginGroup();
        ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "Info");
        ImGui::TextColored(
            {0.5f, 0.5f, 0.5f, 1.0f},
            "%s",
            ("Last point : " + std::to_string(newSplineFollower.lastPoint)).c_str());
        ImGui::TextColored(
            {0.5f, 0.5f, 0.5f, 1.0f},
            "%s",
            ("Segement percentage : " + std::to_string(newSplineFollower.segmentPercentage)).c_str());
        ImGui::TextColored(
            {0.5f, 0.5f, 0.5f, 1.0f},
            "%s",
            ("Total percentage : " + std::to_string(newSplineFollower.totalPercentage)).c_str());
        ImGui::EndGroup();
        ImVec2 p1 = ImVec2(
            ImGui::GetItemRectMax().x + padding.x,
            ImGui::GetItemRectMax().y + padding.y);
        ImGui::Dummy(ImVec2(0.0f, padding.y));
        ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(120, 120, 120, 255), 10.0f);
    }
    return newSplineFollower;
}

const graphics::Model ComponentUtility::DisplayAndModifyModel(
    const graphics::Model& model)
{
    graphics::Model newModel = model;

    if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::MODEL).c_str())) {
        auto& editorMaterialManager = editor_->GetResourcesManagerContainer().editorMaterialsManager;
        auto& editorMeshManager = editor_->GetResourcesManagerContainer().editorMeshesManager;
        graphics::MaterialType materialType = editorMaterialManager.GetMaterialType(model.materialID);

        //Selection of material
        std::string materialName = editorMaterialManager.GetMaterialName(model.materialID);

        //Load all the materials
		ImGui::Text(materialName.c_str());
		ImGui::SameLine();
        if(openNewMaterialPopup_) {
			openNewMaterialPopup_ = false;
			ImGui::OpenPopup("NewMaterial");
        }

        if(ImGui::Button("Change Material")) {
			const std::string path = PokFileSystem::GetPath(FileType::MATERIAL, FolderType::ROM);
			comboBoxElements_ = PokFileSystem::GetAllFilesInDirectories(path, true, true);

			for (std::string& element : comboBoxElements_) {
				size_t nameSize = element.size() - path.size();
				element = element.substr(path.size(), nameSize);
			}

			std::string extension = PokFileSystem::GetExtension(FileType::MATERIAL);
            for (std::string& element : comboBoxElements_) {
                element = element.substr(0, element.size() - extension.size());
            }

			ImGui::OpenPopup("Select_material");
        }

		if (ImGui::BeginPopup("Select_material")) {
			for (const std::string& selectableMaterialName : comboBoxElements_) {
				if (ImGui::MenuItem(selectableMaterialName.c_str())) {
					newModel.materialID = math::HashString(selectableMaterialName);
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create new material")) {
				ImGui::CloseCurrentPopup();
				openNewMaterialPopup_ = true;
				newName_ = "new_material";
			}
			ImGui::EndPopup();
        }

		bool confirmCreate = false;
		if (ImGui::BeginPopupModal("NewMaterial")) {
			ImGui::InputTextWithHint(
				"New material Name",
				"Enter material Name",
				&newName_);

            //Combobox
			std::string materialTypes[4] = { "Diffuse", "Skybox", "Trail", "Particle"};
			if (ImGui::BeginCombo("MaterialType", materialTypes[static_cast<uint8_t>(newMaterialType_)].c_str())) {
				for (uint8_t i = 0; i < IM_ARRAYSIZE(materialTypes); i++) {
					const bool isSelected = static_cast<uint8_t>(newMaterialType_) == i;

					if (ImGui::Selectable(materialTypes[i].c_str(), isSelected)) {
					    newMaterialType_ = static_cast<graphics::MaterialType>(i);
					}
					if (isSelected) {
					    ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Separator();

			ImGui::Checkbox(
				"Set to current object",
				&setWithNew_);

			if (ImGui::Button("Create Material")) {
				if (PokFileSystem::CheckFileExists(
					PokFileSystem::GetFullPath(
						newName_,
						FileType::MATERIAL,
						FolderType::SAVE_IN_ROM)
				))
				{
					ImGui::OpenPopup("Confirm Replace Material");
				}
				else {
					confirmCreate = true;
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::BeginPopupModal("Confirm Replace Material")) {
				ImGui::Text("A material already exists with that name, want to replace ?");

				if (ImGui::Button("Yes")) {
					confirmCreate = true;
					ImGui::CloseCurrentPopup(); // Close "Confirm Replace Material" popup
					ImGui::CloseCurrentPopup(); // Close "Name new material"
				}
				ImGui::SameLine();
				if (ImGui::Button("No")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::EndPopup();
		}
		if (confirmCreate) {
			XXH64_hash_t hash = CreateNewMaterial(newName_, newMaterialType_);
			if (setWithNew_) {
				newModel.materialID = hash;
			}
		}

		const std::string& meshName = editorMeshManager.GetMeshName(model.meshID);
		ImGui::Text(meshName.c_str());
		ImGui::SameLine();

		if (ImGui::Button("Select mesh")) {
			//Add all meshes from folder
			std::string path = PokFileSystem::GetPath(FileType::MESH, FolderType::ROM);
			comboBoxElements_ = PokFileSystem::GetAllFilesInDirectories(path, true, true);

			for (std::string& element : comboBoxElements_) {
				size_t nameSize = element.size() - path.size();
				element = element.substr(path.size(), nameSize);
			}

			ImGui::OpenPopup("Select_mesh");
		}
		if (ImGui::BeginPopup("Select_mesh")) {
			//Add primitives
			std::string primitives[7] = { "Cube", "Sphere", "Plane", "Quad", "LineGizmo", "CubeGizmo", "SphereGizmo" };
			for (size_t primitiveIndex = 0; primitiveIndex < 7; primitiveIndex++) {
				if (ImGui::MenuItem(primitives[primitiveIndex].c_str())) {
					newModel.meshID = math::HashString(primitives[primitiveIndex]);
				}
			}

			for (const std::string& selectableMeshName : comboBoxElements_) {
				if (ImGui::MenuItem(selectableMeshName.c_str())) {
					newModel.meshID = math::HashString(selectableMeshName);
				}
			}
			ImGui::EndPopup();
		}

        if (ImGui::TreeNode("Material")) {
            switch (editorMaterialManager.GetMaterialType(materialName)) {
            case graphics::MaterialType::SKYBOX: {
                auto newMaterialSkybox = DisplayAndModifyMaterialSkybox(
                    editorMaterialManager.GetMaterialSkybox(materialName));
                editorMaterialManager.SetMaterial(newMaterialSkybox, materialName);

                if(ImGui::Button("Save material##materialSkybox")) {
					PokFileSystem::WriteFile(
						materialName, 
						newMaterialSkybox.ToJson(),
						FileType::MATERIAL, 
						FolderType::SAVE_IN_ROM);

					LogDebug("Material " + materialName + " saved !", LogType::EDITOR_LOG);
                }
            }
            break;
            case graphics::MaterialType::DIFFUSE: {
                auto newMaterialDiffuse = DisplayAndModifyMaterialDiffuse(
                    editorMaterialManager.GetMaterialDiffuse(materialName));
                editorMaterialManager.SetMaterial(newMaterialDiffuse, materialName);

				if (ImGui::Button("Save material##materialDiffuse")) {
					PokFileSystem::WriteFile(
						materialName,
						newMaterialDiffuse.ToJson(),
						FileType::MATERIAL,
						FolderType::SAVE_IN_ROM);

					LogDebug("Material " + materialName + " saved !", LogType::EDITOR_LOG);
				}
            }
            break;
            default: ;
            }
            ImGui::TreePop();
        }

        if (materialType != graphics::MaterialType::SKYBOX && ImGui::TreeNode("Mesh")) {
            graphics::Mesh& mesh = editorMeshManager.GetMesh(model.meshID);

            math::Vec3 extent = mesh.GetExtent();
            float extentVec3[3]{
                extent.x,
                extent.y,
                extent.z
            };

            ImGui::InputFloat3("extends", extentVec3, "%.3f", ImGuiInputTextFlags_ReadOnly);

            math::Vec3 positionOffset = mesh.GetPositionOffset();
            float positionOffsetVec3[3]{
                positionOffset.x,
                positionOffset.y,
                positionOffset.z
            };
            ImGui::InputFloat3(
                "Position Offset",
                positionOffsetVec3,
                "%.3f",
                ImGuiInputTextFlags_ReadOnly);

            float radius = mesh.GetRadius();
            ImGui::InputFloat("Radius", &radius, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::Text(("Vertex Count : " + std::to_string(mesh.GetVertexCount())).c_str());
            ImGui::TreePop();
        }
    }
    return newModel;
}

const ecs::Light ComponentUtility::DisplayAndModifyLight(
    const ecs::Light& light)
{
    ecs::Light newLight = light;
    if (ImGui::CollapsingHeader("Light")) {
        //Light type
        auto lightType = light.lightType;
        std::vector<std::string> items = {};

        for (int i = 0; i < static_cast<int>(graphics::LightType::LENGTH); i++) {
            items.push_back(LightTypeToString(static_cast<graphics::LightType>(i)));
        }

        int itemCurrent = static_cast<int>(lightType);

        if (ImGui::BeginCombo("Shape Type", items[static_cast<int>(lightType)].c_str())) {
            for (int i = 0; i < items.size(); i++) {
                const bool isSelected = itemCurrent == i;

                if (ImGui::Selectable(items[i].c_str(), isSelected)) {
                    itemCurrent = i;
                    //isSelectedEntityUpToDate_ = false;
                }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }

            //Change shape if needed
            if (itemCurrent != static_cast<int>(lightType)) {
                switch (static_cast<graphics::LightType>(itemCurrent)) {
                case graphics::LightType::DIRECTIONAL: {
                    if (newLight.lightType == graphics::LightType::SPOT) {
                        newLight.directionalLight.color = light.spotLight.color;
                        newLight.directionalLight.direction = light.spotLight.direction;
                    } else {
                        newLight.directionalLight.color = light.pointLight.color;
                        newLight.directionalLight.direction = light.pointLight.direction;
                    }
                }
                break;
                case graphics::LightType::SPOT:
                    if (newLight.lightType == graphics::LightType::DIRECTIONAL) {
                        newLight.spotLight.color = light.directionalLight.color;
                        newLight.spotLight.direction = light.directionalLight.direction;
                    } else {
                        newLight.spotLight.color = light.pointLight.color;
                        newLight.spotLight.direction = light.pointLight.direction;
                    }
                    break;
                case graphics::LightType::POINT:
                    if (newLight.lightType == graphics::LightType::DIRECTIONAL) {
                        newLight.pointLight.color = light.directionalLight.color;
                        newLight.pointLight.direction = light.directionalLight.direction;
                    } else {
                        newLight.pointLight.color = light.spotLight.color;
                        newLight.pointLight.direction = light.spotLight.direction;
                    }
                    break;
                default: ;
                }
                newLight.lightType = static_cast<graphics::LightType>(itemCurrent);
            }
            ImGui::EndCombo();
        }

        //Modify current light
        switch (newLight.lightType) {
        case graphics::LightType::DIRECTIONAL: {
            //Color
            Color col = newLight.directionalLight.color;
            ImGui::ColorPicker3("Color", &col[0]);
            newLight.directionalLight.color = col;

            //Intensity
            float intensity = newLight.directionalLight.intensity;
            ImGui::DragFloat("Intensity", &intensity, 0.1f);
            newLight.directionalLight.intensity = intensity;

            //Direction
            math::Vec3 dir = newLight.directionalLight.direction;
            ImGui::DragFloat3("Direction", &dir[0], 0.1f);
            newLight.directionalLight.direction = dir;
        }
        break;
        case graphics::LightType::SPOT: {
            //Color
            Color col = newLight.spotLight.color;
            ImGui::ColorPicker3("color", &col[0]);
            newLight.spotLight.color = col;

            //Intensity
            float intensity = newLight.spotLight.intensity;
            ImGui::DragFloat("Intensity", &intensity, 0.1f);
            newLight.spotLight.intensity = intensity;

            //Direction
            math::Vec3 dir = newLight.spotLight.direction;
            ImGui::DragFloat3("Direction", &dir[0], 0.1f);
            newLight.spotLight.direction = dir;

            //Angle
            float angle = newLight.spotLight.angleInDeg;
            ImGui::DragFloat("Angle", &angle, 0.1f);
            newLight.spotLight.angleInDeg = angle;

            //Range
            float range = newLight.spotLight.range;
            ImGui::DragFloat("Range", &range, 0.1f, 0);
            newLight.spotLight.range = range;
        }
        break;
        case graphics::LightType::POINT: {
            //Color
            Color col = newLight.pointLight.color;
            ImGui::ColorPicker3("color", &col[0]);
            newLight.pointLight.color = col;

            //Intensity
            float intensity = newLight.pointLight.intensity;
            ImGui::DragFloat("Intensity", &intensity, 0.1f);
            newLight.pointLight.intensity = intensity;

            //Radius
            float radius = newLight.pointLight.areaSize.x;
            ImGui::DragFloat("Radius", &radius, 0.1f);
            newLight.pointLight.areaSize.x = radius;
        }
        break;
        default:
            break;
        }
    }

    return newLight;
}

const graphics::ParticleSystem ComponentUtility::DisplayAndModifyParticleSystem(
    const graphics::ParticleSystem& particleSystem)
{
    graphics::ParticleSystem newParticleSystem = particleSystem;

    const float dragSpeed = 0.1f;
    const std::string componentName = "ParticleSystem";
    if (ImGui::CollapsingHeader(componentName.c_str())) {
        //Info about particle system
        if (ImGui::Begin("Particle System : Info")) {
            if (ImGui::Button("Restart")) { newParticleSystem.lifetime = 0; }
            ImGui::Text("lifetime %.2f", newParticleSystem.lifetime);
            ImGui::End();
        }

        if (ImGui::TreeNode("Particle System")) {
            ImGui::PushItemWidth(75);
            //Duration
            float duration = particleSystem.duration;
            if (ImGui::DragFloat("Duration", &duration, dragSpeed, 0.0f, 1000.0f, "%.2f"))
                newParticleSystem.duration = duration;

            //Looping
            bool looping = particleSystem.looping;
            if (ImGui::Checkbox("Looping", &looping))
                newParticleSystem.looping = looping;

            //Start delay
            float startDelay = particleSystem.startDelay;
            if (ImGui::DragFloat("Start Delay", &startDelay, dragSpeed, 0.0f, 1000.0f, "%.2f"))
                newParticleSystem.startDelay = startDelay;

            //Start lifetime
            float minLifetime = particleSystem.minLifetime;
            if (ImGui::DragFloat(
                "###minLifeTime",
                &minLifetime,
                dragSpeed,
                0.0f,
                particleSystem.maxLifetime,
                "%.2f"))
                newParticleSystem.minLifetime = minLifetime;
            //Life deviation
            ImGui::SameLine();
            float maxLifetime = particleSystem.maxLifetime;
            if (ImGui::DragFloat(
                "###maxdeviation",
                &maxLifetime,
                dragSpeed,
                minLifetime,
                1000.0f,
                "%.2f"))
                newParticleSystem.maxLifetime = maxLifetime;
            ImGui::SameLine();
            ImGui::Text("Lifetime");

            //min speed
            float minSpeed = particleSystem.minSpeed;
            if (ImGui::DragFloat(
                "###minSpeed",
                &minSpeed,
                dragSpeed,
                0.0f,
                particleSystem.maxSpeed,
                "%.2f"))
                newParticleSystem.minSpeed = minSpeed;
            //max speed
            ImGui::SameLine();
            float maxSpeed = particleSystem.maxSpeed;
            if (ImGui::DragFloat("###maxSpeed", &maxSpeed, dragSpeed, minSpeed, 1000.0f, "%.2f"))
                newParticleSystem.maxSpeed = maxSpeed;
            ImGui::SameLine();
            ImGui::Text("Speed");

            //Min size
            float minSize = particleSystem.minSize;
            if (ImGui::DragFloat(
                "###MinSize",
                &minSize,
                dragSpeed,
                0.0f,
                particleSystem.maxSize,
                "%.2f"))
                newParticleSystem.minSize = minSize;
            //Min size
            ImGui::SameLine();
            float maxSize = particleSystem.maxSize;
            if (ImGui::DragFloat("###MaxSize", &maxSize, dragSpeed, minSize, 100.0f, "%.2f"))
                newParticleSystem.maxSize = maxSize;
            ImGui::SameLine();
            ImGui::Text("Size");

            //Start rotation
            float startRotation = newParticleSystem.startRotation;
            if (ImGui::DragFloat("Start Rotation", &startRotation, dragSpeed, 0.0f, 360.0f, "%.2f"))
                newParticleSystem.startRotation = startRotation;

            //Start color
            ImGradient data;
            std::vector<ImGradientMark> marks;
            marks.resize(particleSystem.startColor.GetMarks().size());
            for (size_t i = 0; i < particleSystem.startColor.GetMarks().size(); i++) {
                auto mark = particleSystem.startColor.GetMarks()[i];
                marks[i].position = mark.position;
                marks[i].color[0] = mark.color.r;
                marks[i].color[1] = mark.color.g;
                marks[i].color[2] = mark.color.b;
                marks[i].color[3] = mark.color.a;
            }
            data.SetMarks(marks);
            static int draggingMarkIndex = -1;
            static int selectedMarkIndex = -1;
            static bool showGradientEditor = false;
            if (ImGui::GradientButton("Start Color", &data)) {
                showGradientEditor = !showGradientEditor;
            }
            if (showGradientEditor) {
                ImGui::Begin("Gradient Editor##ParticlSystemStartColor");
                ImGui::GradientEditor(&data, draggingMarkIndex, selectedMarkIndex);

                std::vector<ColorGradient::ColorMark> newMarks;
                newMarks.resize(data.GetMarks().size());
                for (size_t i = 0; i < data.GetMarks().size(); i++) {
                    auto mark = data.GetMarks()[i];
                    newMarks[i].position = mark.position;
                    newMarks[i].color = Color(
                        mark.color[0],
                        mark.color[1],
                        mark.color[2],
                        mark.color[3]);
                }
                newParticleSystem.startColor.SetMarks(newMarks);
                ImGui::End();
            }

            //Gravity modifier
            float gravityModifier = newParticleSystem.gravityModifier;
            if (ImGui::DragFloat("Gravity Modifier", &gravityModifier, dragSpeed))
                newParticleSystem.gravityModifier = gravityModifier;

            //Max particles
            int maxParticles = newParticleSystem.maxParticles;
            if (ImGui::DragInt("Max Particles", &maxParticles, dragSpeed, 0, 10000))
                newParticleSystem.maxParticles = maxParticles;

            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Emission")) {
            ImGui::PushItemWidth(75);
            //Particle per seconds
            float rateOverTime = newParticleSystem.rateOverTime;
            if (ImGui::DragFloat("Rate Over Time", &rateOverTime, dragSpeed, 0.0f) && rateOverTime
                >= 0.0f)
                newParticleSystem.rateOverTime = rateOverTime;

            //Particle per seconds
            float rateOverDistance = newParticleSystem.rateOverDistance;
            if (ImGui::DragFloat("Rate Over Distance", &rateOverDistance, dragSpeed, 0.0f) &&
                rateOverTime >= 0.0f)
                newParticleSystem.rateOverDistance = rateOverDistance;
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Shape")) {
            ImGui::PushItemWidth(75);
            //Shape type
            graphics::EmitterType shapeType = particleSystem.emitter.type;
            const char* items[] = {"Circle", "Line", "Point", "Sphere", "Cone"};
            int itemCurrent = static_cast<int>(shapeType);

            if (ImGui::BeginCombo("Shape Type", items[static_cast<int>(shapeType)])) {
                for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
                    const bool isSelected = itemCurrent == i;

                    if (ImGui::Selectable(items[i], isSelected)) { itemCurrent = i; }
                    if (isSelected) { ImGui::SetItemDefaultFocus(); }
                }

                //Change shape if needed
                if (itemCurrent != static_cast<int>(shapeType)) {
                    shapeType = static_cast<graphics::EmitterType>(itemCurrent);
                    newParticleSystem.emitter.type = shapeType;
                }
                ImGui::EndCombo();
            }

            switch (newParticleSystem.emitter.type) {
            case graphics::EmitterType::CIRCLE: {
                //Radius
                auto radius = newParticleSystem.emitter.circle.radius;
                ImGui::DragFloat("Radius", &radius, dragSpeed, 0.0f);
                newParticleSystem.emitter.circle.radius = radius;

                //Direction
                math::Vec3 direction = newParticleSystem.emitter.circle.direction;
                ImGui::DragFloat3("Direction", &direction[0], dragSpeed, 0.0f);
                newParticleSystem.emitter.circle.direction = (direction);
            }
            break;
            case graphics::EmitterType::LINE: {
                //Length
                auto length = newParticleSystem.emitter.line.length;
                ImGui::DragFloat("Length", &length, dragSpeed, 0.0f);
                newParticleSystem.emitter.line.length = (length);

                //Axis
                math::Vec3 axis = newParticleSystem.emitter.line.axis;
                ImGui::DragFloat3("Axis", &axis[0], dragSpeed, 0.0f);
                newParticleSystem.emitter.line.axis = (axis);
            }
            break;
            case graphics::EmitterType::POINT: {
                //Point
                math::Vec3 point = newParticleSystem.emitter.point.point;
                ImGui::DragFloat3("Point", &point[0], dragSpeed, 0.0f);
                newParticleSystem.emitter.point.point = (point);
            }
            break;
            case graphics::EmitterType::SPHERE: {
                //Radius
                auto radius = newParticleSystem.emitter.sphere.radius;
                ImGui::DragFloat("Radius", &radius, dragSpeed, 0.0f);
                newParticleSystem.emitter.sphere.radius = (radius);
            }
            break;
			case graphics::EmitterType::CONE: {
				//Radius
				auto angle = newParticleSystem.emitter.cone.angle;
				angle *= math::kRad2Deg;
				ImGui::DragFloat("Angle", &angle, dragSpeed, 0.0f);
				if (angle < 0.0f) { angle = 0.0f; }
				if (angle > 180.0f) { angle = 180.0f; }
				angle *= math::kDeg2Rad;
				newParticleSystem.emitter.cone.angle = (angle);

				//Direction
				math::Vec3 direction = newParticleSystem.emitter.cone.direction;
				ImGui::DragFloat3("Direction", &direction[0], dragSpeed, 0.0f);
				newParticleSystem.emitter.cone.direction = (direction);
			}
			break;
            default: ;
            }

            //Randomize direction
            float randomizeDirection = newParticleSystem.randomizeDirection;
            ImGui::DragFloat("Randomize direction", &randomizeDirection, dragSpeed, 0.0f, 1.0f);
            newParticleSystem.randomizeDirection = randomizeDirection;

            //Randomize position
            float randomizePosition = newParticleSystem.randomizePosition;
            ImGui::DragFloat("Randomize position", &randomizePosition, dragSpeed, 0.0f);
            newParticleSystem.randomizePosition = randomizePosition;
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Color over lifetime")) {
            ImGui::PushItemWidth(75);
            //Color over lifetime
            ImGradient data;
            std::vector<ImGradientMark> marks;
            marks.resize(particleSystem.colorOverLifetime.GetMarks().size());
            for (size_t i = 0; i < particleSystem.colorOverLifetime.GetMarks().size(); i++) {
                auto mark = particleSystem.colorOverLifetime.GetMarks()[i];
                marks[i].position = mark.position;
                marks[i].color[0] = mark.color.r;
                marks[i].color[1] = mark.color.g;
                marks[i].color[2] = mark.color.b;
                marks[i].color[3] = mark.color.a;
            }
            data.SetMarks(marks);
            static int draggingMarkIndex = -1;
            static int selectedMarkIndex = -1;
            static bool showGradientEditor = false;
            if (ImGui::GradientButton("Color", &data)) { showGradientEditor = !showGradientEditor; }
            if (showGradientEditor) {
                ImGui::Begin("Gradient Editor");
                ImGui::GradientEditor(&data, draggingMarkIndex, selectedMarkIndex);

                std::vector<ColorGradient::ColorMark> newMarks;
                newMarks.resize(data.GetMarks().size());
                for (size_t i = 0; i < data.GetMarks().size(); i++) {
                    auto mark = data.GetMarks()[i];
                    newMarks[i].position = mark.position;
                    newMarks[i].color = Color(
                        mark.color[0],
                        mark.color[1],
                        mark.color[2],
                        mark.color[3]);
                }
                newParticleSystem.colorOverLifetime.SetMarks(newMarks);
                ImGui::End();
            }
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

		//Selection of material
		auto& editorMaterialManager = editor_->GetResourcesManagerContainer().editorMaterialsManager;
		std::string materialName = editorMaterialManager.GetMaterialName(newParticleSystem.materialID);

		const std::vector<std::string>& items = editorMaterialManager.GetMaterialParticlesNames();

		if (ImGui::BeginCombo("Selected material", materialName.c_str())) {
			for (auto& item : items) {
				bool isSelected = materialName == item;
				if (ImGui::Selectable(item.c_str(), isSelected)) { materialName = item; }
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}   

		newParticleSystem.materialID = math::HashString(materialName);

		if (ImGui::TreeNode("Material")) {
			auto material = editorMaterialManager.GetMaterialParticle(newParticleSystem.materialID);
			auto newMaterial = DisplayAndModifyMaterialParticle(material);
			if (material != newMaterial) {
				editorMaterialManager.SetMaterial(newMaterial, newParticleSystem.materialID);
			}
			ImGui::TreePop();
		}
    }

    return newParticleSystem;
}

const ecs::EditorComponent ComponentUtility::DisplayAndModifyEditorComponent(
	const ecs::EditorComponent editorComponent)
{
	ecs::EditorComponent newEditorComponent = editorComponent;

	if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::EDITOR_COMPONENT).c_str())) {
		std::string newName = newEditorComponent.name;
		if(ImGui::InputText(
			"Name",
			&newName)) {
			newEditorComponent.name = newName;
		}

		std::string newPrefabName = newEditorComponent.prefabName;
		if(ImGui::InputText(
			"Prefab Name",
			&newPrefabName)) {
			newEditorComponent.prefabName = newPrefabName;
		}
	}
	return newEditorComponent;
}

const ecs::TrailRenderer ComponentUtility::DisplayAndModifyTrailRenderer(
    const ecs::TrailRenderer& trailRenderer)
{
	ecs::TrailRenderer newTrailRenderer = trailRenderer;

	if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::TRAIL_RENDERER).c_str())) {
        
		//Lifetime
		float lifetime = trailRenderer.lifetime;
		if (ImGui::DragFloat("Lifetime", &lifetime, kDragSpeed, 0))
			newTrailRenderer.lifetime = lifetime;

		//Change time
		float changeTime = trailRenderer.changeTime;
		if (ImGui::DragFloat("Change time", &changeTime, kDragSpeed, 0))
			newTrailRenderer.changeTime = changeTime;

		//Width start
		float widthStart = trailRenderer.widthStart;
		if (ImGui::DragFloat("Width start", &widthStart, kDragSpeed, 0))
			newTrailRenderer.widthStart = widthStart;

		//Width end
		float widthEnd = trailRenderer.widthEnd;
		if (ImGui::DragFloat("Width end", &widthEnd, kDragSpeed, 0))
			newTrailRenderer.widthEnd = widthEnd;

		//Width end
		float vertexDistanceMin = trailRenderer.vertexDistanceMin;
		if (ImGui::DragFloat("Vertex min distance", &vertexDistanceMin, kDragSpeed))
			newTrailRenderer.vertexDistanceMin = vertexDistanceMin;

		//Render direction
		math::Vec3 renderDirection = trailRenderer.renderDirection;
		if (ImGui::DragFloat3("Render direction", &renderDirection[0]))
			newTrailRenderer.renderDirection = renderDirection;

		//Is paused
		bool isPaused = trailRenderer.isPaused;
		if (ImGui::Checkbox("Pause", &isPaused))
			newTrailRenderer.isPaused = isPaused;

        //materialID
		ResourceID materialID;

		const ImVec2 padding{ 10, 10 };
		const ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));
		ImGui::BeginGroup();
		ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, "Infos");
		ImGui::TextColored(
			{ 0.5f, 0.5f, 0.5f, 1.0f },
			"Number of points : %.d",
			newTrailRenderer.GetNumberOfPoints());
		ImGui::EndGroup();
		ImVec2 p1 = ImVec2(
			ImGui::GetItemRectMax().x + padding.x,
			ImGui::GetItemRectMax().y + padding.y);
		ImGui::Dummy(ImVec2(0.0f, padding.y));
		ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(120, 120, 120, 255), 10.0f);

		//Selection of material
		auto& editorMaterialManager = editor_->GetResourcesManagerContainer().editorMaterialsManager;
		std::string materialName = editorMaterialManager.GetMaterialName(trailRenderer.materialID);

		std::vector<std::string> items = editorMaterialManager.GetMaterialTrailNames();

		if (ImGui::BeginCombo("Selected material", materialName.c_str())) {
			for (auto& item : items) {
				bool isSelected = (materialName == item);
				if (ImGui::Selectable(item.c_str(), isSelected)) { materialName = item; }
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		newTrailRenderer.materialID = math::HashString(materialName);

		if (ImGui::TreeNode("Material")) {
			auto material = editorMaterialManager.GetMaterialTrail(newTrailRenderer.materialID);
			auto newMaterial = DisplayAndModifyMaterialTrail(material);
			if (material != newMaterial) {
				editorMaterialManager.SetMaterial(newMaterial, newTrailRenderer.materialID);
			}
			ImGui::TreePop();
		}
	}
    

	return newTrailRenderer;
}

const MaterialSkybox ComponentUtility::DisplayAndModifyMaterialSkybox(
    const MaterialSkybox& material)
{
    MaterialSkybox newMaterial = material;

    //Blend
    auto blend = material.GetBlend();
    if (ImGui::SliderFloat("Blend value", &blend, 0, 1)) { newMaterial.SetBlend(blend); }

    //Color
    Color diffuseColor = material.GetColorDiffuse();
    if (ImGui::ColorEdit4("Diffuse color", &diffuseColor[0], ImGuiColorEditFlags_NoAlpha)) {
        newMaterial.SetColorDiffuse(diffuseColor);
    }

    //Image cube
    //TODO(@Nico) Select folder for skyboxes
    //ImGui::Text("MRA map");
    //ImGui::Text("Todo : Display current image");

    //Resource outResource = ResourceAccessorTool::DisplayGetResource("Change", ResourceAccessorTool::DisplayType::BUTTON, FileType::TEXTURE, FolderType::ROM);
    //if (ResourceAccessorTool::IsResourceValid(outResource)) {
    //	size_t basePathSize = PokFileSystem::GetPath(FileType::TEXTURE, FolderType::ROM).size();
    //	std::string fromTexturePath = outResource.path.substr(basePathSize, outResource.path.size() - basePathSize);
    //	editor_->GetResourcesManagerContainer().editorTexturesManager.AddTextureCube(fromTexturePath);

    //	newMaterial.SetTexture(editor_->GetResourcesManagerContainer().editorTexturesManager.GetTextureCubeByName(fromTexturePath));
    //}
    return newMaterial;
}

const MaterialDiffuse ComponentUtility::DisplayAndModifyMaterialDiffuse(
    const MaterialDiffuse& material)
{
    MaterialDiffuse newMaterial = material;

    //Metallic
    auto metallic = material.GetMetallic();
    if (ImGui::SliderFloat("Metallic", &metallic, 0, 1)) { newMaterial.SetMetallic(metallic); }

    //Metallic
    auto roughness = material.GetRoughness();
    if (ImGui::SliderFloat("Roughness", &roughness, 0, 1)) { newMaterial.SetRoughness(roughness); }

    //Color
    auto diffuseColor = material.GetColor();
    if (ImGui::ColorEdit4("Color", &diffuseColor[0], ImGuiColorEditFlags_HDR)) {
        newMaterial.SetColor(diffuseColor);
    }

    //Base Color Texture

	auto& editorTextureManager = editor_->GetResourcesManagerContainer().editorTexturesManager;

	{
		const auto textureBaseColor = newMaterial.GetTextureBaseColor();
		ImGui::Text("BC : ");
		ImGui::SameLine();
		if (textureBaseColor) {
			ImGui::Text(textureBaseColor->GetFilename().c_str());
		}
		else {
			ImGui::Text("empty");
		}
		ImGui::SameLine();

		Resource outResource = ResourceAccessorTool::DisplayGetResource("o##BaseColor", ResourceAccessorTool::DisplayType::BUTTON, FileType::TEXTURE, FolderType::ROM);
		if (ResourceAccessorTool::IsResourceValid(outResource)) {
			size_t basePathSize = PokFileSystem::GetPath(FileType::TEXTURE, FolderType::ROM).size();
			std::string fromTexturePath = outResource.path.substr(basePathSize, outResource.path.size() - basePathSize);
			editorTextureManager.AddTexture2D(fromTexturePath);

			newMaterial.SetTextureBaseColor(editorTextureManager.GetTexture2DByName(fromTexturePath));
		}
	}

	{
		const auto textureNormal = newMaterial.GetTextureNormal();
		ImGui::Separator();
		ImGui::Text("Normal : ");
		ImGui::SameLine();
		if (textureNormal) {
			ImGui::Text(textureNormal->GetFilename().c_str());
		}
		else {
			ImGui::Text("empty");
		}
		ImGui::SameLine();

		Resource outResource = ResourceAccessorTool::DisplayGetResource("o##NormalMap", ResourceAccessorTool::DisplayType::BUTTON, FileType::TEXTURE, FolderType::ROM);
		if (ResourceAccessorTool::IsResourceValid(outResource)) {
			size_t basePathSize = PokFileSystem::GetPath(FileType::TEXTURE, FolderType::ROM).size();
			std::string fromTexturePath = outResource.path.substr(basePathSize, outResource.path.size() - basePathSize);
			editorTextureManager.AddTexture2D(fromTexturePath);

			newMaterial.SetTextureNormal(editorTextureManager.GetTexture2DByName(fromTexturePath));
		}
	}

	{
		const auto textureRMA = newMaterial.GetTextureMRA();
		ImGui::Separator();
		ImGui::Text("RMA : ");
		ImGui::SameLine();
        if(textureRMA) {
			ImGui::Text(textureRMA->GetFilename().c_str());
        }else {
		    ImGui::Text("empty");
        }
		ImGui::SameLine();

		Resource outResource = ResourceAccessorTool::DisplayGetResource("o##RMA", ResourceAccessorTool::DisplayType::BUTTON, FileType::TEXTURE, FolderType::ROM);
		if (ResourceAccessorTool::IsResourceValid(outResource)) {
			size_t basePathSize = PokFileSystem::GetPath(FileType::TEXTURE, FolderType::ROM).size();
			std::string fromTexturePath = outResource.path.substr(basePathSize, outResource.path.size() - basePathSize);
			editorTextureManager.AddTexture2D(fromTexturePath);

			newMaterial.SetTextureRMA(editorTextureManager.GetTexture2DByName(fromTexturePath));
		}
	}

    //IsTransparent
    auto renderMode = material.GetRenderMode();
    const char* renderModeName[] = {"Opaque", "Transparent"};
    if (ImGui::BeginCombo(
        "Selected material",
        MaterialDiffuse::RenderModeToString(renderMode).c_str())) {
        for (size_t i = 0; i < static_cast<size_t>(MaterialDiffuse::RenderMode::LENGTH); i++) {
            bool isSelected = static_cast<size_t>(renderMode) == i;
            if (ImGui::Selectable(
                MaterialDiffuse::RenderModeToString(static_cast<MaterialDiffuse::RenderMode>(i)).
                c_str(),
                isSelected)) { renderMode = static_cast<MaterialDiffuse::RenderMode>(i); }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    newMaterial.SetRenderMode(renderMode);

    return newMaterial;
}

const MaterialTrail ComponentUtility::DisplayAndModifyMaterialTrail(
	const MaterialTrail& material)
{
	MaterialTrail newMaterial = material;

	//Color
	auto diffuseColor = material.GetColor();
	if (ImGui::ColorEdit4("Color", &diffuseColor[0], ImGuiColorEditFlags_HDR)) { newMaterial.SetColor(diffuseColor); }

	//Base Color Texture
	auto& editorTextureManager = editor_->GetResourcesManagerContainer().editorTexturesManager;

	{
		ImGui::Text("Texture : Base Color : "); 
		ImGui::NextColumn();

        const Resource outResource = ResourceAccessorTool::DisplayGetResource("Change##BaseColor", ResourceAccessorTool::DisplayType::MENU_ITEM, FileType::TEXTURE, FolderType::ROM);
		if (ResourceAccessorTool::IsResourceValid(outResource)) {
            const size_t basePathSize = PokFileSystem::GetPath(FileType::TEXTURE, FolderType::ROM).size();
			const std::string fromTexturePath = outResource.path.substr(basePathSize, outResource.path.size() - basePathSize);
			editorTextureManager.AddTexture2D(fromTexturePath);

			newMaterial.SetTextureBaseColor(editorTextureManager.GetTexture2DByName(fromTexturePath));
		}
	}

	return newMaterial;
}

const MaterialParticle ComponentUtility::DisplayAndModifyMaterialParticle(
    const MaterialParticle& material)
{
	MaterialParticle newMaterial = material;

	//Base Color Texture
	auto& editorTextureManager = editor_->GetResourcesManagerContainer().editorTexturesManager;

	{
		ImGui::Text("Base Color : ");
		ImGui::NextColumn();

		const Resource outResource = ResourceAccessorTool::DisplayGetResource("Change##BaseColor", ResourceAccessorTool::DisplayType::MENU_ITEM, FileType::TEXTURE, FolderType::ROM);
		if (ResourceAccessorTool::IsResourceValid(outResource)) {
			const size_t basePathSize = PokFileSystem::GetPath(FileType::TEXTURE, FolderType::ROM).size();
			const std::string fromTexturePath = outResource.path.substr(basePathSize, outResource.path.size() - basePathSize);
			editorTextureManager.AddTexture2D(fromTexturePath);

			newMaterial.SetTextureBaseColor(editorTextureManager.GetTexture2DByName(fromTexturePath));
		}
	}

	return newMaterial;
}

const audio::AudioSource ComponentUtility::DisplayAndModifyAudioSource(
	const audio::AudioSource& audioSource)
{
	audio::AudioSource newAudioSource = audioSource;

	if (ImGui::CollapsingHeader("AudioSource")) {
		bool isEvent = newAudioSource.isEvent;
        if(ImGui::Checkbox("Is Fmod Event", &isEvent)) {
			newAudioSource.isEvent = isEvent;
            if(isEvent) {
				newAudioSource.eventName = "";
				newAudioSource.soundID = 0;
            }else {
				newAudioSource.eventName = "";
				newAudioSource.soundID = 0;
            }
        }

        if(newAudioSource.isEvent) {
			const std::string& eventName = newAudioSource.eventName;

			ImGui::Text(eventName.c_str());
			ImGui::SameLine();

			if (ImGui::Button("Select Event Name")) {
				//Add all meshes from folder
				comboBoxElements_ = AudioEngineLocator::Get().GetAllEventNames();
				ImGui::OpenPopup("Select_sound");
			}
			if (ImGui::BeginPopup("Select_sound")) {
				//Add primitives
				for (const std::string& selectableEventName : comboBoxElements_) {
					if (ImGui::MenuItem(selectableEventName.c_str())) {
						newAudioSource.eventName = selectableEventName;
					}
				}
				ImGui::EndPopup();
			}
        }else {
			const ResourceID soundID = newAudioSource.soundID;

			auto& editorSoundsManager = editor_
				->GetResourcesManagerContainer().editorSoundsManager;

			const std::string& soundName = editorSoundsManager.GetSoundName(soundID);
			ImGui::Text(soundName.c_str());
			ImGui::SameLine();

			if (ImGui::Button("Select Sound ID")) {
				//Add all meshes from folder
				const std::string path = PokFileSystem::GetPath(FileType::SOUNDS, FolderType::ROM);
				comboBoxElements_ = PokFileSystem::GetAllFilesInDirectories(path, true, true);

				ImGui::OpenPopup("Select_sound");
			}
			if (ImGui::BeginPopup("Select_sound")) {
				//Add primitives
				for (const std::string& selectableSoundName : comboBoxElements_) {
					if (ImGui::MenuItem(selectableSoundName.c_str())) {
						editorSoundsManager.AddSound(selectableSoundName);
						newAudioSource.soundID = math::HashString(selectableSoundName);
					}
				}
				ImGui::EndPopup();
			}
        }

		float minDistance = newAudioSource.GetMinDistance();
		if (ImGui::DragFloat("minDistance", &minDistance, kDragSpeed, 0)) {
			newAudioSource.SetMinDistance(minDistance);
		}
		float maxDistance = newAudioSource.GetMaxDistance();
		if (ImGui::DragFloat("maxDistance", &maxDistance, kDragSpeed, 0)) {
			newAudioSource.SetMaxDistance(maxDistance);
		}
		float volume = newAudioSource.GetVolume();
		if (ImGui::DragFloat("volume", &volume, kDragSpeed, 0, audio::kMaxVolume)) {
			newAudioSource.SetVolume(volume);
		}
		float pitch = newAudioSource.GetPitch();
		if (ImGui::DragFloat("pitch", &pitch, kDragSpeed)) {
			newAudioSource.SetPitch(pitch);
		}
		bool playOnLoadScene = newAudioSource.playOnLoadScene;
		if (ImGui::Checkbox("playOnLoadScene", &playOnLoadScene)) {
			newAudioSource.playOnLoadScene = playOnLoadScene;
		}

        if(!newAudioSource.isEvent) {
			float spatialBlend = newAudioSource.GetSpatialBlend();
            if(ImGui::SliderFloat("Spatial blend", &spatialBlend, 0, 1)) {
				newAudioSource.SetSpatialBlend(spatialBlend);
            }
        }

		ImGui::Separator();
		if (!newAudioSource.IsPlaying()) {
			if (ImGui::Button("Play")) {
				newAudioSource.Play(
					EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>().GetWorldPosition(
						editor_->GetSelectedEntityIndex()));
			}

			if (ImGui::Button("PlayInstance")) {
				newAudioSource.PlayInstance(
					EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>().GetWorldPosition(
						editor_->GetSelectedEntityIndex()));
			}
		}
		else {
			if (ImGui::Button("Stop")) {
				newAudioSource.Stop();
			}
		}

		if (newAudioSource.IsPaused() && ImGui::Button("Unpause")) {
			newAudioSource.Unpause();
		}
		else if (!newAudioSource.IsPaused() && ImGui::Button("Pause")) {
			newAudioSource.Pause();
		}

        //Debug infos
		auto worldPos = EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>().GetWorldPosition(editor_->GetSelectedEntityIndex());


		const ImVec2 padding{ 10, 10 };
		const ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));
		ImGui::BeginGroup();
		ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, "Infos");
		const auto distance = math::Vec3::GetDistance(
			CameraLocator::Get().GetPosition(), 
			worldPos);
		ImGui::TextColored(
			{ 0.5f, 0.5f, 0.5f, 1.0f },
			"Distance to listener : (%.2f)",
			distance);
		ImGui::EndGroup();
		ImVec2 p1 = ImVec2(
			ImGui::GetItemRectMax().x + padding.x,
			ImGui::GetItemRectMax().y + padding.y);
		ImGui::Dummy(ImVec2(0.0f, padding.y));
		ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(120, 120, 120, 255), 10.0f);

        //Draw wire sphere
		auto& gizmoCommandBuffer = GraphicsEngineLocator::Get().GetGizmoCommandBuffer();
		const auto color = gizmoCommandBuffer.GetColor();
		gizmoCommandBuffer.SetColor(Color::red);
		GraphicsEngineLocator::Get().GetGizmoCommandBuffer().DrawWireSphere(worldPos, minDistance * 2);
		GraphicsEngineLocator::Get().GetGizmoCommandBuffer().DrawWireSphere(worldPos, maxDistance * 2);
		gizmoCommandBuffer.SetColor(color);
	}

	return newAudioSource;
}

const ecs::SegmentRenderer ComponentUtility::DisplayAndModifySegmentRenderer(
    const ecs::SegmentRenderer& segmentRenderer)
{
	ecs::SegmentRenderer newSegmentRenderer = segmentRenderer;

	if (ImGui::CollapsingHeader("Segment Renderer")) {
        //Lifetime
		float lifetime = newSegmentRenderer.lifetime;
		if (ImGui::DragFloat("lifetime", &lifetime, kDragSpeed, 0, 100)) {
			newSegmentRenderer.lifetime = lifetime;
		}

        //Width
		float width = newSegmentRenderer.width;
		if (ImGui::DragFloat("width", &width, kDragSpeed, 0, 100)) {
			newSegmentRenderer.width = width;
		}

		auto& editorMaterialManager = editor_->GetResourcesManagerContainer().editorMaterialsManager;
		std::string materialName = editorMaterialManager.GetMaterialName(newSegmentRenderer.materialID);

		const std::vector<std::string>& items = editorMaterialManager.GetMaterialDiffuseNames();

		if (ImGui::BeginCombo("Selected material", materialName.c_str())) {
			for (auto& item : items) {
				bool isSelected = materialName == item;
				if (ImGui::Selectable(item.c_str(), isSelected)) { materialName = item; }
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		newSegmentRenderer.materialID = math::HashString(materialName);

		if (ImGui::TreeNode("Material")) {
			auto material = editorMaterialManager.GetMaterialDiffuse(newSegmentRenderer.materialID);
			auto newMaterial = DisplayAndModifyMaterialDiffuse(material);
			if (material != newMaterial) {
				editorMaterialManager.SetMaterial(newMaterial, newSegmentRenderer.materialID);
			}
			ImGui::TreePop();
		}
	}

	return newSegmentRenderer;
}

// !Display components

// Component game
const game::Enemy ComponentUtility::DisplayAndModifyEnemy(
	const game::Enemy& enemy)
{
	game::Enemy newEnemy = enemy;

	if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::ENEMY).c_str())) {
		if (ImGui::TreeNode("Speed options")) {
			float approachSpeed = newEnemy.approachSpeed;
			if (ImGui::DragFloat("Approach speed", &approachSpeed, kDragSpeed)) {
				newEnemy.approachSpeed = approachSpeed;
			}

			float attackSpeed = newEnemy.attackSpeed;
			if (ImGui::DragFloat("Attack speed", &attackSpeed, kDragSpeed)) {
				newEnemy.attackSpeed = attackSpeed;
			}

			float fleeSpeed = newEnemy.fleeSpeed;
			if (ImGui::DragFloat("Flee speed", &fleeSpeed, kDragSpeed)) {
				newEnemy.fleeSpeed = fleeSpeed;
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Spline options")) {
			int splineFireStartPoint = newEnemy.splineFireStartPoint;
			if (ImGui::DragInt("Fire start point", &splineFireStartPoint, kDragSpeed)) {
				newEnemy.splineFireStartPoint = splineFireStartPoint;
			}

			int splineFireEndPoint = newEnemy.splineFireEndPoint;
			if (ImGui::DragInt("Fire end point", &splineFireEndPoint, kDragSpeed)) {
				newEnemy.splineFireEndPoint = splineFireEndPoint;
			}

			ImGui::TreePop();
		}

		std::vector<ecs::EntityIndex> weaponsIndexs = newEnemy.weapons;
		std::vector<ecs::EntityIndex> newWeaponsIndexs;

		for (int i = 0; i < weaponsIndexs.size(); i++) {
			ImGui::DragInt(("WeaponsIndex " + std::to_string(i)).c_str(), &weaponsIndexs[i], kDragSpeed);
			ImGui::SameLine();
			if (ImGui::Button(("X##deleteWeaponIndex" + std::to_string(i)).c_str())) {
				std::cout << i << "\n";
			}
			else
			{
				newWeaponsIndexs.push_back(weaponsIndexs[i]);
			}
		}

		if (ImGui::Button("Add weapon##Weapon")) {newWeaponsIndexs.push_back(ecs::EntityIndex{}); }

		newEnemy.weapons = newWeaponsIndexs;
		
	}
	
	return newEnemy;
}

const game::Player ComponentUtility::DisplayAndModifyPlayer(
	const game::Player& player)
{
	game::Player newPlayer = player;

	if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::PLAYER).c_str())) {

		int life = newPlayer.life;
		if (ImGui::DragInt("Life", &life, kDragSpeed)) {
			newPlayer.life = life;
		}

		float shieldLife = newPlayer.shieldLife;
		if (ImGui::DragFloat("Shield life", &shieldLife, kDragSpeed)) {
			newPlayer.shieldLife = shieldLife;
		}

		int score = newPlayer.score;
		if (ImGui::DragInt("Score", &score, kDragSpeed)) {
			newPlayer.score = score;
		}

		int destructionPoint = newPlayer.destructionPoint;
		if (ImGui::DragInt("Destruction point", &destructionPoint, kDragSpeed)) {
			newPlayer.destructionPoint = destructionPoint;
		}

		int missileAmount = newPlayer.missileAmount;
		if (ImGui::DragInt("Missile amount", &missileAmount, kDragSpeed)) {
			newPlayer.missileAmount = missileAmount;
		}

		float fireDirection[3] = {
			newPlayer.fireDirection.x,
			newPlayer.fireDirection.y,
			newPlayer.fireDirection.z
		};
		if (ImGui::DragFloat3("Fire direction", fireDirection, kDragSpeed)) {
			newPlayer.fireDirection.x = fireDirection[0];
			newPlayer.fireDirection.y = fireDirection[1];
			newPlayer.fireDirection.z = fireDirection[2];
		}

		if (ImGui::TreeNode("Dash options")) {
			float dashCooldown = newPlayer.dashCooldown;
			if (ImGui::DragFloat("Dash Cooldown", &dashCooldown, kDragSpeed)) {
				newPlayer.dashCooldown = dashCooldown;
			}

			float dashDuration = newPlayer.dashDuration;
			if (ImGui::DragFloat("Dash duration", &dashDuration, kDragSpeed)) {
				newPlayer.dashDuration = dashDuration;
			}

			float dashSpeed = newPlayer.dashSpeed;
			if (ImGui::DragFloat("Dash speed", &dashSpeed, kDragSpeed)) {
				newPlayer.dashSpeed = dashSpeed;
			}

			float dashStartedAt = newPlayer.dashStartedAt;
			if (ImGui::DragFloat("Dash started at", &dashStartedAt, kDragSpeed)) {
				newPlayer.dashStartedAt = dashStartedAt;
			}

			float lastDashEndedAt = newPlayer.lastDashEndedAt;
			if (ImGui::DragFloat("Last dash ended at", &lastDashEndedAt, kDragSpeed)) {
				newPlayer.lastDashEndedAt = lastDashEndedAt;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Move options")) {
			float maxMoveSpeed = newPlayer.maxMoveSpeed;
			if (ImGui::DragFloat("Max move speed", &maxMoveSpeed, kDragSpeed)) {
				newPlayer.maxMoveSpeed = maxMoveSpeed;
			}

			float moveSpeed = newPlayer.moveSpeed;
			if (ImGui::DragFloat("Move speed", &moveSpeed, kDragSpeed)) {
				newPlayer.moveSpeed = moveSpeed;
			}

			float moveDirection[2] = {
				newPlayer.moveDirection.x,
				newPlayer.moveDirection.y
			};
			if (ImGui::DragFloat2("Move direction", moveDirection, kDragSpeed)) {
				newPlayer.moveDirection.x = moveDirection[0];
				newPlayer.moveDirection.y = moveDirection[1];
			}
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Shield Variables")) {
			float shieldRegenSpeed = newPlayer.shieldRegenSpeed;
			if (ImGui::DragFloat("Shield regen speed", &shieldRegenSpeed, kDragSpeed)) {
				newPlayer.shieldRegenSpeed = shieldRegenSpeed;
			}

			float timeBeforeShieldRegen = newPlayer.timeBeforeShieldRegen;
			if (ImGui::DragFloat("Time before shield regen", &timeBeforeShieldRegen, kDragSpeed)) {
				newPlayer.timeBeforeShieldRegen = timeBeforeShieldRegen;
			}
			
			float maxShieldLife = newPlayer.maxShieldLife;
			if (ImGui::DragFloat("Max shield life", &maxShieldLife, kDragSpeed)) {
				newPlayer.maxShieldLife = maxShieldLife;
			}
			ImGui::TreePop();
		}

		{
			const game::Player::PlayerMode mode = newPlayer.mode;
			const char* items[] = { "Standard", "Battering Ram" };
			int itemCurrent = static_cast<int>(mode);

			if (ImGui::BeginCombo("Mode", items[itemCurrent])) {
				for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
					const bool isSelected = itemCurrent == i;

					if (ImGui::Selectable(items[i], isSelected)) {
						itemCurrent = i;
					}
					if (isSelected) { ImGui::SetItemDefaultFocus(); }
				}
				newPlayer.mode = static_cast<game::Player::PlayerMode>(itemCurrent);
				ImGui::EndCombo();
			}
		}
		{
			const game::Player::PlayerState state = newPlayer.state;
			const char* items[] = { "None", "Dashing" };
			int itemCurrent = static_cast<int>(state);
			if (ImGui::BeginCombo("State", items[itemCurrent])) {
				for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
					const bool isSelected = itemCurrent == i;
					if (ImGui::Selectable(items[i], isSelected)) {
						itemCurrent = i;
					}
					if (isSelected) { ImGui::SetItemDefaultFocus(); }
				}
				newPlayer.state = static_cast<game::Player::PlayerState>(itemCurrent);
				ImGui::EndCombo();
			}
		}
	}

	return newPlayer;
}

const game::DestructibleElement ComponentUtility::DisplayAndModifyDestructibleElement(
	const game::DestructibleElement& destructibleElement)
{
	game::DestructibleElement newDestructibleElement = destructibleElement;

	if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::DESTRUCTIBLE_ELEMENT).c_str())) {
		
		int life = newDestructibleElement.life;
		if (ImGui::DragInt("Life", &life, kDragSpeed)) {
			newDestructibleElement.life = life;
		}

		int scorePointGiven = newDestructibleElement.scorePointGiven;
		if (ImGui::DragInt("Score point given", &scorePointGiven, kDragSpeed)) {
			newDestructibleElement.scorePointGiven = scorePointGiven;
		}

		int destructionPointGiven = newDestructibleElement.destructionPointGiven;
		if (ImGui::DragInt("Destruction point given", &destructionPointGiven, kDragSpeed)) {
			newDestructibleElement.destructionPointGiven = destructionPointGiven;
		}

		{
			const game::DestructibleElement::Type type = newDestructibleElement.type;
			const char* items[] = { "Asteroid", "Ship", "Turret" };
			int itemCurrent = static_cast<int>(type);

			if (ImGui::BeginCombo("State", items[itemCurrent])) {
				for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
					const bool isSelected = itemCurrent == i;

					if (ImGui::Selectable(items[i], isSelected)) {
						itemCurrent = i;
					}
					if (isSelected) { ImGui::SetItemDefaultFocus(); }
				}
				newDestructibleElement.type = static_cast<game::DestructibleElement::Type>(itemCurrent);
				ImGui::EndCombo();
			}
		}

		bool isIndestructible = newDestructibleElement.isIndestructible;
		if(ImGui::Checkbox("Is indestructible", &isIndestructible)) {
			newDestructibleElement.isIndestructible = isIndestructible;
		}

	}

	return newDestructibleElement;
}

const game::Weapon ComponentUtility::DisplayAndModifyWeapon(
	const game::Weapon& weapon, bool treeNode)
{
	game::Weapon newWeapon = weapon;
	std::vector<ecs::EntityIndex> gunPositionsIndexes = newWeapon.gunPositions;
	std::vector<ecs::EntityIndex> newGunPositionsIndexes;

	if (treeNode ?
		ImGui::TreeNode(GetComponentName(ecs::ComponentType::WEAPON).c_str()) :
		ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::WEAPON).c_str())) {

		//TODO : Adapt to and entitiy index vector
		/*for (auto& transform : newWeapon.gunPositions) {
			transform = DisplayAndModifyTransform(transform);
		}*/

		float lastShootAt = newWeapon.lastShootAt;
		if (ImGui::DragFloat("Last shot at", &lastShootAt, kDragSpeed)) {
			newWeapon.lastShootAt = lastShootAt;
		}

		float shootCoolDown = newWeapon.shootCoolDown;
		if (ImGui::DragFloat("ShootCoolDown", &shootCoolDown, kDragSpeed)) {
			newWeapon.shootCoolDown = shootCoolDown;
		}

		const game::Weapon::WeaponType weaponType = newWeapon.weaponType;
		const char* items[] = {
			"Projectile",
			"Player projectile",
			"Missile",
			"Player missile",
			"Laser"
		};
		int itemCurrent = static_cast<int>(weaponType);

		if (ImGui::BeginCombo("Mode", items[itemCurrent])) {
			for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
				const bool isSelected = itemCurrent == i;

				if (ImGui::Selectable(items[i], isSelected)) {
					itemCurrent = i;
				}
				if (isSelected) { ImGui::SetItemDefaultFocus(); }
			}
			newWeapon.weaponType = static_cast<game::Weapon::WeaponType>(itemCurrent);
			ImGui::EndCombo();
		}

		if (treeNode) ImGui::TreePop();

		for (int i = 0; i < gunPositionsIndexes.size(); i++) {
			ImGui::DragInt(("GunPositionIndexes " + std::to_string(i)).c_str(), &gunPositionsIndexes[i], kDragSpeed);
			ImGui::SameLine();
			if (ImGui::Button(("X##deleteGunPositionsIndexes" + std::to_string(i)).c_str())) {
				std::cout << i << "\n";
			}
			else
			{
				newGunPositionsIndexes.push_back(gunPositionsIndexes[i]);
			}
		}

		if (ImGui::Button("Add gunPosition##Weapon")) { newGunPositionsIndexes.push_back(ecs::EntityIndex{}); }

		newWeapon.gunPositions = newGunPositionsIndexes;

	}

	return newWeapon;
}

const game::Projectile ComponentUtility::DisplayAndModifyProjectile(
	const game::Projectile& projectile, bool treeNode)
{
	game::Projectile newProjectile = projectile;

	if (treeNode ?
		ImGui::TreeNode(GetComponentName(ecs::ComponentType::PROJECTILE).c_str()) :
		ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::PROJECTILE).c_str())) {
		int damage = newProjectile.damage;
		if (ImGui::DragInt("Damage", &damage, kDragSpeed)) {
			newProjectile.damage = damage;
		}

		float durationLifeTime = newProjectile.durationLifeTime;
		if (ImGui::DragFloat("Duration life time", &durationLifeTime, kDragSpeed)) {
			newProjectile.durationLifeTime = durationLifeTime;
		}

		float moveSpeed = newProjectile.moveSpeed;
		if (ImGui::DragFloat("Move speed", &moveSpeed, kDragSpeed)) {
			newProjectile.moveSpeed = moveSpeed;
		}

		float shootAtTime = newProjectile.shootAtTime;
		if (ImGui::DragFloat("Shoot at time", &shootAtTime, kDragSpeed)) {
			newProjectile.shootAtTime = shootAtTime;
		}

		const ImVec2 padding{ 10, 10 };
		const ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));
		ImGui::BeginGroup();
		ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, "Infos");
		const  auto remainingTime = (projectile.shootAtTime + projectile.durationLifeTime) - Time::Get().GetTime();
		ImGui::TextColored(
			{ 0.5f, 0.5f, 0.5f, 1.0f },
			"Time remaining : %.2f [s]",
			remainingTime);
		ImGui::EndGroup();
		ImVec2 p1 = ImVec2(
			ImGui::GetItemRectMax().x + padding.x,
			ImGui::GetItemRectMax().y + padding.y);
		ImGui::Dummy(ImVec2(0.0f, padding.y));
		ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(120, 120, 120, 255), 10.0f);

		if (treeNode) ImGui::TreePop();
	}

	return newProjectile;
}

const game::Missile ComponentUtility::DisplayAndModifyMissile(
	const game::Missile& missile, bool treeNode)
{
	game::Missile newMissile = missile;

	if (treeNode ?
		ImGui::TreeNode(GetComponentName(ecs::ComponentType::MISSILE).c_str()) :
		ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::MISSILE).c_str())) {

		int damage = newMissile.damage;
		if (ImGui::DragInt("Damage", &damage, kDragSpeed)) {
			newMissile.damage = damage;
		}

		float lifeTime = newMissile.lifeTime;
		if (ImGui::DragFloat("LifeTime", &lifeTime, kDragSpeed)) {
			newMissile.lifeTime = lifeTime;
		}

		float currentLifeTime = newMissile.currentLifeTime;
		if (ImGui::DragFloat("Current life time", &currentLifeTime, kDragSpeed)) {
			newMissile.currentLifeTime = currentLifeTime;
		}

		float direction[3] = {
			newMissile.direction.x,
			newMissile.direction.y,
			newMissile.direction.z
		};
		if (ImGui::DragFloat3("Move direction", direction, kDragSpeed)) {
			newMissile.direction.x = direction[0];
			newMissile.direction.y = direction[1];
			newMissile.direction.z = direction[2];
		}

		float speed = newMissile.speed;
		if (ImGui::DragFloat("Speed", &speed, kDragSpeed)) {
			newMissile.speed = speed;
		}

		ecs::EntityIndex target = newMissile.target;
		if(ImGui::InputInt("Target", &target)) {
		    newMissile.target = target;
		}

		float shootDelay = newMissile.shootDelay;
		if (ImGui::DragFloat("Shoot delay", &shootDelay, kDragSpeed)) {
			newMissile.shootDelay = shootDelay;
		}

		bool hasBeenShot = newMissile.hasBeenShot;
        if(ImGui::Checkbox("Has been shot", &hasBeenShot)) {
			newMissile.hasBeenShot = hasBeenShot;
        }

		if (treeNode) ImGui::TreePop();
	}

	return newMissile;
}

const game::SpecialAttack ComponentUtility::DisplayAndModifySpecialAttack(
	const game::SpecialAttack & specialAttack)
{
	game::SpecialAttack newSpecialAttack = specialAttack;

	if (ImGui::CollapsingHeader(
		GetComponentName(ecs::ComponentType::SPECIAL_ATTACK).c_str())
		) {

		int expansionSpeed = newSpecialAttack.expansionSpeed;
		if (ImGui::DragInt("Expansion speed", &expansionSpeed, kDragSpeed)) {
			newSpecialAttack.expansionSpeed = expansionSpeed;
		}

		float maxRadius = newSpecialAttack.maxRadius;
		if (ImGui::DragFloat("Max Radius", &maxRadius, kDragSpeed)) {
			newSpecialAttack.maxRadius = maxRadius;
		}
	}

	return newSpecialAttack;
}

const game::Jiggle ComponentUtility::DisplayAndModifyJiggle(
    const game::Jiggle& jiggle) {
    game::Jiggle newJiggle = jiggle;

    if (ImGui::CollapsingHeader(
        GetComponentName(ecs::ComponentType::JIGGLE).c_str())
        ) {

        float speed = newJiggle.speed;
        if (ImGui::DragFloat("Speed", &speed, kDragSpeed, 0.1f, 50.0f)) {
            newJiggle.speed = speed;
        }

        float maxTime = newJiggle.maxTime;
        if (ImGui::DragFloat("Max Time", &maxTime, kDragSpeed, 0.1f, 50.0f)) {
            newJiggle.maxTime = maxTime;
        }

        float rotationSpeed = newJiggle.rotationSpeed;
        if (ImGui::DragFloat("Rotation Speed", &rotationSpeed, 0.01f)) {
            newJiggle.rotationSpeed = rotationSpeed;
        }
    }

    return newJiggle;
}

const game::GameCamera ComponentUtility::DisplayAndModifyGameCamera(
    const game::GameCamera& gameCamera) {

	game::GameCamera newGameCamera = gameCamera;

	if (ImGui::CollapsingHeader(
		GetComponentName(ecs::ComponentType::GAME_CAMERA).c_str())
		) {

		float playerOffset[3] = {
	        newGameCamera.playerOffset.x,
	        newGameCamera.playerOffset.y,
	        newGameCamera.playerOffset.z
		};
		if (ImGui::DragFloat3("Player offset", playerOffset, kDragSpeed, 0)) {
			newGameCamera.playerOffset = math::Vec3(playerOffset[0], playerOffset[1], playerOffset[2]);
		}
	}

	return newGameCamera;
}

const game::SplineStates ComponentUtility::DisplayandModifySplineStates(const game::SplineStates & splineStates){

	game::SplineStates newSplineStates = splineStates;


	if (ImGui::CollapsingHeader(GetComponentName(ecs::ComponentType::SPLINE_STATES).c_str())) {
	
		if (ImGui::Button("Add point##SplineStates")) { newSplineStates.pointStates.emplace_back(game::PointState{}); }
		

		for (size_t i = 0; i < newSplineStates.pointStates.size(); i++){

			auto rotation = newSplineStates.pointStates[i].rotation;

			if (ImGui::DragFloat3(("Rotation##Spline_state" + std::to_string(i)).c_str(), &rotation[0], kDragSpeed)) {
				newSplineStates.pointStates[i].rotation = rotation;
			}

			auto speed = newSplineStates.pointStates[i].moveSpeed;
			if (ImGui::DragFloat(("Speed##Spline_state" + std::to_string(i)).c_str(), &speed, kDragSpeed)) {
				newSplineStates.pointStates[i].moveSpeed = speed;
			}

			auto lerpSpeed = newSplineStates.pointStates[i].lerpSpeed;
			if (ImGui::DragFloat(("Lerp speed##Spline_state" + std::to_string(i)).c_str(), &lerpSpeed, kDragSpeed)) {
				newSplineStates.pointStates[i].lerpSpeed = lerpSpeed;
			}
			
			int newPointIndex = newSplineStates.pointStates[i].pointIndex;

			if (ImGui::InputInt(("Index##Spline_state" + std::to_string(i)).c_str(), &newPointIndex)) {
				newSplineStates.pointStates[i].pointIndex = newPointIndex;
			}
		}

	}
	return newSplineStates;
}

// !Component game
const void ComponentUtility::DisplayTransformEditor()
{
    if (ImGui::BeginMenu("Transform Editor")) {
        if (ImGui::RadioButton(
            "Translate",
            mCurrentGizmoOperation_ == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation_ = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton(
            "Rotate",
            mCurrentGizmoOperation_ == ImGuizmo::ROTATE))
            mCurrentGizmoOperation_ = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation_ == ImGuizmo::SCALE))
            mCurrentGizmoOperation_ = ImGuizmo::SCALE;

        if (mCurrentGizmoOperation_ != ImGuizmo::SCALE) {
            ImGui::SameLine();
            if (ImGui::RadioButton("Local", mCurrentGizmoMode_ == ImGuizmo::LOCAL))
                mCurrentGizmoMode_ = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode_ == ImGuizmo::WORLD))
                mCurrentGizmoMode_ = ImGuizmo::WORLD;
        }

        if (ImGui::TreeNodeEx("Snap")) {
            ImGui::Checkbox("Use Snap", &useSnap_);
            ImGui::SameLine();

            switch (mCurrentGizmoOperation_) {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", snap_);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", snap_);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", snap_);
                break;
            default: ;
            }
            if (boundSizing_) {
                ImGui::Checkbox("Use Snap", &useSnap_);
                ImGui::SameLine();
                ImGui::InputFloat3("Bounds Snap", boundsSnap_);
            }
            ImGui::TreePop();
        }
        ImGui::EndMenu();
    }
}

const math::Transform ComponentUtility::DisplayTransformEditorGizmo(
	const math::Transform& transform) {
	math::Transform newTransform = transform;

	ImGuizmo::BeginFrame();
	auto& camera = CameraLocator::Get();

	float matrixTranslation[3]{
		newTransform.GetLocalPosition().x,
		newTransform.GetLocalPosition().y,
		newTransform.GetLocalPosition().z
	};
	float matrixRotation[3]{
		newTransform.GetLocalRotation().x * math::kRad2Deg,
		newTransform.GetLocalRotation().y * math::kRad2Deg,
		newTransform.GetLocalRotation().z * math::kRad2Deg
	};
	float matrixScale[3]{
		newTransform.GetLocalScale().x,
		newTransform.GetLocalScale().y,
		newTransform.GetLocalScale().z
	};

	float localObjectMatrix[16];

	ImGuizmo::RecomposeMatrixFromComponents(
		matrixTranslation,
		matrixRotation,
		matrixScale,
		localObjectMatrix);

	float cameraView[16] =
	{
		camera.GetViewMatrix()[0].x,
		camera.GetViewMatrix()[0].y,
		camera.GetViewMatrix()[0].z,
		camera.GetViewMatrix()[0].w,
		camera.GetViewMatrix()[1].x,
		camera.GetViewMatrix()[1].y,
		camera.GetViewMatrix()[1].z,
		camera.GetViewMatrix()[1].w,
		camera.GetViewMatrix()[2].x,
		camera.GetViewMatrix()[2].y,
		camera.GetViewMatrix()[2].z,
		camera.GetViewMatrix()[2].w,
		camera.GetViewMatrix()[3].x,
		camera.GetViewMatrix()[3].y,
		camera.GetViewMatrix()[3].z,
		camera.GetViewMatrix()[3].w
	};

	float cameraProjection[16] =
	{
		camera.GetProjectionMatrix()[0].x,
		camera.GetProjectionMatrix()[0].y,
		camera.GetProjectionMatrix()[0].z,
		camera.GetProjectionMatrix()[0].w,
		camera.GetProjectionMatrix()[1].x,
		camera.GetProjectionMatrix()[1].y,
		camera.GetProjectionMatrix()[1].z,
		camera.GetProjectionMatrix()[1].w,
		camera.GetProjectionMatrix()[2].x,
		camera.GetProjectionMatrix()[2].y,
		camera.GetProjectionMatrix()[2].z,
		camera.GetProjectionMatrix()[2].w,
		camera.GetProjectionMatrix()[3].x,
		camera.GetProjectionMatrix()[3].y,
		camera.GetProjectionMatrix()[3].z,
		camera.GetProjectionMatrix()[3].w
	};

	const ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(
		cameraView,
		cameraProjection,
		mCurrentGizmoOperation_,
		mCurrentGizmoMode_,
		localObjectMatrix,
		nullptr,
		useSnap_ ? snap_ : nullptr,
		boundSizing_ ? bounds_ : nullptr,
		useSnap_ ? boundsSnap_ : nullptr);

	ImGuizmo::DecomposeMatrixToComponents(
		localObjectMatrix,
		matrixTranslation,
		matrixRotation,
		matrixScale);

	newTransform.SetLocalPosition(
		{
			matrixTranslation[0],
			matrixTranslation[1],
			matrixTranslation[2]
		});
	newTransform.SetLocalRotation(
		{
			matrixRotation[0] * math::kDeg2Rad,
			matrixRotation[1] * math::kDeg2Rad,
			matrixRotation[2] * math::kDeg2Rad
		});
	newTransform.SetLocalScale(
		{
			matrixScale[0],
			matrixScale[1],
			matrixScale[2]
		});
	return newTransform;
}

void ComponentUtility::DisplayRemoveComponentButton(
    const ecs::ComponentType::ComponentType componentType) {

	const std::string componentName = ComponentUtility::GetComponentName(componentType);
	if (ImGui::BeginPopupContextItem(componentName.c_str())) {
		if (ImGui::Button("Delete Component")) {
			editor_->GetEditorEcsManager().RemoveComponent(
				editor_->GetSelectedEntityIndex(),
				componentType);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

XXH64_hash_t ComponentUtility::CreateNewMaterial(
	const std::string& newMaterialName,
    const graphics::MaterialType materialType)
{
	json newMaterial = json();
	newMaterial["blend"] = 1.0f;
	newMaterial["materialType"] = 0;
	newMaterial["metallic"] = 0.0f;
	newMaterial["roughness"] = 0.0f;
    if(materialType == graphics::MaterialType::SKYBOX) {
		newMaterial["texture"] = "default";
		newMaterial["colorDiffuse"]["a"] = 1.0f;
		newMaterial["colorDiffuse"]["r"] = 97.0f / 255.0f;
		newMaterial["colorDiffuse"]["g"] = 95.0f / 255.0f;
		newMaterial["colorDiffuse"]["b"] = 87.0f / 255.0f;
    } else {
		newMaterial["textureDiffuse"] = "default_diffuse.jpg";
		newMaterial["color"]["a"] = 1.0f;
		newMaterial["color"]["r"] = 97.0f / 255.0f;
		newMaterial["color"]["g"] = 95.0f / 255.0f;
		newMaterial["color"]["b"] = 87.0f / 255.0f;
		newMaterial["renderMode"] = 0;
        
    }
	newMaterial["ignoreLighting"] = false;
	newMaterial["ignoreFog"] = false;

	PokFileSystem::WriteFile(
		newMaterialName,
		newMaterial,
		FileType::MATERIAL,
		FolderType::SAVE_IN_ROM);

	editor_->GetResourcesManagerContainer()
		.editorMaterialsManager
		.AddMaterial(newMaterialName);

	return XXH64(
		newMaterialName.c_str(),
		newMaterialName.size(),
		math::kHashSeed);
}

///////////////////////////////////////////////////////////

std::string ComponentUtility::GetComponentName(
    const ecs::ComponentType::ComponentType componentType,
		const bool inJson)
{
    switch (componentType) {
    case ecs::ComponentType::TRANSFORM:
	    return inJson ? "transform" : "Transform";
    case ecs::ComponentType::RIGIDBODY:
	    return inJson ? "rigidbody" : "Rigidbody";
    case ecs::ComponentType::COLLIDER:
	    return inJson ? "collider" : "Collider";
    case ecs::ComponentType::SPLINE_FOLLOWER:
	    return inJson ? "splineFollower" : "Spline Follower";
    case ecs::ComponentType::MODEL:
	    return inJson ? "model" : "Model";
    case ecs::ComponentType::LIGHT:
	    return inJson ? "light" : "Light";
    case ecs::ComponentType::PARTICLE_SYSTEM:
	    return inJson ? "particleSystem" : "Particle System";
    case ecs::ComponentType::AUDIO_SOURCE:
	    return inJson ? "audioSource" : "Audio Source";
	case ecs::ComponentType::EDITOR_COMPONENT:
		return inJson ? "editorComponent" : "Editor Component";
	case ecs::ComponentType::TRAIL_RENDERER:
		return inJson ? "trailRenderer" : "Trail Renderer";
	case ecs::ComponentType::SEGMENT_RENDERER:
		return inJson ? "segmentRenderer" : "Segment Renderer";
    case ecs::ComponentType::ENEMY:
	    return inJson ? "enemy" : "Enemy";
    case ecs::ComponentType::PLAYER:
	    return inJson ? "player" : "Player";
    case ecs::ComponentType::DESTRUCTIBLE_ELEMENT:
	    return inJson ? "destructibleElement" : "Destructible element";
    case ecs::ComponentType::WEAPON:
	    return inJson ? "weapon" : "Weapon";
    case ecs::ComponentType::PROJECTILE:
	    return inJson ? "projectile" : "Projectile";
    case ecs::ComponentType::MISSILE:
	    return inJson ? "missile" : "Missile";
	case ecs::ComponentType::SPECIAL_ATTACK:
		return inJson ? "specialAttack" : "Special attack";
    case ecs::ComponentType::JIGGLE:
        return inJson ? "jiggle" : "Jiggle";
	case ecs::ComponentType::SPLINE_STATES:
		return inJson ? "splineStates" : "Spline States";
	case ecs::ComponentType::GAME_CAMERA:
		return inJson ? "gameCamera" : "Game Camera";
    default:
	    return inJson ? "missingName" : "Missing Name";
    }
}
}