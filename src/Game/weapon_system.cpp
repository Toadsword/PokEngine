#include <Game/weapon_system.h>
#include <Utility/profiler.h>
#include <CoreEngine/engine.h>
#include <Game/game.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Utility/time_custom.h>
#include <Game/Components/projectile.h>
#include <Game/Components/weapon.h>

namespace poke::game {
WeaponSystem::WeaponSystem(Engine& engine, Game& game) :
	GameSystem(engine, game),
	time_(Time::Get()),
	weaponManager_(ecsManager_.GetComponentsManager<WeaponManager>()),
	projectileManager_(ecsManager_.GetComponentsManager<ProjectileManager>()),
	missilesManager_(ecsManager_.GetComponentsManager<MissilesManager>()),
	playerManager_(ecsManager_.GetComponentsManager<PlayerManager>()),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
	modelsManager_(ecsManager_.GetComponentsManager<ecs::ModelsManager>()),
	lightsManager_(ecsManager_.GetComponentsManager<ecs::LightsManager>()),
	collidersManager_(ecsManager_.GetComponentsManager<ecs::CollidersManager>()),
	gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
	gamePrefabsManager_(static_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())),
    audioSourcesManager_(ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>())
{
	ObserveLoadScene();
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	ObserveEntitySetActive();
	ObserveEntitySetInactive();

	ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
	{OnAddComponent(entityIndex, component); }
	);
	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
	{OnRemoveComponent(entityIndex, component); }
	);
	ObserveLoadScene();
}

void WeaponSystem::OnUpdate() {
	pok_BeginProfiling(Weapon_System, 0);
	for (const ecs::EntityIndex entityIndex : entityIndexes_) { 
		const Weapon weapon = weaponManager_.GetComponent(entityIndex); 
		if (weapon.isShooting)
			Shoot(entityIndex);

		if (weapon.weaponType == Weapon::WeaponType::PROJECTILE ||
			weapon.weaponType == Weapon::WeaponType::PROJECTILE_PLAYER)
			UpdateLight(entityIndex);
	}
	pok_EndProfiling(Weapon_System);
}

void WeaponSystem::OnLoadScene() {
	gamePrefabsManager_.AddPrefab("ProjectilePlayer");
	gamePrefabsManager_.AddPrefab("MissilePlayer");
	gamePrefabsManager_.AddPrefab("Projectile");
	gamePrefabsManager_.AddPrefab("Missile");

    if(!archetypeAlreadyLoad_) {
	    //TODO (@Robin) if the vertical slice can't load with this method to get the value of each prefab, Create the projectile and missile by hand for all four
	    archetypeMissilePlayer_ = reinterpret_cast<const GameArchetype&>(
		    PrefabsManagerLocator::Get().GetPrefab(kMissilePlayerName_).GetObject(0));
		archetypeMissilePlayer_.transform.SetLocalPosition({ 1000000 });
    	
	    archetypeMissile_ = reinterpret_cast<const GameArchetype&>(
		    PrefabsManagerLocator::Get().GetPrefab(kMissileName_).GetObject(0));
		archetypeMissile_.transform.SetLocalPosition({ 1000000 });

	    archetypeProjectilePlayer_ = reinterpret_cast<const GameArchetype&>(
		    PrefabsManagerLocator::Get().GetPrefab(kProjectilePlayerName_).GetObject(0));
		archetypeProjectilePlayer_.transform.SetLocalPosition({ 1000000 });

	    archetypeProjectile_ = reinterpret_cast<const GameArchetype&>(
		    PrefabsManagerLocator::Get().GetPrefab(kProjectileName_).GetObject(0));
		archetypeProjectile_.transform.SetLocalPosition({ 1000000 });
	    //End part to replace if trouble

	    ArchetypesManagerLocator::Get().AddArchetype(archetypeMissilePlayer_, kMissilePlayerName_, kMissilePlayerNb_);
	    ArchetypesManagerLocator::Get().AddArchetype(archetypeMissile_, kMissileName_, kMissileNb_);
	    ArchetypesManagerLocator::Get().AddArchetype(archetypeProjectilePlayer_, kProjectilePlayerName_, kProjectilePlayerNb_);
	    ArchetypesManagerLocator::Get().AddArchetype(archetypeProjectile_, kProjectileName_, kProjectileNb_);

	    missilePlayerId_ = ArchetypesManagerLocator::Get().GetArchetypeID(kMissilePlayerName_);
	    missileId_ = ArchetypesManagerLocator::Get().GetArchetypeID(kMissileName_);
	    projectilePlayerId_ = ArchetypesManagerLocator::Get().GetArchetypeID(kProjectilePlayerName_);
	    projectileId_ = ArchetypesManagerLocator::Get().GetArchetypeID(kProjectileName_);

		archetypeAlreadyLoad_ = true;
    }
}

void WeaponSystem::Shoot(const ecs::EntityIndex entityIndex) {
	Weapon weapon = weaponManager_.GetComponent(entityIndex);
    const float deltaTime = static_cast<float>(Time::Get().deltaTime.count());
	weapon.lastShootAt = weapon.lastShootAt - deltaTime;
	if (weapon.lastShootAt <= 0) {
		weapon = weaponManager_.GetComponent(entityIndex);

		weapon.lastShootAt = weapon.shootCoolDown * 1000;

		weaponManager_.SetComponent(entityIndex, weapon);

		/*if (weapon.weaponType == Weapon::WeaponType::PROJECTILE ||
			weapon.weaponType == Weapon::WeaponType::PROJECTILE_PLAYER)*/
			ShootProjectile(entityIndex);
		/*else if (weapon.weaponType == Weapon::WeaponType::MISSILE ||
				 weapon.weaponType == Weapon::WeaponType::MISSILE_PLAYER)
			ShootMissile(entityIndex);*/

		weapon = weaponManager_.GetComponent(entityIndex);
		
		weapon.activeGunID++;

		if (weapon.activeGunID >= weapon.gunPositions.size())
			weapon.activeGunID = 0;
	}
	weaponManager_.SetComponent(entityIndex, weapon);
}

void WeaponSystem::ShootProjectile(const ecs::EntityIndex entityIndex) {
	Weapon weapon = weaponManager_.GetComponent(entityIndex);
		
	ecs::Light light = lightsManager_.GetComponent(entityIndex);
	light.pointLight.intensity = weapon.lightIntensity;
	lightsManager_.SetComponent(entityIndex, light);

	ecs::EntityIndex projectileIndex;
	math::Vec3 targetVelocity;
	std::cout << "Shoot !\n";
	if (weapon.weaponType == Weapon::WeaponType::PROJECTILE)
		projectileIndex = ecsManager_.AddEntity(projectileId_);
	else
		projectileIndex = ecsManager_.AddEntity(projectilePlayerId_);
	
	Projectile projectile = projectileManager_.GetComponent(projectileIndex);
	projectile.shootAtTime = time_.GetTime();
	projectile.origin = weapon.origin;
	projectileManager_.SetComponent(projectileIndex, projectile);

	math::Transform currentGunWorldPosition;
	math::Transform currentGunLocalPosition;
	math::Vec3 currentProjectileScale = transformsManager_.GetComponent(projectileIndex).GetLocalScale();
	currentGunWorldPosition.SetLocalPosition(transformsManager_.GetWorldPosition(weapon.gunPositions[weapon.activeGunID]));
	currentGunWorldPosition.SetLocalRotation(transformsManager_.GetWorldRotation(weapon.gunPositions[weapon.activeGunID]));
	currentGunWorldPosition.SetLocalScale(currentProjectileScale);
	currentGunLocalPosition.SetLocalPosition(transformsManager_.GetComponent(weapon.gunPositions[weapon.activeGunID]).GetLocalPosition());
	transformsManager_.SetComponent(projectileIndex, currentGunWorldPosition);
	transformsManager_.SetComponent(entityIndex, currentGunLocalPosition);
	
	targetVelocity = weapon.shootDirection * projectile.moveSpeed;

	physics::Rigidbody rigid = rigidbodyManager_.GetComponent(projectileIndex);
	rigid.linearVelocity = targetVelocity;
	rigidbodyManager_.SetComponent(projectileIndex, rigid);

    //Play sound, audio source is on same object than weapon
	auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
	if (audioSource.IsPlaying()) return;
	audioSource.PlayInstance(currentGunWorldPosition.GetLocalPosition());
}

void WeaponSystem::ShootMissile(const ecs::EntityIndex entityIndex) {
	Weapon weapon = weaponManager_.GetComponent(entityIndex);

    if(weapon.targets.empty()) {
		weapon.isShooting = false;
		weapon.targets.clear();
		weaponManager_.SetComponent(entityIndex, weapon);
    	return;
    }

	ecs::EntityIndex missilesIndex;

	if (weapon.weaponType == Weapon::WeaponType::MISSILE) {
		missilesIndex = ecsManager_.AddEntity(missileId_);
	}
	else {
		missilesIndex = ecsManager_.AddEntity(missilePlayerId_);
	}
	
	Missile missile = missilesManager_.GetComponent(missilesIndex);

	missile.target = weapon.targets.back();
	weapon.targets.pop_back();
	missile.origin = weapon.origin;

	math::Transform missileTransform = transformsManager_.GetComponent(weapon.gunPositions[weapon.activeGunID]);
	
	transformsManager_.SetParent(missilesIndex, weapon.origin);
	transformsManager_.SetComponent(missilesIndex, missileTransform);
	missilesManager_.SetComponent(missilesIndex, missile);
	weaponManager_.SetComponent(entityIndex, weapon);
}

void WeaponSystem::UpdateLight(const ecs::EntityIndex entityIndex) const {
	Weapon weapon = weaponManager_.GetComponent(entityIndex);
	ecs::Light light = lightsManager_.GetComponent(entityIndex);
	
	if (light.pointLight.intensity > 0.1f) {
		light.pointLight.intensity -= weapon.lightIntensity / weapon.lightDuration;

		if (light.pointLight.intensity < 0.1f)
			light.pointLight.intensity = 0.0f;

		lightsManager_.SetComponent(entityIndex, light);
	}
}

void WeaponSystem::OnAddComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	//Check if the entity have the component required
	if ((component & ecs::ComponentType::WEAPON) != ecs::ComponentType::WEAPON) { return; }

	//Check if the entity is active
	if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

	if (entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);

	Weapon weapon = weaponManager_.GetComponent(entityIndex);
	weapon.targets.resize(1);
	weaponManager_.SetComponent(entityIndex, weapon);
}

void WeaponSystem::OnRemoveComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	if ((component & ecs::ComponentType::WEAPON) != ecs::ComponentType::WEAPON) {
		return;
	}
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}

void WeaponSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::WEAPON)) { return; }

	if (entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}

void WeaponSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove the deactivated entity for the list of entities
	entityIndexes_.erase(it);
}
}// namespace poke::game
