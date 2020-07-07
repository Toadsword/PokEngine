#include <Ecs/ComponentManagers/models_manager.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <GraphicsEngine/Models/material.h>

namespace poke {
namespace ecs {

void ModelsManager::ResizeEntities(const std::size_t size)
{
	models_.resize(size, graphics::Model(MaterialsManagerLocator::Get().GetDefaultMaterialID(), MeshManagerLocator::Get().GetSphereID()));
	MaterialsManagerLocator::Get().GetMaterial(MaterialsManagerLocator::Get().GetDefaultMaterialID()).
		CreatePipeline(
			GetVertexInput());
}

void ModelsManager::ClearEntity(const EntityIndex entityIndex)
{
    models_[entityIndex].materialID = MaterialsManagerLocator::Get().GetDefaultMaterialID();
    models_[entityIndex].meshID = MeshManagerLocator::Get().GetSphereID();
}

void ModelsManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (EntityIndex entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity; entityIndex++) {
        models_[entityIndex].materialID = archetype.model.materialID;

		if (archetype.model.materialID != 0) {

			MaterialsManagerLocator::Get().GetMaterial(archetype.model.materialID).
				CreatePipeline(
					GetVertexInput());
		}

        models_[entityIndex].meshID = archetype.model.meshID;
    }
}

void ModelsManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
	models_.insert(models_.begin() + entity, graphics::Model());
	models_[entity].materialID = archetype.model.materialID;
	models_[entity].meshID = archetype.model.meshID;
}

void ModelsManager::EraseEntities(const EntityPool pool, const size_t nbObjectToErase)
{
	models_.erase(models_.begin() + pool.firstEntity, models_.begin() + pool.firstEntity + nbObjectToErase);
}

void ModelsManager::SetComponent(
    const EntityIndex entityIndex,
    const graphics::Model& model)
{
    models_[entityIndex].materialID = model.materialID;
    models_[entityIndex].meshID = model.meshID;
}

void ModelsManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
	models_[entityIndex].SetFromJson(componentJson);
}

json ModelsManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return models_[entityIndex].ToJson();
}

graphics::Model ModelsManager::GetComponent(const EntityIndex entityIndex)
{
    return models_[entityIndex];
}

graphics::VertexInput ModelsManager::GetVertexInput(const uint32_t binding)
{
    return graphics::VertexMesh::GetVertexInput(binding);
}
} //namespace ecs
} //namespace poke
