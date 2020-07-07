#include <Game/ResourcesManager/PrefabsManager/game_prefabs_manager.h>

#include <Utility/log.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Ecs/ComponentManagers/trail_renderer_manager.h>
#include "Game/ComponentManagers/enemies_manager.h"

namespace poke {
namespace game {

GamePrefabsManager::GamePrefabsManager() {}

std::vector<ecs::EntityIndex> GamePrefabsManager::InstantiatePlayer() const
{
	std::vector<ecs::EntityIndex> playerIndexes = Instantiate("Player2");

	PlayerManager& playerManager_ = EcsManagerLocator::Get().GetComponentsManager<PlayerManager>();
	WeaponManager& weaponManager_ = EcsManagerLocator::Get().GetComponentsManager<WeaponManager>();
	ecs::TransformsManager& transformsManager = EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>();
	
	// Assign gun position to the projectile weapon
	Weapon projectileWeapon = weaponManager_.GetComponent(playerIndexes[3]);
	projectileWeapon.gunPositions.resize(2);
	projectileWeapon.gunPositions[0] = playerIndexes[4];
	projectileWeapon.gunPositions[1] = playerIndexes[5];
	weaponManager_.SetComponent(playerIndexes[3], projectileWeapon);
	
	// Assign elements to the player
	Player player = playerManager_.GetComponent(playerIndexes[1]);
	player.missileWeapon = playerIndexes[6];
	player.projectileWeapon = playerIndexes[3];
	//player.aimIndex = playerIndexes[18];
	//player.lockedAimIndex = playerIndexes[19];
	player.lockedAimBaseTransform = transformsManager.GetComponent(playerIndexes[19]);
	player.cameraObjectIndex = playerIndexes[0];
	player.damageAreas.push_back(playerIndexes[12]);
	player.damageAreas.push_back(playerIndexes[13]);
	player.damageAreas.push_back(playerIndexes[14]);
	player.damageAreas.push_back(playerIndexes[15]);
	player.shieldIndex = playerIndexes[21];
	player.specialAttackIndex = playerIndexes[23];

	player.dashSoundIndex = playerIndexes[26];
	player.hitSoundIndex = playerIndexes[28];

	player.targetSightPositionOne = playerIndexes[18];
	player.targetSightPositionTwo = playerIndexes[19];
	player.targetSightPositionThree = playerIndexes[20];
	player.targetSightModelOne = playerIndexes[29];
	player.targetSightModelTwo = playerIndexes[30];
	player.targetSightModelThree = playerIndexes[31];

    //TODO (@Build and save) When you don't want to make a new save or build, you can uncomment this part.
	//transformsManager.SetParent(playerIndexes[29], ecs::kNoEntity);
	//transformsManager.SetParent(playerIndexes[30], ecs::kNoEntity);
	//transformsManager.SetParent(playerIndexes[31], ecs::kNoEntity);
	
	playerManager_.SetComponent(playerIndexes[1], player);
	
	// Assign gun position to the missile weapon
	Weapon missileWeapon = weaponManager_.GetComponent(playerIndexes[6]);
	missileWeapon.gunPositions.resize(4);
	missileWeapon.gunPositions[0] = playerIndexes[7];
	missileWeapon.gunPositions[1] = playerIndexes[8];
	missileWeapon.gunPositions[2] = playerIndexes[9];
	missileWeapon.gunPositions[3] = playerIndexes[10];
	weaponManager_.SetComponent(playerIndexes[6], missileWeapon);

	return playerIndexes;
}

std::vector<ecs::EntityIndex> GamePrefabsManager::InstantiateLightFighter() const {
	std::vector<ecs::EntityIndex> lightFighterIndexes = Instantiate("LightFighter");

	EnemiesManager& enemiesManager_ = EcsManagerLocator::Get().GetComponentsManager<EnemiesManager>();
	WeaponManager& weaponManager_ = EcsManagerLocator::Get().GetComponentsManager<WeaponManager>();

	// Assign weapon to the enemy
	Enemy enemy = enemiesManager_.GetComponent(lightFighterIndexes[0]);
	enemy.weapons.push_back(lightFighterIndexes[3]);
	enemy.enemyReactorIndex = lightFighterIndexes[8];
	enemiesManager_.SetComponent(lightFighterIndexes[0], enemy);
	
	// Assign gun position to the projectile weapon
	Weapon projectileWeapon = weaponManager_.GetComponent(lightFighterIndexes[3]);
	projectileWeapon.gunPositions.resize(2);
	projectileWeapon.targets.resize(1);
	projectileWeapon.gunPositions[0] = lightFighterIndexes[1];
	projectileWeapon.gunPositions[1] = lightFighterIndexes[2];
	weaponManager_.SetComponent(lightFighterIndexes[3], projectileWeapon);
	
	return lightFighterIndexes;
}

std::vector<ecs::EntityIndex> GamePrefabsManager::InstantiateDestroyer() const {
	std::vector<ecs::EntityIndex> destroyerIndexes = Instantiate("Destroyer");

	EnemiesManager& enemiesManager_ = EcsManagerLocator::Get().GetComponentsManager<EnemiesManager>();
	WeaponManager& weaponManager_ = EcsManagerLocator::Get().GetComponentsManager<WeaponManager>();

	// Assign weapon to the enemy
	Enemy enemy = enemiesManager_.GetComponent(destroyerIndexes[0]);
	enemy.weapons.push_back(destroyerIndexes[6]);
	enemy.weapons.push_back(destroyerIndexes[9]);
	enemy.weapons.push_back(destroyerIndexes[12]);
	enemy.weapons.push_back(destroyerIndexes[15]);
	enemy.enemyReactorIndex = destroyerIndexes[27];
	enemiesManager_.SetComponent(destroyerIndexes[0], enemy);

	// Assign gun position to the first projectile weapon
	Weapon projectileWeapon = weaponManager_.GetComponent(destroyerIndexes[6]);
	projectileWeapon.gunPositions.resize(2);
	projectileWeapon.targets.resize(1);
	projectileWeapon.gunPositions[0] = destroyerIndexes[7];
	projectileWeapon.gunPositions[1] = destroyerIndexes[8];
	weaponManager_.SetComponent(destroyerIndexes[6], projectileWeapon);

	// Assign gun position to the second projectile weapon
	projectileWeapon = weaponManager_.GetComponent(destroyerIndexes[9]);
	projectileWeapon.gunPositions.resize(2);
	projectileWeapon.targets.resize(1);
	projectileWeapon.gunPositions[0] = destroyerIndexes[10];
	projectileWeapon.gunPositions[1] = destroyerIndexes[11];
	weaponManager_.SetComponent(destroyerIndexes[9], projectileWeapon);

	// Assign gun position to the third projectile weapon
	projectileWeapon = weaponManager_.GetComponent(destroyerIndexes[12]);
	projectileWeapon.gunPositions.resize(2);
	projectileWeapon.targets.resize(1);
	projectileWeapon.gunPositions[0] = destroyerIndexes[13];
	projectileWeapon.gunPositions[1] = destroyerIndexes[14];
	weaponManager_.SetComponent(destroyerIndexes[12], projectileWeapon);

	// Assign gun position to the fourth projectile weapon
	projectileWeapon = weaponManager_.GetComponent(destroyerIndexes[15]);
	projectileWeapon.gunPositions.resize(2);
	projectileWeapon.targets.resize(1);
	projectileWeapon.gunPositions[0] = destroyerIndexes[16];
	projectileWeapon.gunPositions[1] = destroyerIndexes[17];
	weaponManager_.SetComponent(destroyerIndexes[15], projectileWeapon);

	return destroyerIndexes;
}

std::vector<ecs::EntityIndex> GamePrefabsManager::Instantiate(
    const nonstd::string_view& prefabName) const
{
    const auto hash = math::HashString(prefabName);
    for (size_t i = 0; i < prefabsID_.size(); i++) {
        if (hash == prefabsID_[i]) {
            return EcsManagerLocator::Get().InstantiatePrefab(prefabs_[i]);
        }
    }

    LogWarning(
        "You're trying to instantiate a prefab with the name " + static_cast<std
            ::string>(prefabName) +
        " but this prefab has not been loaded by the manager",
        LogType::ECS_LOG);
    return {};
}

void GamePrefabsManager::AddPrefab(const std::string& prefabName) 
{
    const auto hash = math::HashString(prefabName);

	const auto it = std::find(prefabsID_.begin(), prefabsID_.end(), hash);
	if (it != prefabsID_.end()) return;

    prefabsID_.push_back(hash);

    auto prefabJson = LoadJson(
        prefabName,
        FileType::PREFAB,
        FolderType::ROM);

	prefabJson[0]["prefabName"] = prefabName;

    prefabs_.emplace_back();
    prefabs_.back().SetFromJson(prefabJson);
}

const ecs::Prefab& GamePrefabsManager::GetPrefab(
    const nonstd::string_view& prefabName) const
{
    const auto hash = math::HashString(prefabName);
    const auto it = std::find(
        prefabsID_.begin(),
        prefabsID_.end(),
        hash);

    const auto index = std::distance(prefabsID_.begin(), it);

    return prefabs_[index];
}

const std::vector<ResourceID>& GamePrefabsManager::GetPrefabsIDs() const
{
    return prefabsID_;
}

json GamePrefabsManager::ToJson()
{
    LogWarning(
        "Impossible to save with the GamePrefabManager as its meant to be read-only");
    return json();
}

void GamePrefabsManager::SetFromJson(const json& prefabsJson)
{
	for (const auto& prefabJson : prefabsJson) {
		std::string prefabName = prefabJson;

		AddPrefab(prefabName);
	}
}

void GamePrefabsManager::Clear()
{
	prefabs_.clear();
	prefabsID_.clear();
}

void GamePrefabsManager::Resize(const size_t newSize)
{
	prefabsID_.resize(newSize);
	prefabs_.resize(newSize);
}
} //namespace game
} //namespace poke
