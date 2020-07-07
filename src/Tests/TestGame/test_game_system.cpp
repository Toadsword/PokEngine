#include <Tests/TestEcs/move.h>

#include <Ecs/core_ecs_manager.h>
#include <Utility/log.h>

#include <CoreEngine/engine.h>
#include <Tests/TestGame/test_game_system.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke
{
TestGameSystem::TestGameSystem(Engine& engineExport)
	: System(engineExport),
	splinesManager_(ecsManager_.GetComponentsManager<ecs::SplineFollowersManager>(ecs::ComponentType::SPLINE_FOLLOWER)),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>(ecs::ComponentType::TRANSFORM)),
    playerManager_(ecsManager_.GetComponentsManager<game::PlayerManager>(ecs::ComponentType::PLAYER)),
    destructibleElementManager_(ecsManager_.GetComponentsManager<game::DestructibleElementManager>(ecs::ComponentType::DESTRUCTIBLE_ELEMENT)),
    modelsManager_(ecsManager_.GetComponentsManager<ecs::ModelsManager>(ecs::ComponentType::MODEL)),
	colliderManager_(ecsManager_.GetComponentsManager<ecs::CollidersManager>(ecs::ComponentType::COLLIDER)),
	weaponManager_(ecsManager_.GetComponentsManager<game::WeaponManager>(ecs::ComponentType::WEAPON)) {
	ObserveLoadScene();	
}

void TestGameSystem::OnLoadScene() {
	const int indexPlayer = ecsManager_.AddEntity();
	const int indexDestructibleElement = ecsManager_.AddEntity();

	// Model component
	TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
	std::string materialName = "metal_test";
	auto materialHash = XXH64(&materialName[0], materialName.size(), math::kHashSeed);
	MaterialsManagerLocator::Get().AddMaterial(materialName);
	std::string meshName = "msh_astero2_low.obj.obj";
	auto meshHash = XXH64(&meshName[0], meshName.size(), math::kHashSeed);
	MeshManagerLocator::Get().AddMesh(meshName);
	graphics::Model model(materialHash, meshHash);
	modelsManager_.SetComponent(indexPlayer, model);
	modelsManager_.SetComponent(indexDestructibleElement, model);

	math::Vec3 cameraPos = graphics::CameraData().position;
	cameraPos.z += 10.0f;
	// Transform component
	math::Transform transform_;
	transform_.SetLocalPosition(cameraPos);
	transform_.SetLocalScale({ 0.005f,0.005f,0.005f });
	transformsManager_.SetComponent(indexPlayer, transform_);
	cameraPos.z += 20.0f;
	transform_.SetLocalPosition(cameraPos);
	transformsManager_.SetComponent(indexDestructibleElement, transform_);

	// Destructible element component
	game::DestructibleElement destructibleElement_;
	destructibleElement_.life = 10;
	destructibleElement_.scorePointGiven = 25;
	destructibleElement_.destructionPointGiven = 5;

	destructibleElementManager_.SetComponent(indexDestructibleElement, destructibleElement_);

	// Spline component
	/*std::vector<math::Vec3> points = {
		math::Vec3(-1, -2, -1),
		math::Vec3(-1, -1, -1),
		math::Vec3(-1, 1, -1),
		math::Vec3(-1, -1, 1),
		math::Vec3(-1, 1, 1),
		math::Vec3(-1, 1, 1)
	};

	splinesManager_.SetComponent(indexDestructibleElement, ecs::Spline(points));*/

	// Player component
	game::Player player_;
	player_.life = 10;
	player_.mode = game::Player::PlayerMode::STANDARD;
	player_.missileAmount = 10;
	player_.moveSpeed = 1.0f;
	player_.maxMoveSpeed = 10.0f;
	player_.dashSpeed = 50.0f;
	player_.dashDuration = 0.25f;
	player_.dashCooldown = 1.0f;

	playerManager_.SetComponent(indexPlayer, player_);

	// Collider component
	physics::Collider collider_;
	physics::MeshShape shape_(MeshManagerLocator::Get().GetSphere());
	collider_.isTrigger = true;
	collider_.SetShape(shape_);
	colliderManager_.SetComponent(indexPlayer, collider_);
	colliderManager_.SetComponent(indexDestructibleElement, collider_);

	// Weapon component
	game::Weapon weapon_;
	weapon_.gunPositions.push_back(transform_);
	weapon_.shootCoolDown = 0.1f;
	weapon_.lastShootAt = -10.0f;
	weapon_.projectile.moveSpeed = 1.0f;
	weapon_.projectile.damage = 10;
	weapon_.projectile.durationLifeTime = 1.0f;

	weaponManager_.SetComponent(indexPlayer, weapon_);

	// Player construction
	ecsManager_.AddComponent(indexPlayer, ecs::ComponentType::PLAYER);
	ecsManager_.AddComponent(indexPlayer, ecs::ComponentType::TRANSFORM);
	ecsManager_.AddComponent(indexPlayer, ecs::ComponentType::RIGIDBODY);
	ecsManager_.AddComponent(indexPlayer, ecs::ComponentType::COLLIDER);
	ecsManager_.AddComponent(indexPlayer, ecs::ComponentType::MODEL);
	ecsManager_.AddComponent(indexPlayer, ecs::ComponentType::WEAPON);

	// Destructible element construction
	ecsManager_.AddComponent(indexDestructibleElement, ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
	ecsManager_.AddComponent(indexDestructibleElement, ecs::ComponentType::TRANSFORM);
	//ecsManager_.AddComponent(indexDestructibleElement, ecs::ComponentType::SPLINE_FOLLOWER);
	ecsManager_.AddComponent(indexDestructibleElement, ecs::ComponentType::COLLIDER);
	ecsManager_.AddComponent(indexDestructibleElement, ecs::ComponentType::RIGIDBODY);
	ecsManager_.AddComponent(indexDestructibleElement, ecs::ComponentType::MODEL);
}
}//namespace poke
