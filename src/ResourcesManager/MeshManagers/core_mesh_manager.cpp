#include <ResourcesManager/MeshManagers/core_mesh_manager.h>

#include <CoreEngine/cassert.h>
#include <CoreEngine/engine.h>
#include <Utility/log.h>
#include <json.hpp>

namespace poke {
CoreMeshManager::CoreMeshManager()
    : cubePrimitive_(false),
      spherePrimitive_(false, 0.5f),
      planePrimitive_(false),
      quadPrimitive_(false),
      lineGizmoPrimitive_(false),
      cubeGizmoPrimitive_(false),
      sphereGizmoPrimitive_(false)
{
    meshes_.reserve(kMeshObjDefaultSize);
    meshIDs_.reserve(kMeshObjDefaultSize);

	dynamicMeshes_.resize(1000);
	dynamicMeshIDs_.resize(1000);
}

void CoreMeshManager::Init()
{
    cubePrimitive_.Load();

    spherePrimitive_.Load();

    planePrimitive_.Load();

    quadPrimitive_.Load();

    lineGizmoPrimitive_.Load();

    cubeGizmoPrimitive_.Load();

    sphereGizmoPrimitive_.Load();
}

void CoreMeshManager::AddMesh(const std::string& name)
{
    const auto id = math::HashString(name);

    meshIDs_.emplace_back(id);

    meshes_.emplace_back(MeshObj());
	reinterpret_cast<MeshObj&>(meshes_.back()).Load(name);
}

DynamicMeshIndex CoreMeshManager::CreateDynamicMesh()
{
    //Set dynamicMeshIDs
    
    for (size_t i = 0; i < dynamicMeshes_.size(); i++) {
        if (dynamicMeshIDs_[i] == 0) {
			dynamicMeshIDs_[i] = math::HashString("DynamicMesh" + std::to_string(i));
			return i;
        }
    }

	std::vector<std::vector<graphics::VertexMesh>> vertices(dynamicMeshes_.size());
	std::vector<std::vector<uint32_t>> indices(dynamicMeshes_.size());

	for (size_t i = 0; i < dynamicMeshes_.size(); i++) {
		vertices[i] = dynamicMeshes_[i].GetVertices<graphics::VertexMesh>(0);
		indices[i] = dynamicMeshes_[i].GetIndices(0);
	}

	dynamicMeshes_.resize(dynamicMeshes_.size() * 2 + 1);
	dynamicMeshIDs_.resize(dynamicMeshes_.size());
	for (size_t i = 0; i < dynamicMeshes_.size(); i++) {
		dynamicMeshes_[i].Initialize(vertices[i], indices[i]);
	}
	

	for (size_t i = 0; i < dynamicMeshes_.size(); i++) {
		if (dynamicMeshIDs_[i] == 0) {
			dynamicMeshIDs_[i] = math::HashString("DynamicMesh" + std::to_string(i));
			return i;
		}
	}

	return 0;
}

ResourceID CoreMeshManager::GetDynamicMeshResourceID(const DynamicMeshIndex dynamicMeshIndex)
{
    return dynamicMeshIDs_[dynamicMeshIndex];
}

void CoreMeshManager::UpdateDynamicMesh(const DynamicMeshIndex dynamicMeshIndex, const std::vector<graphics::VertexMesh>& vertices, const std::vector<uint32_t>& indexes)
{
	dynamicMeshes_[dynamicMeshIndex].Initialize(vertices, indexes);
}

void CoreMeshManager::DestroyDynamicMesh(const DynamicMeshIndex dynamicMeshIndex)
{
	const std::vector<graphics::VertexMesh> vertices{
	    graphics::VertexMesh(math::Vec3(0, 0, 0), math::Vec2(0, 0), math::Vec3(0, 0, 0)),
	    graphics::VertexMesh(math::Vec3(0, 0, 0), math::Vec2(0, 0), math::Vec3(0, 0, 0)),
	    graphics::VertexMesh(math::Vec3(0, 0, 0), math::Vec2(0, 0), math::Vec3(0, 0, 0)),
	    graphics::VertexMesh(math::Vec3(0, 0, 0), math::Vec2(0, 0), math::Vec3(0, 0, 0))
	};
	const std::vector<uint32_t> indexes{
		0, 1, 2, 3, 2, 1
	};
	dynamicMeshes_[dynamicMeshIndex].Initialize(vertices, indexes);
	dynamicMeshIDs_[dynamicMeshIndex] = 0;
}

graphics::Mesh& CoreMeshManager::GetMesh(const std::string& name)
{
    const auto id = math::HashString(name);

    return GetMesh(id);
}

graphics::Mesh& CoreMeshManager::GetMesh(const ResourceID resourceID)
{
    //Look through all mesh
    for (size_t i = 0; i < meshIDs_.size(); i++) {
        if (meshIDs_[i] == resourceID) { return meshes_[i]; }
    }

	//Look through all dynamic mesh
	for (size_t i = 0; i < dynamicMeshIDs_.size(); i++) {
		if (dynamicMeshIDs_[i] == resourceID) {
		    return dynamicMeshes_[i];
		}
	}

    //Check if it's a primitive
	if (resourceID == math::HashString("Cube")) {
		return cubePrimitive_;
	}
	if (resourceID == math::HashString("CubeGizmo")) {
		return cubeGizmoPrimitive_;
	}
	if (resourceID == math::HashString("LineGizmo")) {
		return lineGizmoPrimitive_;
	}
	if (resourceID == math::HashString("Plane")) {
		return planePrimitive_;
	}
	if (resourceID == math::HashString("Quad")) {
		return quadPrimitive_;
	}
	if (resourceID == math::HashString("Sphere")) {
		return spherePrimitive_;
	}
	if (resourceID == math::HashString("SphereGizmo")) {
		return sphereGizmoPrimitive_;
	}
	return spherePrimitive_;
    cassert(false, "The mesh doesn't exist");
}

graphics::Mesh& CoreMeshManager::GetCube() { return cubePrimitive_; }

graphics::Mesh& CoreMeshManager::GetPlane() { return planePrimitive_; }

graphics::Mesh& CoreMeshManager::GetQuad() { return quadPrimitive_; }

graphics::Mesh& CoreMeshManager::GetLineGizmo() { return lineGizmoPrimitive_; }

graphics::Mesh& CoreMeshManager::GetCubeGizmo() { return cubeGizmoPrimitive_; }

graphics::Mesh& CoreMeshManager::GetSphereGizmo()
{
    return sphereGizmoPrimitive_;
}

ResourceID CoreMeshManager::GetCubeID()
{
	//tenter avec string
	return math::HashString("Cube");
}

ResourceID CoreMeshManager::GetLineID()
{
	return math::HashString("LineGizmo");
}

ResourceID CoreMeshManager::GetQuadID()
{
	return math::HashString("Quad");
}

ResourceID CoreMeshManager::GetSphereGizmoID()
{
	return math::HashString("SphereGizmo");
}

void CoreMeshManager::Resize(const size_t newSize)
{
    if (meshIDs_.size() > newSize) {
        meshIDs_.resize(newSize);
        meshes_.resize(newSize);
    } else {
        meshIDs_.reserve(newSize);
        meshes_.reserve(newSize);
    }
}

void CoreMeshManager::SetFromJson(const json& meshesJson)
{
	for (const auto& meshJson : meshesJson) {
		std::string meshName = meshJson;

		AddMesh(meshName);
	}
}

json CoreMeshManager::ToJson()
{
	LogWarning("You're trying to save using the CoreMeshManager, nothing will be saved.");
	return json();
}

void CoreMeshManager::Clear()
{
    meshIDs_.clear();
    meshes_.clear();

    //TODO(@Nico) Free primitive if the core mesh manager from the engine is not used.
}

ResourceID CoreMeshManager::GetCubeGizmoID()
{
	return math::HashString("CubeGizmo");
}

ResourceID CoreMeshManager::GetPlaneID()
{
	return math::HashString("Plane");
}

ResourceID CoreMeshManager::GetSphereID()
{
	return math::HashString("Sphere");
}

graphics::Mesh& CoreMeshManager::GetSphere() { return spherePrimitive_; }
} // namespace poke
