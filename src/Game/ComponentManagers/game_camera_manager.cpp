#include <Game/Ecs/game_archetype.h>
#include <Game/ComponentManagers/game_camera_manager.h>

namespace poke::game {
void GameCameraManager::ResizeEntities(const std::size_t size) {
	gameCameras_.resize(size);
}

void GameCameraManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	gameCameras_[entityIndex] = GameCamera();
}

GameCamera GameCameraManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return gameCameras_[entityIndex];
}

void GameCameraManager::SetWithArchetype(const ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		gameCameras_[entityIndex] = gameArchetype.gameCamera;
	}
}
	
void GameCameraManager::SetComponent(const ecs::EntityIndex entityIndex, const GameCamera& gameCamera) {
	gameCameras_[entityIndex] = gameCamera;
}

void GameCameraManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	GameCamera gameCamera;
	gameCamera.SetFromJson(componentJson);
	gameCameras_[entityIndex] = gameCamera;
}

json GameCameraManager::GetJsonFromComponent(ecs::EntityIndex entityIndex) {
	return gameCameras_[entityIndex].ToJson();
}

void GameCameraManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase) {
	gameCameras_.erase(gameCameras_.begin() + pool.firstEntity, gameCameras_.begin() + pool.firstEntity + nbObjectToErase);
}

void GameCameraManager::InsertArchetype(
	const ecs::EntityIndex entity,
	const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);
	gameCameras_.insert(gameCameras_.begin() + entity, gameArchetype.gameCamera);
}
}
