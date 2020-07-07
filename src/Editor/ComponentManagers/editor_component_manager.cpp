#include <Editor/ComponentManagers/editor_component_manager.h>

namespace poke::ecs {
void EditorComponentManager::ResizeEntities(const std::size_t size) {
	editorComponents_.resize(size);
}

void EditorComponentManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	editorComponents_[entityIndex] = EditorComponent();
}

EditorComponent EditorComponentManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return editorComponents_[entityIndex];
}

void EditorComponentManager::SetComponent(
	const ecs::EntityIndex entityIndex, 
    const EditorComponent& editorComponent) {
    
	editorComponents_[entityIndex].name = editorComponent.name;
	editorComponents_[entityIndex].prefabName = editorComponent.prefabName;
}

void EditorComponentManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	EditorComponent editorComponent;
	editorComponent.SetFromJson(componentJson);
	SetComponent(entityIndex, editorComponent);
}

json EditorComponentManager::GetJsonFromComponent(
    ecs::EntityIndex entityIndex) {
	return editorComponents_[entityIndex].ToJson();
}

void EditorComponentManager::SetWithArchetype(
	const ecs::EntityPool entityPool,
	const ecs::Archetype& archetype)
{
	for (EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity;
		entityIndex++) {
		editorComponents_[entityIndex] = archetype.editorComponent;
	}
}

void EditorComponentManager::InsertArchetype(
	const ecs::EntityIndex entity,
	const ecs::Archetype& archetype)
{
}

void EditorComponentManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase)
{
	editorComponents_.erase(
		editorComponents_.begin() + pool.firstEntity, 
		editorComponents_.begin() + pool.firstEntity + nbObjectToErase);
}
}// namespace poke::ecs
