#include <ResourcesManager/MaterialsManager/core_materials_manager.h>

#include <GraphicsEngine/Models/material.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/log.h>

namespace poke {

CoreMaterialsManager::CoreMaterialsManager()
{
    skyboxMaterialIDs_.reserve(kMaterialsSkyboxDefaultSize);
    skyboxMaterials_.reserve(kMaterialsSkyboxDefaultSize);

    diffuseMaterials_.reserve(kMaterialsDefaultSize);
    diffuseMaterialIDs_.reserve(kMaterialsDefaultSize);

    trailMaterials_.reserve(kMaterialsDefaultSize);
    trailMaterialIDs_.reserve(kMaterialsDefaultSize);

	particleMaterials_.reserve(kMaterialsDefaultSize);
	particleMaterialIDs_.reserve(kMaterialsDefaultSize);
}

void CoreMaterialsManager::AddMaterial(const std::string& materialName)
{
    auto materialJson = LoadJson(materialName, FileType::MATERIAL);

    const graphics::MaterialType materialType = materialJson["materialType"];

    switch (materialType) {
    case graphics::MaterialType::SKYBOX: {
        skyboxMaterials_.emplace_back(MaterialSkybox());

        TextureManagerLocator::Get().AddTextureCube(materialJson["texture"].get<std::string>());

        skyboxMaterials_.back().SetFromJson(materialJson);

        skyboxMaterialIDs_.emplace_back(math::HashString(materialName));

        skyboxMaterials_.back().CreatePipeline(graphics::VertexMesh::GetVertexInput(0));
    }
    break;
    case graphics::MaterialType::DIFFUSE: {
        diffuseMaterials_.emplace_back(MaterialDiffuse());

        //TODO(@Nico) Remove it
        if (CheckJsonExists(materialJson, "textureBaseColor")) {
            TextureManagerLocator::Get().AddTexture2D(
                materialJson["textureBaseColor"].get<std::string>());
        }
        if (CheckJsonExists(materialJson, "textureNormal")) {
            TextureManagerLocator::Get().AddTexture2D(
                materialJson["textureNormal"].get<std::string>());
        }
        if (CheckJsonExists(materialJson, "textureRMA")) {
            TextureManagerLocator::Get().AddTexture2D(
                materialJson["textureRMA"].get<std::string>());
        }
        diffuseMaterials_.back().SetFromJson(materialJson);

        diffuseMaterialIDs_.emplace_back(math::HashString(materialName));

        diffuseMaterials_.back().CreatePipeline(graphics::VertexMesh::GetVertexInput(0));
    }
    break;
    case graphics::MaterialType::TRAIL: {
        trailMaterials_.emplace_back(MaterialTrail());

        //TODO(@Nico) Remove it
        if (CheckJsonExists(materialJson, "textureBaseColor")) {
            TextureManagerLocator::Get().AddTexture2D(
                materialJson["textureBaseColor"].get<std::string>());
        }
        trailMaterials_.back().SetFromJson(materialJson);

        trailMaterialIDs_.emplace_back(math::HashString(materialName));

        trailMaterials_.back().CreatePipeline(graphics::VertexMesh::GetVertexInput(0));
    }
    break;
	case graphics::MaterialType::PARTICLE: {
		particleMaterials_.emplace_back(MaterialParticle());

		//TODO(@Nico) Remove it
		if (CheckJsonExists(materialJson, "textureBaseColor")) {
			TextureManagerLocator::Get().AddTexture2D(
				materialJson["textureBaseColor"].get<std::string>());
		}
		particleMaterials_.back().SetFromJson(materialJson);

		particleMaterialIDs_.emplace_back(math::HashString(materialName));

		particleMaterials_.back().CreatePipeline(graphics::VertexMesh::GetVertexInput(0));
	}
	break;
    default: ;
    }
}

graphics::Material& CoreMaterialsManager::GetMaterial(const std::string& materialName)
{
    return GetMaterial(math::HashString(materialName));
}

graphics::Material& CoreMaterialsManager::GetMaterial(
    const ResourceID resourceID)
{
    //Diffuse materials
    for (size_t i = 0; i < diffuseMaterialIDs_.size(); i++) {
        if (diffuseMaterialIDs_[i] == resourceID) { return diffuseMaterials_[i]; }
    }

    //Particles materials
	for (size_t i = 0; i < particleMaterialIDs_.size(); i++) {
		if (particleMaterialIDs_[i] == resourceID) { return particleMaterials_[i]; }
	}

    //Trails materials
    for (size_t i = 0; i < trailMaterialIDs_.size(); i++) {
        if (trailMaterialIDs_[i] == resourceID) { return trailMaterials_[i]; }
    }

    //Skybox materials
    for (size_t i = 0; i < skyboxMaterialIDs_.size(); i++) {
        if (skyboxMaterialIDs_[i] == resourceID) { return skyboxMaterials_[i]; }
    }

    cassert(
        false,
        "Impossible to get " << resourceID <<
        " material, should be loaded before being accessed");
}

void CoreMaterialsManager::Clear()
{
    skyboxMaterialIDs_.clear();
    skyboxMaterials_.clear();

    diffuseMaterials_.clear();
    diffuseMaterialIDs_.clear();

    trailMaterialIDs_.clear();
    trailMaterials_.clear();

	particleMaterials_.clear();
	particleMaterialIDs_.clear();

    defaultMaterialIDLoaded_ = false;
    defaultMaterialSkyboxIDLoaded_ = false;
}

void CoreMaterialsManager::Resize(const size_t newSize)
{
    //Doesn't resize skybox material
    if (diffuseMaterials_.size() > newSize) {
        diffuseMaterials_.resize(newSize);
        diffuseMaterialIDs_.resize(newSize);

        trailMaterials_.resize(newSize);
        trailMaterialIDs_.resize(newSize);

		particleMaterials_.resize(newSize);
		particleMaterialIDs_.resize(newSize);
    } else {
        trailMaterials_.reserve(newSize);
        trailMaterialIDs_.reserve(newSize);

        diffuseMaterials_.reserve(newSize);
        diffuseMaterialIDs_.reserve(newSize);

		particleMaterials_.reserve(newSize);
		particleMaterialIDs_.reserve(newSize);
    }
}

ResourceID CoreMaterialsManager::GetDefaultMaterialID()
{
    if (!defaultMaterialIDLoaded_) {
        diffuseMaterials_.emplace_back(MaterialDiffuse());

        diffuseMaterialIDs_.emplace_back(math::HashString(kDefaultMaterialName));

        diffuseMaterials_.back().CreatePipeline(graphics::VertexMesh::GetVertexInput(0));

        defaultMaterialIDLoaded_ = true;
    }

    return math::HashString(kDefaultMaterialName);
}

ResourceID CoreMaterialsManager::GetDefaultSkyboxMaterialID()
{
    if (!defaultMaterialSkyboxIDLoaded_) {
        defaultImageCube_.Load();

        skyboxMaterials_.emplace_back(MaterialSkybox());
        skyboxMaterials_.back().SetTexture(defaultImageCube_);

        skyboxMaterialIDs_.emplace_back(math::HashString(kDefaultSkyboxMaterialName));

        skyboxMaterials_.back().CreatePipeline(graphics::VertexMesh::GetVertexInput(0));

        defaultMaterialSkyboxIDLoaded_ = true;
    }

    return math::HashString(kDefaultSkyboxMaterialName);
}

void CoreMaterialsManager::SetFromJson(const json& materialsJson)
{
    for (const auto& materialJson : materialsJson) 
    {
        std::string materialsName = materialJson;

        AddMaterial(materialsName);
    }
}

json CoreMaterialsManager::ToJson()
{
    LogWarning("You're trying to save using the CoreMeshManager, nothing will be saved.");
    return json();
}
} //namespace poke
