#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <Utility/file_system.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Tests/TestGame/test_game_system.h>
#include <Game/app_systems_container.h>
#include <Game/main_menu.h>
#include <Game/app_components_managers_container.h>
#include "Utility/log.h"
#include <Game/Ecs/game_archetype.h>
#include <Math/hash.h>
#include "Game/ComponentManagers/spline_state_manager.h"

class MovingGameCameraSystem final : public poke::game::GameSystem {
public:
	explicit MovingGameCameraSystem(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		inputManager_(engine.GetModuleManager().inputManager) {
		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}
	~MovingGameCameraSystem() = default;

	void Init() const
	{
		camera_.SetPosition(basePosition);
		camera_.SetRotation(poke::math::Vec3::Multiply(baseRotation, {poke::math::kDeg2Rad}));
		camera_.Update();
	}
	
	void OnUpdate()
	{
		if (inputManager_.IsKeyDown(poke::input::KeyCode::M))
			cameraMovementIsEnable = !cameraMovementIsEnable;

		if (cameraMovementIsEnable)
			MoveCamera();
	}
	
	void MoveCamera()
	{
		poke::math::Vec3 positionDelta;

		if (inputManager_.IsKeyHeld(poke::input::KeyCode::W)) { positionDelta.z -= 0.5f; }
		if (inputManager_.IsKeyHeld(poke::input::KeyCode::S)) { positionDelta.z += 0.5f; }
		if (inputManager_.IsKeyHeld(poke::input::KeyCode::D)) { positionDelta.x -= 0.5f; }
		if (inputManager_.IsKeyHeld(poke::input::KeyCode::A)) { positionDelta.x += 0.5f; }

		if (inputManager_.IsKeyHeld(poke::input::KeyCode::KEY_LEFT_SHIFT)) { positionDelta *= 2; }


		if (inputManager_.IsMouseButtonDown(poke::input::MouseButtonCode::RIGHT)) 
		{
			dragPosition_ = inputManager_.GetMousePosition();
		}

		poke::math::Vec3 rotation = camera_.GetRotation();
		if (inputManager_.IsMouseButtonHeld(poke::input::MouseButtonCode::RIGHT)) 
		{
			const poke::math::Vec3 rotationDelta = (dragPosition_ - inputManager_.GetMousePosition())* 0.005f;
			rotation.x = rotationDelta.y;
			if (rotation.x < -89.0f * poke::math::kDeg2Rad) { rotation.x = -89.0f * poke::math::kDeg2Rad; }
			if (rotation.x > 89.0f * poke::math::kDeg2Rad) { rotation.x = 89.0f * poke::math::kDeg2Rad; }
			rotation.y = rotationDelta.x;
		}

		poke::math::Vec3 position = camera_.GetPosition();

		position.x += -(positionDelta.z * std::sin(rotation.y) + positionDelta.x * std::cos(rotation.y));
		position.y += (positionDelta.z * std::sin(rotation.x));
		position.z += (positionDelta.z * std::cos(rotation.y) - positionDelta.x * std::sin(position.y));

		camera_.SetPosition(position);
		camera_.SetRotation(rotation);
		camera_.Update();
	}

	void SetBasePosition(poke::math::Vec3 position) { basePosition = position; }
	void SetBaseRotation(poke::math::Vec3 position) { baseRotation = position; }
	void EnableCameraMovement(bool enable) { cameraMovementIsEnable = enable; }
private:
	bool cameraMovementIsEnable = false;
	
	poke::ICamera& camera_;

	poke::input::InputManager& inputManager_;

	const float kMaxOrientation_ = 89.0f;
	const float kSensitivity_ = 0.5f;
	poke::math::Vec2 dragPosition_;

	poke::math::Vec3 basePosition = { 0.0f };
	poke::math::Vec3 baseRotation = { 0.0f, 180.0f, 0.0f };
};

class TestProjectileSystem final : public poke::game::GameSystem {
public:
	explicit TestProjectileSystem(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		splinesManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		playerManager_(ecsManager_.GetComponentsManager<poke::game::PlayerManager>()),
		destructibleElementManager_(ecsManager_.GetComponentsManager<poke::game::DestructibleElementManager>()),
		modelsManager_(ecsManager_.GetComponentsManager<poke::ecs::ModelsManager>()),
		colliderManager_(ecsManager_.GetComponentsManager<poke::ecs::CollidersManager>()),
		weaponManager_(ecsManager_.GetComponentsManager<poke::game::WeaponManager>()),
		rigidbodyManager_(ecsManager_.GetComponentsManager<poke::ecs::RigidbodyManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) { }

	~TestProjectileSystem() = default;

	void OnUpdate() {
		updateCount_++;

		if (updateCount_ >= updateLoop_) {
			engine_.Stop();
			ASSERT_FALSE(CheckTestResult());
		}

		gizmoCommandBuffer_.SetColor(poke::Color::yellow);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(weaponIndex_).GetLocalPosition(), 1.0f);

		gizmoCommandBuffer_.SetColor(poke::Color::green);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(destructibleIndex_).GetLocalPosition(), 1.0f);
	}

	void InitTest() {
		poke::math::Vec3 position = { 0.0f, 0.0f, 0.0f };

		poke::TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = poke::math::HashString(materialName);
		poke::MaterialsManagerLocator::Get().AddMaterial(materialName);
		std::string meshName = "msh_astero2_low.obj";
		auto meshHash = poke::math::HashString("msh_astero2_low.obj");
		poke::MeshManagerLocator::Get().AddMesh(meshName);
		defaultModel_ = poke::graphics::Model{ materialHash, meshHash };

		const float offsetWeapon = 10.0f;

		destructibleIndex_ = CreateDestructibleElement(position);

		position.z -= offsetWeapon;

		weaponIndex_ = CreateWeapon(position);

		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}
	
	bool CheckTestResult() const {
		return ecsManager_.HasComponent(destructibleIndex_, poke::ecs::EntityFlag::IS_ACTIVE);
	}
private:
	poke::ecs::EntityIndex CreateDestructibleElement(poke::math::Vec3 position) {
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		// Prepare the components values
		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		const float scaleDestructibleObject = 0.005f;
		transform.SetLocalScale({ scaleDestructibleObject });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::DestructibleElement destructibleElement;
		destructibleElement.type = poke::game::DestructibleElement::Type::ASTEROID;
		destructibleElement.life = 1;
		destructibleElement.scorePointGiven = 25;
		destructibleElement.destructionPointGiven = 5;
		destructibleElementManager_.SetComponent(entityIndex, destructibleElement);

		poke::physics::Rigidbody rigid;
		rigid.type = poke::physics::RigidbodyType::DYNAMIC;
		rigid.linearVelocity = { 0.0f, 0.0f,0.0f };
		rigidbodyManager_.SetComponent(entityIndex, rigid);

		poke::physics::Collider collider;
		poke::physics::SphereShape shape({ 0.0f, 0.0f, 0.0f });
		collider.isTrigger = true;
		collider.SetShape(shape);
		colliderManager_.SetComponent(entityIndex, collider);

		modelsManager_.SetComponent(entityIndex, defaultModel_);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::COLLIDER);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::MODEL);

		return entityIndex;
	}
	poke::ecs::EntityIndex CreateWeapon(poke::math::Vec3 position) {
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 0.005f, 0.005f, 0.005f });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::Weapon weapon;
		weapon.gunPositions.push_back(entityIndex);
		weapon.targets.push_back(destructibleIndex_);
		weapon.shootCoolDown = 0.1f;
		weapon.lastShootAt = -10.0f;
		weapon.weaponType = poke::game::Weapon::WeaponType::PROJECTILE_PLAYER;
		weapon.isShooting = true;
		weaponManager_.SetComponent(entityIndex, weapon);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::WEAPON);

		return entityIndex;
	};

	poke::ecs::SplineFollowersManager& splinesManager_;
	poke::ecs::TransformsManager& transformsManager_;
	poke::game::PlayerManager& playerManager_;
	poke::game::DestructibleElementManager& destructibleElementManager_;
	poke::ecs::ModelsManager& modelsManager_;
	poke::ecs::CollidersManager& colliderManager_;
	poke::game::WeaponManager& weaponManager_;
	poke::ecs::RigidbodyManager& rigidbodyManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	poke::graphics::Model defaultModel_;
	int updateLoop_ = 200;
	int updateCount_ = 0;

	poke::ecs::EntityIndex weaponIndex_;
	poke::ecs::EntityIndex destructibleIndex_;
};

class TestMissileSystem final : public poke::ecs::System {
public:
	explicit TestMissileSystem(poke::Engine& engineExport)
		: System(engineExport),
		splinesManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		playerManager_(ecsManager_.GetComponentsManager<poke::game::PlayerManager>()),
		destructibleElementManager_(ecsManager_.GetComponentsManager<poke::game::DestructibleElementManager>()),
		modelsManager_(ecsManager_.GetComponentsManager<poke::ecs::ModelsManager>()),
		colliderManager_(ecsManager_.GetComponentsManager<poke::ecs::CollidersManager>()),
		missileManager_(ecsManager_.GetComponentsManager<poke::game::MissilesManager>()),
		rigidbodyManager_(ecsManager_.GetComponentsManager<poke::ecs::RigidbodyManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())) {	}

	~TestMissileSystem() = default;

	void OnUpdate() {
		updateCount_++;

		if (updateCount_ >= updateLoop_) {
			engine_.Stop();
			ASSERT_TRUE(CheckTestResult());
		}
	}

	void InitTest() {
		poke::TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = poke::math::HashString(materialName);
		poke::MaterialsManagerLocator::Get().AddMaterial(materialName);
		std::string meshName = "msh_astero2_low.obj";
		auto meshHash = poke::math::HashString(meshName);
		poke::MeshManagerLocator::Get().AddMesh(meshName);
		defaultModel_ = poke::graphics::Model{ materialHash, meshHash };

		meshName = "missileTest.obj";
		meshHash = poke::math::HashString(meshName);
		poke::MeshManagerLocator::Get().AddMesh(meshName);
		missileModel_ = poke::graphics::Model{ materialHash, meshHash };

		const poke::math::Vec3 offsetMultipliers[kNbLaunch] ={
		    {0.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
		    {1.0f, 0.0f, 1.0f},
		    {1.0f, -1.0f, 1.0f},
		    {0.0f, -1.0f, 1.0f},
		    {-1.0f, -1.0f, 1.0f},
		    {-1.0f, 0.0f, 1.0f},
		    {-1.0f, 1.0f, 1.0f},
		    {0.0f, 0.0f, -1.0f}
		};

		const poke::math::Vec3 offset(25.0f, 25.0f, 50.0f);

		for (size_t index = 0; index < kNbLaunch; index++) {
			destructibleIndexes_[index] = 
				CreateDestructibleElement({ 
				    offset.x * offsetMultipliers[index].x,
				    offset.y * offsetMultipliers[index].y,
				    offset.z * offsetMultipliers[index].z
				});
			missileIndexes_[index] = CreateMissile({0}, destructibleIndexes_[index]);
        }

		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	bool CheckTestResult() const {
		bool asteroOne = ecsManager_.HasComponent(destructibleIndexes_[0], poke::ecs::EntityFlag::IS_ACTIVE);
		bool asteroTwo = ecsManager_.HasComponent(destructibleIndexes_[1], poke::ecs::EntityFlag::IS_ACTIVE);
        bool asteroThree = ecsManager_.HasComponent(destructibleIndexes_[8], poke::ecs::EntityFlag::IS_ACTIVE);

		return
	        !asteroOne &&
			!asteroTwo &&
			 asteroThree;
	}
private:
	poke::ecs::EntityIndex CreateDestructibleElement(poke::math::Vec3 position) {
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		// Prepare the components values
		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 0.005f, 0.005f, 0.005f });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::DestructibleElement destructibleElement;
		destructibleElement.type = poke::game::DestructibleElement::Type::ASTEROID;
		destructibleElement.life = 1;
		destructibleElement.scorePointGiven = 25;
		destructibleElement.destructionPointGiven = 5;
		destructibleElementManager_.SetComponent(entityIndex, destructibleElement);

		poke::physics::Rigidbody rigid;
		rigid.type = poke::physics::RigidbodyType::DYNAMIC;
		rigid.linearVelocity = { 0.0f, 0.0f, 0.0f };
		rigidbodyManager_.SetComponent(entityIndex, rigid);

		poke::physics::Collider collider;
		poke::physics::SphereShape shape({ 0.0f, 0.0f, 0.0f }, 200.0f);
		collider.isTrigger = true;
		collider.SetShape(shape);
		colliderManager_.SetComponent(entityIndex, collider);

		modelsManager_.SetComponent(entityIndex, defaultModel_);

		ecsManager_.AddComponent(entityIndex,
			poke::ecs::ComponentType::DESTRUCTIBLE_ELEMENT |
			poke::ecs::ComponentType::TRANSFORM |
			poke::ecs::ComponentType::COLLIDER |
			poke::ecs::ComponentType::RIGIDBODY |
			poke::ecs::ComponentType::MODEL
		);

		return entityIndex;
	}

	poke::ecs::EntityIndex CreateMissile(poke::math::Vec3 position, poke::ecs::EntityIndex target) {
		poke::ecs::EntityIndex entityIndex = gamePrefabsManager_.Instantiate("MissilePlayer")[0];

		poke::physics::Rigidbody rigidbody = rigidbodyManager_.GetComponent(entityIndex);
		rigidbody.type = poke::physics::RigidbodyType::DYNAMIC;
		rigidbodyManager_.SetComponent(entityIndex, rigidbody);

		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 1.0f, 1.0f, 1.0f });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::Missile missile;
		missile.damage = 10;
		missile.lifeTime = 100.0f;
		missile.speed = 40.0f;
		missile.target = target;
		missileManager_.SetComponent(entityIndex, missile);

		return entityIndex;
	};

	poke::ecs::SplineFollowersManager& splinesManager_;
	poke::ecs::TransformsManager& transformsManager_;
	poke::game::PlayerManager& playerManager_;
	poke::game::DestructibleElementManager& destructibleElementManager_;
	poke::ecs::ModelsManager& modelsManager_;
	poke::ecs::CollidersManager& colliderManager_;
	poke::game::MissilesManager& missileManager_;
	poke::ecs::RigidbodyManager& rigidbodyManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;

	poke::graphics::Model defaultModel_;
	poke::graphics::Model missileModel_;

	int updateLoop_ = 1000;
	int updateCount_ = 0;

	inline static const std::size_t kNbLaunch = 9;

	std::array<poke::ecs::EntityIndex, kNbLaunch> missileIndexes_;

	std::array<poke::ecs::EntityIndex, kNbLaunch> destructibleIndexes_;
};

/* Manual test of the input, has to be done tree times with :
 *		- Mouse/Keyboard
 *		- PC Controller
 *		- Switch controller
 * If one input is not working, press escape to end the test
 * 1. Test the movement input
 * 2. Test the dash input
 * 3. Test the shoot input (maintain the input)
 * 4. Test the missile input
 *		4.1 Maintain the input
 *		4.2 Release the input
 * 5. Test the special capacity input
 */
class TestInputSystem final : public poke::ecs::System {
public:
	explicit TestInputSystem(poke::Engine& engine)
		: System(engine),
		inputManager_(engine.GetModuleManager().inputManager) {	}
	~TestInputSystem() = default;

	void OnUpdate() {
		ASSERT_FALSE(inputManager_.IsKeyDown(poke::input::KeyCode::ESCAPE));

		switch (state_) {
		case TestState::MOVEMENT_H:
			if (inputManager_.IsActionDown(poke::input::Action::RIGHT) || inputManager_.IsActionDown(poke::input::Action::LEFT)) {
				std::cout << "Vertical movement input test phase\n";
				state_ = TestState::MOVEMENT_V;
			}
			break;
		case TestState::MOVEMENT_V:
			if (inputManager_.IsActionDown(poke::input::Action::UP) || inputManager_.IsActionDown(poke::input::Action::DOWN)) {
				std::cout << "Dash input test phase\n";
				state_ = TestState::DASH;
			}
			break;
		case TestState::DASH:
			if (inputManager_.IsActionDown(poke::input::Action::DASH)) {
				std::cout << "Shoot input test phase, maintain until the next message shows up\n";
				state_ = TestState::SHOOT_PROJECTILE;
			}
			break;
		case TestState::SHOOT_PROJECTILE:
			if (inputManager_.IsActionHeld(poke::input::Action::MAIN_SHOOT)) {
				if (maintainedCount_ >= maintainedLoopToValidate_) {
					std::cout << "Missile aim input test phase, maintain until the next message shows up\n";
					maintainedCount_ = 0;
					state_ = TestState::SHOOT_MISSILE_MAINTAINED;
				}
				else {
					maintainedCount_++;
				}
			}
			break;
		case TestState::SHOOT_MISSILE_MAINTAINED:
			if (inputManager_.IsActionHeld(poke::input::Action::SECONDARY_TARGET)) {
				if (maintainedCount_ >= maintainedLoopToValidate_) {
					std::cout << "Missile shoot input test phase, release the current pressed input\n";
					maintainedCount_ = 0;
					state_ = TestState::SHOOT_MISSILE_RELEASED;
				}
				else {
					maintainedCount_++;
				}
			}
			break;
		case TestState::SHOOT_MISSILE_RELEASED:
			if (inputManager_.IsActionUp(poke::input::Action::SECONDARY_SHOOT)) {
				std::cout << "Special capacity test phase\n";
				state_ = TestState::SPECIAL_CAPACITY;
			}
			break;
		case TestState::SPECIAL_CAPACITY:
			if (inputManager_.IsActionDown(poke::input::Action::ABILITIES)) {
				std::cout << "Input test successful\n";
				state_ = TestState::DONE;
			}
			break;
		case TestState::DONE:
			engine_.Stop();
			break;
		default:
			if (inputManager_.IsKeyDown(poke::input::KeyCode::S)) {
				std::cout << "Horizontal movement input test phase\n";
				state_ = TestState::MOVEMENT_H;
			}
			break;
		}
	}

	void InitTest() {
		std::cout << "To start the input test, press S\nIf one input don't work, press escape to signal the failure\n";
		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	//TODO(@C�dric) Make a function to check has succed
private:
	enum class TestState : uint8_t
	{
		NONE = 0u,
		MOVEMENT_H,
		MOVEMENT_V,
		DASH,
		SHOOT_PROJECTILE,
		SHOOT_MISSILE_MAINTAINED,
		SHOOT_MISSILE_RELEASED,
		SPECIAL_CAPACITY,
		DONE
	};

	poke::input::InputManager& inputManager_;

	TestState state_ = TestState::NONE;

	int maintainedLoopToValidate_ = 100;
	int maintainedCount_ = 0;
};

class TestDestructionSystem final : public poke::game::GameSystem {
public:
	explicit TestDestructionSystem(poke::Engine& engine, poke::game::Game& game)
		: GameSystem(engine, game),
		splinesManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		playerManager_(ecsManager_.GetComponentsManager<poke::game::PlayerManager>()),
		destructibleElementManager_(ecsManager_.GetComponentsManager<poke::game::DestructibleElementManager>()),
		modelsManager_(ecsManager_.GetComponentsManager<poke::ecs::ModelsManager>()),
		colliderManager_(ecsManager_.GetComponentsManager<poke::ecs::CollidersManager>()),
		weaponManager_(ecsManager_.GetComponentsManager<poke::game::WeaponManager>()),
		rigidbodyManager_(ecsManager_.GetComponentsManager<poke::ecs::RigidbodyManager>()),
		jiggleManager_(ecsManager_.GetComponentsManager<poke::game::JiggleManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {	}
	~TestDestructionSystem() = default;

	void OnUpdate() {
		updateCount_++;

		gizmoCommandBuffer_.SetColor(poke::Color::yellow);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(weaponIndex_).GetLocalPosition(), 1.0f);

		gizmoCommandBuffer_.SetColor(poke::Color::green);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(destructibleIndex_).GetLocalPosition(), 1.0f);
	}

	void InitTest() {
		poke::math::Vec3 position = { 0.0f, 0.0f, 0.0f };
		
		poke::TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = poke::math::HashString(materialName);
		poke::MaterialsManagerLocator::Get().AddMaterial(materialName);
		std::string meshName = "msh_astero2_low.obj";
		auto meshHash = poke::math::HashString("msh_astero2_low.obj");
		poke::MeshManagerLocator::Get().AddMesh(meshName);
		defaultModel_ = poke::graphics::Model{ materialHash, meshHash };

		position.z += 20.0f;
		destructibleIndex_ = CreateDestructibleElement(position);
		for (int i = 0; i < 10; i++)
		{
			position.z += 20.0f;
			CreateDestructibleElement(position);
		}
		position = { 0.0f, 0.0f, 0.0f };
		weaponIndex_ = CreateWeapon(position);

		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	bool CheckTestResult() {
		return ecsManager_.HasComponent(destructibleIndex_, poke::ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
	}
private:
	poke::ecs::EntityIndex CreateDestructibleElement(poke::math::Vec3 position) {
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		// Prepare the components values
		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		float randScale = poke::math::Random::Get().RandomRange(0.002f, 0.05f);
		transform.SetLocalScale({ randScale });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::DestructibleElement destructibleElement;
		int typeIndex = poke::math::Random::Get().RandomRange(0, 2);
		typeIndex = 0;
		poke::game::DestructibleElement::Type type;
		switch (typeIndex) {
		case 0:
			type = poke::game::DestructibleElement::Type::ASTEROID;
			break;
		case 1:
			type = poke::game::DestructibleElement::Type::SHIP;
			break;
		case 2:
			type = poke::game::DestructibleElement::Type::TURRET;
			break;
		default:
			type = poke::game::DestructibleElement::Type::ASTEROID;
			break;
		}
		destructibleElement.type = type;
		destructibleElement.life = 1;
		destructibleElement.scorePointGiven = 25;
		destructibleElement.destructionPointGiven = 5;
		destructibleElementManager_.SetComponent(entityIndex, destructibleElement);

		poke::physics::Rigidbody rigid;
		rigid.type = poke::physics::RigidbodyType::DYNAMIC;
		rigid.linearVelocity = { 0.0f, 0.0f,0.0f };
		rigidbodyManager_.SetComponent(entityIndex, rigid);

		poke::physics::Collider collider;
		poke::physics::SphereShape shape({ 0.0f, 0.0f, 0.0f }, 0.5f);
		collider.isTrigger = true;
		collider.SetShape(shape);
		colliderManager_.SetComponent(entityIndex, collider);

		modelsManager_.SetComponent(entityIndex, defaultModel_);

		poke::game::Jiggle jiggle;
		jiggle.speed = 1.0f;
		jiggle.maxTime = 2.0f;
		jiggle.rotationSpeed = 0.05f;
		jiggleManager_.SetComponent(entityIndex, jiggle);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::COLLIDER);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::MODEL);
		//ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::JIGGLE);

		return entityIndex;
	}

	poke::ecs::EntityIndex CreateWeapon(poke::math::Vec3 position) {
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 0.005f, 0.005f, 0.005f });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::Weapon weapon;
		weapon.gunPositions.push_back(entityIndex);
		weapon.targets.push_back(destructibleIndex_);
		weapon.shootCoolDown = 2.0f;
		weapon.shootDirection = poke::math::Vec3(0, 0, 0.2f);
		weapon.weaponType = poke::game::Weapon::WeaponType::PROJECTILE_PLAYER;
		weapon.isShooting = true;
		weapon.origin = entityIndex;
		weaponManager_.SetComponent(entityIndex, weapon);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::WEAPON);

		return entityIndex;
	};

	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::ecs::SplineFollowersManager& splinesManager_;
	poke::ecs::TransformsManager& transformsManager_;
	poke::game::PlayerManager& playerManager_;
	poke::game::DestructibleElementManager& destructibleElementManager_;
	poke::ecs::ModelsManager& modelsManager_;
	poke::ecs::CollidersManager& colliderManager_;
	poke::game::WeaponManager& weaponManager_;
	poke::ecs::RigidbodyManager& rigidbodyManager_;
	poke::game::JiggleManager& jiggleManager_;

	poke::graphics::Model defaultModel_;
	int updateLoop_ = 200;
	int updateCount_ = 0;

	poke::ecs::EntityIndex weaponIndex_;
	poke::ecs::EntityIndex destructibleIndex_;
};

class TestPlayerSystem final : public poke::game::GameSystem {
public:
	explicit TestPlayerSystem(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		gamePrefabsManager_(reinterpret_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {
		gamePrefabsManager_.AddPrefab("Player");
		gamePrefabsManager_.AddPrefab("LightFighter");
		gamePrefabsManager_.AddPrefab("Destroyer");
		gamePrefabsManager_.AddPrefab("DestructibleA1");
		gamePrefabsManager_.AddPrefab("DestructibleA2");
		gamePrefabsManager_.AddPrefab("DestructibleA3");
		gamePrefabsManager_.AddPrefab("DestructibleA4");
		gamePrefabsManager_.AddPrefab("DestructibleA5");
		gamePrefabsManager_.AddPrefab("DestructibleA6");
	}
	~TestPlayerSystem() = default;

	void InitTest() {
		// Create the player at the front of the camera
		//InstiatePrefabAtPosition(camera_.GetPosition() + playerPositionOffset, "Player");
		const std::vector<poke::ecs::EntityIndex> playerIndex = gamePrefabsManager_.InstantiatePlayer();
		poke::math::Transform playerTransform = transformsManager_.GetComponent(playerIndex[0]);
		playerTransform.SetLocalPosition(camera_.GetPosition() + playerPositionOffset);
		transformsManager_.SetComponent(playerIndex[0], playerTransform);

		poke::game::Player player = ecsManager_.GetComponentsManager<poke::game::PlayerManager>().GetComponent(playerIndex[0]);
		player.movementArea = { 60.0f, 30.0f };
		player.movementArea.x -= camera_.GetPosition().x;
		player.movementArea.y += camera_.GetPosition().y;
		ecsManager_.GetComponentsManager<poke::game::PlayerManager>().SetComponent(playerIndex[0], player);

		// Create the destructible elements in front of the player
		//poke::math::Vec3 currentPos = playerTransform.GetLocalPosition() + destructibleElementBasePosition;
		//for (size_t z = 0; z < destructibleElementAmount.z; z++) {
		//	currentPos.y = destructibleElementBasePosition.y;
		//	for (size_t y = 0; y < destructibleElementAmount.y; y++) {
		//		currentPos.x = destructibleElementBasePosition.x;
		//		for (size_t x = 0; x < destructibleElementAmount.x; x++) {
		//			//const poke::ecs::EntityIndex lightFighterIndex = InstiatePrefabAtPosition(currentPos, "DestructibleA" + std::to_string(((rand() % 6) + 1)));
		//			//const poke::ecs::EntityIndex lightFighterIndex = gamePrefabsManager_.InstantiateLightFighter()[0];
		//			const poke::ecs::EntityIndex lightFighterIndex = gamePrefabsManager_.InstantiateDestroyer()[0];
		//			poke::math::Transform lightFighterTransform = transformsManager_.GetComponent(lightFighterIndex);
		//			lightFighterTransform.SetLocalPosition(currentPos);
		//			transformsManager_.SetComponent(lightFighterIndex, lightFighterTransform);
		//			currentPos.x += destructibleElementOffset.x;
		//		}
		//		currentPos.y += destructibleElementOffset.y;
		//	}
		//	currentPos.z += destructibleElementOffset.z;
		//}
	}
private:
	poke::ecs::EntityIndex InstiatePrefabAtPosition(poke::math::Vec3 position, std::string prefabName) {
		std::vector<poke::ecs::EntityIndex> entityIndex = gamePrefabsManager_.Instantiate(prefabName);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex[0]);
		transform.SetLocalPosition(position);
		transformsManager_.SetComponent(entityIndex[0], transform);

		return entityIndex[0];
	};

	poke::ICamera& camera_;
	poke::ecs::TransformsManager& transformsManager_;

	poke::game::GamePrefabsManager& gamePrefabsManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	poke::math::Vec3 playerPositionOffset = { 0.0f, 0.0f, 80.0f };
	poke::math::Vec3 destructibleElementBasePosition = { -70.0f, -30.0f, 30.0f };
	poke::math::Vec3 destructibleElementOffset = { 36.0f, 18.8f, 15.0f };
	poke::math::Vec3 destructibleElementAmount = { 5.0f };
};

class TestSpecialAttackSystem final : public poke::game::GameSystem {
public:
	explicit TestSpecialAttackSystem(poke::Engine& engine, poke::game::Game& game)
		: GameSystem(engine, game),
		time_(poke::Time::Get()),
		inputManager_(engine.GetModuleManager().inputManager),
		splinesManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		playerManager_(ecsManager_.GetComponentsManager<poke::game::PlayerManager>()),
		destructibleElementManager_(ecsManager_.GetComponentsManager<poke::game::DestructibleElementManager>()),
		modelsManager_(ecsManager_.GetComponentsManager<poke::ecs::ModelsManager>()),
		colliderManager_(ecsManager_.GetComponentsManager<poke::ecs::CollidersManager>()),
		specialAttackManager_(ecsManager_.GetComponentsManager<poke::game::SpecialAttackManager>()),
		rigidbodyManager_(ecsManager_.GetComponentsManager<poke::ecs::RigidbodyManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {	}
	~TestSpecialAttackSystem() = default;

	void InitTest()
	{
		poke::math::Vec3 position = { 0.0f, 0.0f, 0.0f };

		poke::TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		const auto materialHash = XXH64(&materialName[0], materialName.size(), poke::math::kHashSeed);
		poke::MaterialsManagerLocator::Get().AddMaterial(materialName);
		std::string meshName = "msh_astero2_low.obj";
		const auto meshHash = XXH64(&meshName[0], meshName.size(), poke::math::kHashSeed);
		poke::MeshManagerLocator::Get().AddMesh(meshName);
		defaultModel_ = poke::graphics::Model{ materialHash, meshHash };

		weaponIndex_ = CreateWeapon(position);

		position.z += 10.0f;

		destructibleIndex_ = CreateDestructibleElement(position);

		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	void OnUpdate()
	{
		updateCount_++;

		if (updateCount_ >= updateLoop_) {
			engine_.Stop();
			ASSERT_FALSE(CheckTestResult());
		}
		
		gizmoCommandBuffer_.SetColor(poke::Color::yellow);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(weaponIndex_).GetLocalPosition(), 1.0f);

		gizmoCommandBuffer_.SetColor(poke::Color::green);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(destructibleIndex_).GetLocalPosition(), 1.0f);
	}

	bool CheckTestResult() const {
		return ecsManager_.HasComponent(destructibleIndex_, poke::ecs::EntityFlag::IS_ACTIVE);
	}
private:
	poke::ecs::EntityIndex CreateDestructibleElement(poke::math::Vec3 position)
	{
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		// Prepare the components values
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 0.005f, 0.005f, 0.005f });
		transformsManager_.SetComponent(entityIndex, transform);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
		const poke::game::DestructibleElement destructibleElement{
			1, 25, 5,
			poke::game::DestructibleElement::Type::ASTEROID
		};
		destructibleElementManager_.SetComponent(entityIndex, destructibleElement);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY);
		poke::physics::Rigidbody rigid;
		rigid.type = poke::physics::RigidbodyType::DYNAMIC;
		rigidbodyManager_.SetComponent(entityIndex, rigid);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::COLLIDER);
		poke::physics::Collider collider;
		const poke::physics::SphereShape shape({ 0.0f, 0.0f, 0.0f });
		collider.isTrigger = true;
		collider.SetShape(shape);
		colliderManager_.SetComponent(entityIndex, collider);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::MODEL);
		modelsManager_.SetComponent(entityIndex, defaultModel_);

		return entityIndex;
	}

	poke::ecs::EntityIndex CreateWeapon(const poke::math::Vec3 position) const
	{
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 0.005f, 0.005f, 0.005f });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::SpecialAttack specialAttack;

		specialAttackManager_.SetComponent(entityIndex, specialAttack);

		poke::physics::Rigidbody rigid = rigidbodyManager_.GetComponent(
			entityIndex);
		rigid.type = poke::physics::RigidbodyType::DYNAMIC;
		rigidbodyManager_.SetComponent(entityIndex, rigid);

		modelsManager_.SetComponent(entityIndex, defaultModel_);

		const poke::physics::SphereShape shape({ 0.0f, 0.0f, 0.0f }, 1.0f);
		poke::physics::Collider collider;
		collider.isTrigger = true;
		collider.SetShape(shape);
		colliderManager_.SetComponent(entityIndex, collider);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::COLLIDER);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::MODEL);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::SPECIAL_ATTACK);

		return entityIndex;
	}

	poke::Time& time_;
	poke::input::InputManager& inputManager_;
	poke::ecs::SplineFollowersManager& splinesManager_;
	poke::ecs::TransformsManager& transformsManager_;
	poke::game::PlayerManager& playerManager_;
	poke::game::DestructibleElementManager& destructibleElementManager_;
	poke::ecs::ModelsManager& modelsManager_;
	poke::ecs::CollidersManager& colliderManager_;
	poke::game::SpecialAttackManager& specialAttackManager_;
	poke::ecs::RigidbodyManager& rigidbodyManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	poke::graphics::Model defaultModel_;
	const int updateLoop_ = 200;
	int updateCount_ = 0;

	poke::ecs::EntityIndex weaponIndex_{};
	poke::ecs::EntityIndex destructibleIndex_{};

	const uint8_t expansionFrameSpeed_ = 60;
	const float maxProjectileRadius_ = 50.0f;
};

class TestDashBumpSystem final : public poke::game::GameSystem {
public:
	explicit TestDashBumpSystem(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {
		gamePrefabsManager_.AddPrefab("Player");
		gamePrefabsManager_.AddPrefab("LightFighter");
		gamePrefabsManager_.AddPrefab("DestructibleA1");
		gamePrefabsManager_.AddPrefab("DestructibleA2");
		gamePrefabsManager_.AddPrefab("DestructibleA3");
		gamePrefabsManager_.AddPrefab("DestructibleA4");
		gamePrefabsManager_.AddPrefab("DestructibleA5");
		gamePrefabsManager_.AddPrefab("DestructibleA6");
	}
	~TestDashBumpSystem() = default;

	void InitTest() {
		// Create the player at the front of the camera
		//InstiatePrefabAtPosition(camera_.GetPosition() + playerPositionOffset, "Player");
		const poke::ecs::EntityIndex playerIndex = gamePrefabsManager_.InstantiatePlayer()[0];
		poke::math::Transform playerTransform = transformsManager_.GetComponent(playerIndex);
		playerTransform.SetLocalPosition(camera_.GetPosition() + playerPositionOffset);
		transformsManager_.SetComponent(playerIndex, playerTransform);

		poke::game::Player player = ecsManager_.GetComponentsManager<poke::game::PlayerManager>().GetComponent(playerIndex);
		player.movementArea = { 60.0f, 30.0f };
		player.movementArea.x -= camera_.GetPosition().x;
		player.movementArea.y += camera_.GetPosition().y;
		ecsManager_.GetComponentsManager<poke::game::PlayerManager>().SetComponent(playerIndex, player);

		// Create the destructible elements in front of the player
		poke::math::Vec3 currentPos = playerTransform.GetLocalPosition() + destructibleElementBasePosition;
		for (size_t y = 0; y < destructibleElementAmount.y; y++) {
			currentPos.x = destructibleElementBasePosition.x;
			for (size_t x = 0; x < destructibleElementAmount.x; x++) {
				//InstiatePrefabAtPosition(currentPos, "DestructibleA" + std::to_string(((rand() % 6) + 1)));
				const poke::ecs::EntityIndex lightFighterIndex = gamePrefabsManager_.InstantiateLightFighter()[0];
				poke::math::Transform lightFighterTransform = transformsManager_.GetComponent(lightFighterIndex);
				lightFighterTransform.SetLocalPosition(currentPos);
				transformsManager_.SetComponent(lightFighterIndex, lightFighterTransform);
				currentPos.x += destructibleElementOffset.x;
			}
			currentPos.y += destructibleElementOffset.y;
		}
	}
private:
	poke::ecs::EntityIndex InstiatePrefabAtPosition(poke::math::Vec3 position, std::string prefabName) {
		std::vector<poke::ecs::EntityIndex> entityIndex = gamePrefabsManager_.Instantiate(prefabName);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex[0]);
		transform.SetLocalPosition(position);
		transformsManager_.SetComponent(entityIndex[0], transform);

		return entityIndex[0];
	};

	poke::ICamera& camera_;
	poke::ecs::TransformsManager& transformsManager_;

	poke::game::GamePrefabsManager& gamePrefabsManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	poke::math::Vec3 playerPositionOffset = { 0.0f, 0.0f, 80.0f };
	poke::math::Vec3 destructibleElementBasePosition = { -70.0f, -20.0f, 0.0f };
	poke::math::Vec3 destructibleElementOffset = { 36.0f, 40.0f, 0.0f };
	poke::math::Vec3 destructibleElementAmount = { 5.0f };
};

class TestEnemiesSystem final : public poke::game::GameSystem {
public:
	explicit TestEnemiesSystem(poke::Engine& engine, poke::game::Game& game)
		: GameSystem(engine, game),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		colliderManager_(ecsManager_.GetComponentsManager<poke::ecs::CollidersManager>()),
		splinesManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		destructibleElementManager_(ecsManager_.GetComponentsManager<poke::game::DestructibleElementManager>()),
		weaponManager_(ecsManager_.GetComponentsManager<poke::game::WeaponManager>()),
		enemiesManager_(ecsManager_.GetComponentsManager<poke::game::EnemiesManager>()),
		playerManager_(ecsManager_.GetComponentsManager<poke::game::PlayerManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())) {
		gamePrefabsManager_.AddPrefab("Destroyer");
	}

	~TestEnemiesSystem() = default;

	void OnUpdate() {
		updateCount_++;

		if (updateCount_ >= updateLoop_) {
			engine_.Stop();
			ASSERT_TRUE(CheckTestResult());
		}
	}

	void InitTest() {
		for (size_t i = 0; i < nbEnemyX; i++) {
			for (size_t j = 0; j < nbEnemyY; j++) {
				enemies_[i + j * nbEnemyY] = CreateEnemySpline(i, j);
			}
		}
	    for (size_t i = 0; i < nbEnemyX; i++) {
			for (size_t j = 0; j < nbEnemyY; j++) {
				ecsManager_.SetActive(enemies_[i + j * nbEnemyY], poke::ecs::EntityStatus::INACTIVE);
			}
		}

		for (size_t i = 0; i < nbEnemyX; i++) {
			for (size_t j = 0; j < nbEnemyY; j++) {
				ecsManager_.SetActive(enemies_[i + j * nbEnemyX], poke::ecs::EntityStatus::ACTIVE);
			}
		}

		CreatePlayer({ 0.0f });
		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	bool CheckTestResult() const {
		return false;
	}
private:
	poke::ecs::EntityIndex CreateEnemySpline(size_t x, size_t y) {
		poke::ecs::EntityIndex entityIndex = gamePrefabsManager_.InstantiateLightFighter()[0];
		//poke::ecs::EntityIndex entityIndex = gamePrefabsManager_.InstantiateDestroyer()[0];

		poke::ecs::SplineFollower splineFollower = splinesManager_.GetComponent(entityIndex);
		for (size_t i = 0; i < pointNb; i++) {
			splineFollower.spline.AddPoint(kSplinePoints_[i] +
				poke::math::Vec3((nbEnemyX * -kDistBetweenEach_) / 2 + kDistBetweenEach_ * x, 
				(nbEnemyX * -kDistBetweenEach_) / 2 + kDistBetweenEach_ * y, 
					((i == 1 ? poke::math::Random::Get().RandomRange(0.0f, 60.0f) : 0.0f)) + 80.0f));
        }

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex);
		transform.SetLocalPosition({
			(nbEnemyX * -kDistBetweenEach_) / 2 + kDistBetweenEach_ * x,
			(nbEnemyX * -kDistBetweenEach_) / 2 + kDistBetweenEach_ * y, 
			 kOffsetFromCamera_ + 50 });

		transformsManager_.SetComponent(entityIndex, transform);

		splinesManager_.SetComponent(entityIndex, splineFollower);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::SPLINE_FOLLOWER);

		poke::game::Enemy enemy = enemiesManager_.GetComponent(entityIndex);
		enemy.approachSpeed = poke::math::Random::Get().RandomRange(kApproachMinSpeed_, kApproachMaxSpeed_);
		enemy.splineFireStartPoint = 2;
		enemy.attackSpeed = poke::math::Random::Get().RandomRange(kAttackMinSpeed_, kAttackMaxSpeed_);
		enemy.splineFireEndPoint = 6;
		enemy.fleeSpeed = poke::math::Random::Get().RandomRange(kFleeMinSpeed_, kFleeMaxSpeed_);
		enemiesManager_.SetComponent(entityIndex, enemy);

		return entityIndex;
	}

	poke::ecs::EntityIndex CreatePlayer(poke::math::Vec3 position) {
		std::vector<poke::ecs::EntityIndex> entityIndex = gamePrefabsManager_.InstantiatePlayer();

		poke::game::Player player = playerManager_.GetComponent(entityIndex[0]);
		player.movementArea = { 100.0f, 100.0f };
		playerManager_.SetComponent(entityIndex[0], player);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex[0]);
		transform.SetLocalPosition(poke::math::Vec3(0.0f, 0.0f, kOffsetFromCamera_));
		transformsManager_.SetComponent(entityIndex[0], transform);

		//ecsManager_.SetActive(entityIndex, ecs::EntityStatus::ACTIVE);

		return entityIndex[0];
	};

	poke::ecs::TransformsManager& transformsManager_;
	poke::ecs::CollidersManager& colliderManager_;
	poke::ecs::SplineFollowersManager& splinesManager_;
	poke::game::DestructibleElementManager& destructibleElementManager_;
	poke::game::WeaponManager& weaponManager_;
	poke::game::EnemiesManager& enemiesManager_;
	poke::game::PlayerManager& playerManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;

	poke::graphics::Model defaultModel_;
	poke::graphics::Model missileModel_;

	int updateLoop_ = 100000;
	int updateCount_ = 0;

	const float kApproachMinSpeed_ = 300.0f;
	const float kApproachMaxSpeed_ = 400.0f;

	const float kAttackMinSpeed_ = 20.0f;
	const float kAttackMaxSpeed_ = 40.0f;

	const float kFleeMinSpeed_ = 300.0f;
	const float kFleeMaxSpeed_ = 400.0f;

	inline static const std::size_t nbEnemyX = 5;
	inline static const std::size_t nbEnemyY = 1;
	const float kDistBetweenEach_ = 15.0f;

	const float kOffsetFromCamera_ = 80.0f;

	std::array<poke::ecs::EntityIndex, nbEnemyX * nbEnemyY> enemies_;
	inline static const size_t pointNb = 9;

	const poke::math::Vec3 kSplinePoints_[pointNb] = {
		{0.0f,  10'000.0f,   10'000.0f},
		{0.0f,   2'000.0f,    2'000.0f},
		{30.0f,     -5.0f,       90.0f},
		{0.0f,     -5.0f,       60.0f},
		{-30.0f,     -5.0f,       30.0f},
		{0.0f,     -5.0f,        0.0f},
		{0.0f,     -5.0f,      -30.0f},
		{0.0f,    -100.0f,    -1000.0f},
		{0.0f, -10'000.0f,     -100.0f},
	};
};

class TestGameCameraSystem final : public poke::game::GameSystem {
public:
	explicit TestGameCameraSystem(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		splineFollowersManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()){
		gamePrefabsManager_.AddPrefab("Player");
		gamePrefabsManager_.AddPrefab("LightFighter");
		gamePrefabsManager_.AddPrefab("DestructibleA1");
		gamePrefabsManager_.AddPrefab("DestructibleA2");
		gamePrefabsManager_.AddPrefab("DestructibleA3");
		gamePrefabsManager_.AddPrefab("DestructibleA4");
		gamePrefabsManager_.AddPrefab("DestructibleA5");
		gamePrefabsManager_.AddPrefab("DestructibleA6");

		game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	}
	~TestGameCameraSystem() = default;

	void OnUpdate() {
		poke::math::Vec3 cameraRotation = camera_.GetRotation();
		cameraRotation.x += 0.01f;
		cameraRotation.y += 0.01f;
		cameraRotation.z += 0.01f;
		camera_.SetRotation(cameraRotation);
	}
	
	void InitTest() {
		cameraSplineFollower_ = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(kCameraBasePosition_);
		transformsManager_.SetComponent(cameraSplineFollower_, transform);

		std::vector<poke::math::Vec3>	splinePoints;
		splinePoints.push_back(kCameraBasePosition_ - playerPositionOffset_);
		splinePoints.push_back(kCameraBasePosition_);
		splinePoints.push_back(kCameraBasePosition_ + playerPositionOffset_);
		splinePoints.push_back(kCameraBasePosition_ + (playerPositionOffset_ * 2));
		splinePoints.push_back(kCameraBasePosition_ + (playerPositionOffset_ * 3));
		splinePoints.push_back(kCameraBasePosition_ + (playerPositionOffset_ * 4));
		splinePoints.push_back(kCameraBasePosition_ + (playerPositionOffset_ * 5));
		splinePoints.push_back(kCameraBasePosition_ + (playerPositionOffset_ * 6));
		splinePoints.push_back(kCameraBasePosition_ + (playerPositionOffset_ * 7));

		poke::ecs::SplineFollower spline(splinePoints);
		spline.speed = 80.0f;
		splineFollowersManager_.SetComponent(cameraSplineFollower_, spline);

		poke::game::GameCamera gameCamera;
		gameCamera.playerOffset = playerPositionOffset_;
		ecsManager_.GetComponentsManager<poke::game::GameCameraManager>().SetComponent(cameraSplineFollower_, gameCamera);

		ecsManager_.AddComponent(cameraSplineFollower_,
			poke::ecs::ComponentType::TRANSFORM |
			poke::ecs::ComponentType::SPLINE_FOLLOWER |
			poke::ecs::ComponentType::GAME_CAMERA);

		camera_.SetViewMatrix(poke::math::Matrix4::LookAt(kCameraBasePosition_, kCameraBaseDirection_));
		camera_.SetPosition(kCameraBasePosition_);
		camera_.SetRotation({ 0.0f, 180.0f * poke::math::kDeg2Rad, 0.0f });
		camera_.Update();

		// Spawn player
		playerIndex_ = gamePrefabsManager_.InstantiatePlayer()[0];
		transformsManager_.SetParent(playerIndex_, cameraSplineFollower_);
		transform = transformsManager_.GetComponent(playerIndex_);
		transform.SetLocalPosition(kCameraBasePosition_ + playerPositionOffset_);
		transformsManager_.SetComponent(playerIndex_, transform);

		poke::game::Player player = ecsManager_.GetComponentsManager<poke::game::PlayerManager>().GetComponent(playerIndex_);
		player.movementArea = { 60.0f, 30.0f };
		ecsManager_.GetComponentsManager<poke::game::PlayerManager>().SetComponent(playerIndex_, player);

		// Manage the target aim sights (ID 4 on the scene)
		transformsManager_.SetParent(4, cameraSplineFollower_);
	}
private:
	poke::ecs::EntityIndex InstiatePrefabAtPosition(poke::math::Vec3 position, std::string prefabName) {
		std::vector<poke::ecs::EntityIndex> entityIndex = gamePrefabsManager_.Instantiate(prefabName);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex[0]);
		transform.SetLocalPosition(position);
		transformsManager_.SetComponent(entityIndex[0], transform);

		return entityIndex[0];
	};

	poke::ICamera& camera_;
	poke::ecs::TransformsManager& transformsManager_;

	poke::game::GamePrefabsManager& gamePrefabsManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::ecs::SplineFollowersManager& splineFollowersManager_;

	poke::ecs::EntityIndex playerIndex_ = poke::ecs::kNoEntity;
	poke::ecs::EntityIndex trailIndex_ = poke::ecs::kNoEntity;

	poke::ecs::EntityIndex cameraSplineFollower_ = poke::ecs::kNoEntity;
	
	poke::math::Vec3 playerPositionOffset_ = { 0.0f, 0.0f, 80.0f };

	const poke::math::Vec3 kCameraBasePosition_ = { 20.0f, 15.0f, -5.0f };
	const poke::math::Vec3 kCameraBaseDirection_ = { 20.0f, 15.0f, 10.0f };
};

class TestCameraSplineSystem final : public poke::game::GameSystem {
public:
	explicit TestCameraSplineSystem(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		splineFollowersManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		splineStateManager_(ecsManager_.GetComponentsManager<poke::game::SplineStateManager>()) {
		gamePrefabsManager_.AddPrefab("Player");
		gamePrefabsManager_.AddPrefab("LightFighter");
		gamePrefabsManager_.AddPrefab("DestructibleA1");
		gamePrefabsManager_.AddPrefab("DestructibleA2");
		gamePrefabsManager_.AddPrefab("DestructibleA3");
		gamePrefabsManager_.AddPrefab("DestructibleA4");
		gamePrefabsManager_.AddPrefab("DestructibleA5");
		gamePrefabsManager_.AddPrefab("DestructibleA6");
	}
	~TestCameraSplineSystem() = default;

	void InitTest() {
		cameraSplineFollower_ = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(kCameraSplinePoints_[1]);
		transform.SetLocalRotation(kCameraSplineRotation_[0] * poke::math::kDeg2Rad);
		transformsManager_.SetComponent(cameraSplineFollower_, transform);

		std::vector<poke::math::Vec3> splinePoints(kCameraSplinePoints_);

		poke::ecs::SplineFollower spline(splinePoints);
		spline.speed = 20.0f;
		splineFollowersManager_.SetComponent(cameraSplineFollower_, spline);

		poke::game::SplineStates splineStates;
		for (size_t i = 0; i < kCameraSplineRotation_.size(); i++) {
			poke::game::PointState point;
			point.pointIndex = i;
			point.rotation = kCameraSplineRotation_[i];
			point.moveSpeed = kCameraSplineMoveSpeed_[i];
			point.lerpSpeed = kCameraSplineLerpSpeed_[i];
			splineStates.pointStates.push_back(point);
		}
		splineStateManager_.SetComponent(cameraSplineFollower_, splineStates);

		poke::game::GameCamera gameCamera;
		gameCamera.playerOffset = playerPositionOffset_;
		ecsManager_.GetComponentsManager<poke::game::GameCameraManager>().SetComponent(cameraSplineFollower_, gameCamera);

		ecsManager_.AddComponent(cameraSplineFollower_,
			poke::ecs::ComponentType::TRANSFORM |
			poke::ecs::ComponentType::SPLINE_FOLLOWER |
			poke::ecs::ComponentType::SPLINE_STATES |
			poke::ecs::ComponentType::GAME_CAMERA);

		camera_.SetViewMatrix(poke::math::Matrix4::LookAt(kCameraSplinePoints_[1], kCameraSplinePoints_[2]));
		camera_.Update();

		// Spawn player
		playerIndex_ = gamePrefabsManager_.InstantiatePlayer()[0];
		transformsManager_.SetParent(playerIndex_, cameraSplineFollower_);
		transform = transformsManager_.GetComponent(playerIndex_);
		transform.SetLocalPosition(kCameraBasePosition_ + playerPositionOffset_);
		transformsManager_.SetComponent(playerIndex_, transform);

		poke::game::Player player = ecsManager_.GetComponentsManager<poke::game::PlayerManager>().GetComponent(playerIndex_);
		player.movementArea = { 60.0f, 30.0f };
		ecsManager_.GetComponentsManager<poke::game::PlayerManager>().SetComponent(playerIndex_, player);

		// Manage the target aim sights (ID 4 on the scene)
		transformsManager_.SetParent(4, cameraSplineFollower_);
	}
private:
	poke::ecs::EntityIndex InstiatePrefabAtPosition(poke::math::Vec3 position, std::string prefabName) {
		std::vector<poke::ecs::EntityIndex> entityIndex = gamePrefabsManager_.Instantiate(prefabName);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex[0]);
		transform.SetLocalPosition(position);
		transformsManager_.SetComponent(entityIndex[0], transform);

		return entityIndex[0];
	};

	poke::ICamera& camera_;
	poke::ecs::TransformsManager& transformsManager_;

	poke::game::GamePrefabsManager& gamePrefabsManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::ecs::SplineFollowersManager& splineFollowersManager_;
	poke::game::SplineStateManager& splineStateManager_;

	poke::ecs::EntityIndex playerIndex_ = poke::ecs::kNoEntity;
	poke::ecs::EntityIndex trailIndex_ = poke::ecs::kNoEntity;

	poke::ecs::EntityIndex cameraSplineFollower_ = poke::ecs::kNoEntity;

	poke::math::Vec3 playerPositionOffset_ = { 0.0f, 0.0f, 80.0f };

	const poke::math::Vec3 kCameraBasePosition_ = { 20.0f, 15.0f, -5.0f };
	const poke::math::Vec3 kCameraBaseDirection_ = { 20.0f, 15.0f, 10.0f };
	const poke::math::Vec3 kCameraBaseRotation_ = { 90.0f, 0.0f, 0.0f };
	const std::vector<poke::math::Vec3> kCameraSplinePoints_ = {
		{ 0.0f, 210.0f, 0.0f },			// 0
		{ 0.0f, 200.0f, 0.0f },			// 1
		{ 0.0f, -21.0f, 0.0f },			// 2
		{ 0.0f, -156.0f, 6.75f },			// 3
		{ 0.0f, -197.1f, 77.4f },			// 4
		{ -6.72f, -246.4f, 407.9f },		// 5
		{ -101.1f, -250.9f, 440.0f },		// 6
		{ -403.5f, -251.3f, 445.8f },		// 7
		{ -743.0f, -251.0f, 439.8f },		// 8
		{ -805.5f, -251.0f, 374.6f },		// 9
		{ -810.6f, -251.0f, 67.8f },		// 10
		{ -807.4f, -251.0f, -301.6f },		// 11
		{ -807.6f, -251.5f, -334.7f }		// 12
	};

	const std::vector<poke::math::Vec3> kCameraSplineRotation_ = {
		{ 90.0f, 0.0f, 0.0f },			// 0
		{ 90.0f, 0.0f, 0.0f },			// 1
		{ 90.0f, 0.0f, 0.0f },			// 2
		{ 60.0f, 0.0f, 0.0f },			// 3
		{ 18.0f, 0.0f, 0.0f },			// 4
		{ 0.0f, -80.0f, 0.0f },		// 5
		{ 0.0f, -90.0f, 0.0f },		// 6
		{ 0.0f, -90.0f, 0.0f },		// 7
		{ 0.0f, -150.0f, 0.0f },		// 8
		{ 0.0f, -180.0f, 0.0f },		// 9
		{ 0.0f, -180.0f, 0.0f },		// 10
		{ 0.0f, -180.0f, 0.0f },		// 11
		{ 0.0f, -180.0f, 0.0f }		// 12
	};

	const std::vector<float> kCameraSplineLerpSpeed_ = {
		0.01f,		// 0
		0.01f,		// 1
		0.01f,		// 2
		0.05f,		// 3
		0.05f,		// 4
		0.05f,		// 5
		0.05f,		// 6
		0.05f,		// 7
		0.05f,		// 8
		0.05f,		// 9	
		0.05f,		// 10
		0.05f,		// 11
		0.05f		// 12
	};
	
	const std::vector<float> kCameraSplineMoveSpeed_ = {
		20.0f,		// 0
		20.0f,		// 1
		20.0f,		// 2
		20.0f,		// 3
		20.0f,		// 4
		5.0f,		// 5
		20.0f,		// 6
		20.0f,		// 7
		20.0f,		// 8
		20.0f,		// 9
		20.0f,		// 10
		20.0f,		// 11
		20.0f		// 12
	};
};

class TestCameraSplineSystemQuaternion final : public poke::game::GameSystem {
public:
	explicit TestCameraSplineSystemQuaternion(poke::Engine& engine, poke::game::Game& game)
		: poke::game::GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		splineFollowersManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		splineStateManager_(ecsManager_.GetComponentsManager<poke::game::SplineStateManager>()) {
		gamePrefabsManager_.AddPrefab("Player");
		gamePrefabsManager_.AddPrefab("LightFighter");
		gamePrefabsManager_.AddPrefab("DestructibleA1");
		gamePrefabsManager_.AddPrefab("DestructibleA2");
		gamePrefabsManager_.AddPrefab("DestructibleA3");
		gamePrefabsManager_.AddPrefab("DestructibleA4");
		gamePrefabsManager_.AddPrefab("DestructibleA5");
		gamePrefabsManager_.AddPrefab("DestructibleA6");
		game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	}
	~TestCameraSplineSystemQuaternion() = default;

	void OnUpdate() {
		poke::math::Transform transform = transformsManager_.GetComponent(cameraSplineFollower_);
		poke::ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(cameraSplineFollower_);

		poke::math::Quaternion qLookAt = poke::math::Quaternion::LookRotation(
			transform.GetLocalPosition() - splineFollower.spline.GetPoints()[splineFollower.lastPoint + 1]);
		//splineFollower.spline.GetPoints()[splineFollower.lastPoint + 1] - transform.GetLocalPosition());

		transform.SetLocalPosition(splineFollower.spline.Lerp(splineFollower.lastPoint, splineFollower.segmentPercentage));
		transform.SetLocalRotation(qLookAt.GetEulerAngles() * poke::math::kDeg2Rad);
		transformsManager_.SetComponent(cameraSplineFollower_, transform);

		camera_.SetPosition(transform.GetLocalPosition());
		//camera_.SetRotation(qLookAt.GetEulerAngles() * poke::math::kDeg2Rad * poke::math::Vec3{ -1.0f });
		camera_.SetRotation(qLookAt.GetInverse().GetEulerAngles() * poke::math::kDeg2Rad);
		camera_.Update();
		/*poke::math::Matrix4 newViewMatrix = qLookAt.ToRotationMatrix4();

		camera_.SetViewMatrix(poke::math::Matrix4::GetInverse(newViewMatrix));*/
	}

	void InitTest() {
		cameraSplineFollower_ = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(kCameraSplinePoints_[1]);
		transform.SetLocalRotation(kCameraSplineRotation_[0] * poke::math::kDeg2Rad);
		transformsManager_.SetComponent(cameraSplineFollower_, transform);

		std::vector<poke::math::Vec3> splinePoints(kCameraSplinePoints_);

		poke::ecs::SplineFollower spline(splinePoints);
		spline.speed = 20.0f;
		splineFollowersManager_.SetComponent(cameraSplineFollower_, spline);

		ecsManager_.AddComponent(cameraSplineFollower_,
			poke::ecs::ComponentType::TRANSFORM |
			poke::ecs::ComponentType::SPLINE_FOLLOWER);

		camera_.SetViewMatrix(poke::math::Matrix4::LookAt(kCameraSplinePoints_[1], kCameraSplinePoints_[2]));
		camera_.Update();

		// Spawn player
		playerIndex_ = gamePrefabsManager_.InstantiatePlayer();
		transformsManager_.SetParent(playerIndex_[0], cameraSplineFollower_);
		transform = transformsManager_.GetComponent(playerIndex_[0]);
		transform.SetLocalPosition(kCameraBasePosition_ + playerPositionOffset_);
		transformsManager_.SetComponent(playerIndex_[0], transform);
		ecsManager_.RemoveComponent(playerIndex_[1], poke::ecs::ComponentType::GAME_CAMERA);

		poke::game::Player player = ecsManager_.GetComponentsManager<poke::game::PlayerManager>().GetComponent(playerIndex_[0]);
		player.movementArea = { 60.0f, 30.0f };
		ecsManager_.GetComponentsManager<poke::game::PlayerManager>().SetComponent(playerIndex_[0], player);

		// Manage the target aim sights (ID 4 on the scene)
		transformsManager_.SetParent(4, cameraSplineFollower_);
	}
private:
	poke::ecs::EntityIndex InstiatePrefabAtPosition(poke::math::Vec3 position, std::string prefabName) {
		std::vector<poke::ecs::EntityIndex> entityIndex = gamePrefabsManager_.Instantiate(prefabName);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex[0]);
		transform.SetLocalPosition(position);
		transformsManager_.SetComponent(entityIndex[0], transform);

		return entityIndex[0];
	};

	poke::ICamera& camera_;
	poke::ecs::TransformsManager& transformsManager_;

	poke::game::GamePrefabsManager& gamePrefabsManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::ecs::SplineFollowersManager& splineFollowersManager_;
	poke::game::SplineStateManager& splineStateManager_;

	std::vector<poke::ecs::EntityIndex> playerIndex_;
	poke::ecs::EntityIndex trailIndex_ = poke::ecs::kNoEntity;

	poke::ecs::EntityIndex cameraSplineFollower_ = poke::ecs::kNoEntity;

	poke::math::Vec3 playerPositionOffset_ = { 0.0f, 0.0f, 80.0f };

	const poke::math::Vec3 kCameraBasePosition_ = { 20.0f, 15.0f, -5.0f };
	const poke::math::Vec3 kCameraBaseDirection_ = { 20.0f, 15.0f, 10.0f };
	const poke::math::Vec3 kCameraBaseRotation_ = { 90.0f, 0.0f, 0.0f };
	const std::vector<poke::math::Vec3> kCameraSplinePoints_ = {
		{ 0.0f, 210.0f, 0.0f },			// 0
		{ 0.0f, 200.0f, 0.0f },			// 1
		{ 0.0f, -21.0f, 0.0f },			// 2
		{ 0.0f, -156.0f, 6.75f },			// 3
		{ 0.0f, -197.1f, 77.4f },			// 4
		{ -6.72f, -246.4f, 407.9f },		// 5
		{ -101.1f, -250.9f, 440.0f },		// 6
		{ -403.5f, -251.3f, 445.8f },		// 7
		{ -743.0f, -251.0f, 439.8f },		// 8
		{ -805.5f, -251.0f, 374.6f },		// 9
		{ -810.6f, -251.0f, 67.8f },		// 10
		{ -807.4f, -251.0f, -301.6f },		// 11
		{ -807.6f, -251.5f, -334.7f }		// 12
	};

	const std::vector<poke::math::Vec3> kCameraSplineRotation_ = {
		{ 90.0f, 0.0f, 0.0f },			// 0
		{ 90.0f, 0.0f, 0.0f },			// 1
		{ 90.0f, 0.0f, 0.0f },			// 2
		{ 60.0f, 0.0f, 0.0f },			// 3
		{ 18.0f, 0.0f, 0.0f },			// 4
		{ 0.0f, -80.0f, 0.0f },		// 5
		{ 0.0f, -90.0f, 0.0f },		// 6
		{ 0.0f, -90.0f, 0.0f },		// 7
		{ 0.0f, -150.0f, 0.0f },		// 8
		{ 0.0f, -180.0f, 0.0f },		// 9
		{ 0.0f, -180.0f, 0.0f },		// 10
		{ 0.0f, -180.0f, 0.0f },		// 11
		{ 0.0f, -180.0f, 0.0f }		// 12
	};

	const std::vector<float> kCameraSplineLerpSpeed_ = {
		0.01f,		// 0
		0.01f,		// 1
		0.01f,		// 2
		0.05f,		// 3
		0.05f,		// 4
		0.05f,		// 5
		0.05f,		// 6
		0.05f,		// 7
		0.05f,		// 8
		0.05f,		// 9	
		0.05f,		// 10
		0.05f,		// 11
		0.05f		// 12
	};

	const std::vector<float> kCameraSplineMoveSpeed_ = {
		20.0f,		// 0
		20.0f,		// 1
		20.0f,		// 2
		20.0f,		// 3
		20.0f,		// 4
		5.0f,		// 5
		20.0f,		// 6
		20.0f,		// 7
		20.0f,		// 8
		20.0f,		// 9
		20.0f,		// 10
		20.0f,		// 11
		20.0f		// 12
	};
};

class TestWeaponLightsSystem final : public poke::game::GameSystem {
public:
	explicit TestWeaponLightsSystem(poke::Engine& engine, poke::game::Game& game)
		: GameSystem(engine, game),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		lightsManager_(ecsManager_.GetComponentsManager<poke::ecs::LightsManager>()),
		weaponManager_(ecsManager_.GetComponentsManager<poke::game::WeaponManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {
		game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	}
	~TestWeaponLightsSystem() = default;

	void InitTest() {
		weaponIndex_ = CreateWeapon({ 0.0f });
	}

	void OnUpdate() {
		gizmoCommandBuffer_.SetColor(poke::Color::yellow);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(weaponIndex_).GetLocalPosition(), 1.0f);

		gizmoCommandBuffer_.SetColor(poke::Color::blue);
		gizmoCommandBuffer_.DrawWireSphere(transformsManager_.GetComponent(weaponIndex_).GetLocalPosition(),
			lightsManager_.GetComponent(weaponIndex_).pointLight.areaSize.x);
	}
	
private:
	poke::ecs::EntityIndex CreateWeapon(poke::math::Vec3 position) const {
		const poke::ecs::EntityIndex entityIndex = ecsManager_.AddEntity();

		poke::math::Transform transform;
		transform.SetLocalPosition(position);
		transform.SetLocalScale({ 0.01f, 0.01f, 0.01f });
		transformsManager_.SetComponent(entityIndex, transform);

		poke::game::Weapon weapon;
		weapon.gunPositions.push_back(entityIndex);
		weapon.shootCoolDown = 1.0f;
		weapon.targets.push_back(poke::ecs::kNoEntity);
		weapon.lastShootAt = -10.0f;
		weapon.weaponType = poke::game::Weapon::WeaponType::PROJECTILE_PLAYER;
		weapon.isShooting = true;
		weapon.shootDirection = { 0.0f, 0.0f, 1.0f };
		weaponManager_.SetComponent(entityIndex, weapon);

		poke::ecs::Light light;
		light.lightType = poke::graphics::LightType::POINT;
		light.pointLight.color = poke::Color::white;
		light.pointLight.areaSize = 2.0f;
		light.pointLight.intensity = 0.0f;
		lightsManager_.SetComponent(entityIndex, light);

		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::WEAPON);
		ecsManager_.AddComponent(entityIndex, poke::ecs::ComponentType::LIGHT);

		return entityIndex;
	};

	poke::ecs::TransformsManager& transformsManager_;
	poke::ecs::LightsManager& lightsManager_;
	poke::game::WeaponManager& weaponManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;

	poke::ecs::EntityIndex weaponIndex_;
};

TEST(Game, TestProjectile)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	poke::CoreCamera camera(engine);
	poke::CameraLocator::Assign(&camera);

	MovingGameCameraSystem mgcs(engine, game);
	mgcs.Init();
	mgcs.EnableCameraMovement(true);
	
	// TEST

	TestProjectileSystem testProjectileSystem(engine, game);
	testProjectileSystem.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestMissile)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	poke::CoreCamera camera(engine);
	poke::CameraLocator::Assign(&camera);
	
	MovingGameCameraSystem mgcs(engine, game);
	mgcs.Init();
	mgcs.EnableCameraMovement(true);
	
	// TEST

	TestMissileSystem testMissileSystem(engine); // TODO(@Robin) Rename it MissileSystemTest.
	testMissileSystem.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestInput)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);
	mgcs.Init();
	mgcs.EnableCameraMovement(true);
	
	// TEST

	TestInputSystem testInputSystem(engine); //TODO(@C�dric) Rename it InputManagerTest.
	testInputSystem.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestPlayer)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);
	mgcs.SetBasePosition({ 20.0f, 15.0f, -5.0f });
	mgcs.Init();

	// TEST

	TestPlayerSystem testPlayerSystem(engine, game);
	testPlayerSystem.InitTest();
	
	// TEST	

	engine.Run();
}

TEST(Game, TestDestruction)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();


	editor.StartGame();

	engine.Init();

	poke::CoreCamera camera(engine);
	poke::CameraLocator::Assign(&camera);

	MovingGameCameraSystem mgcs(engine, game);
	mgcs.Init();
	mgcs.EnableCameraMovement(true);

	// TEST

	TestDestructionSystem testDestructionSystem(engine, game);
	testDestructionSystem.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestSpecialAttack)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);
	mgcs.Init();
	mgcs.EnableCameraMovement(true);

	// TEST

	TestSpecialAttackSystem testSpecialAttack(engine, game);
	testSpecialAttack.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestDashBump)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 20.0f, 15.0f, -5.0f });
	mgcs.Init();

	// TEST

	TestDashBumpSystem testDashBumpSystem(engine, game);
	testDashBumpSystem.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestEnemy) {
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK engine"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());


	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestEnemiesSystem testEnemies(engine, game);
	testEnemies.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestGameCamera)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK Test Game Camera"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 20.0f, 15.0f, -5.0f });
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestGameCameraSystem testGameCamera(engine, game);
	testGameCamera.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestCameraSpline)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK Test Game Camera"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 20.0f, 15.0f, -5.0f });
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestCameraSplineSystem testCameraSpline(engine, game);
	testCameraSpline.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestCameraSplineQuaternion)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK Test Game Camera"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 20.0f, 15.0f, -5.0f });
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestCameraSplineSystemQuaternion testCameraSplineQuaternion(engine, game);
	testCameraSplineQuaternion.InitTest();

	// TEST	

	engine.Run();
}

TEST(Game, TestWeaponLights)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK Test Weapon Lights"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 20.0f, 15.0f, -5.0f });
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestWeaponLightsSystem testWeaponLights(engine, game);
	testWeaponLights.InitTest();

	// TEST	

	engine.Run();
}

class TestRotationSystem final : public poke::game::GameSystem {
public:
	explicit TestRotationSystem(poke::Engine& engine, poke::game::Game& game)
		: GameSystem(engine, game),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
        modelsManager_(ecsManager_.GetComponentsManager<poke::ecs::ModelsManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())) {	}

	~TestRotationSystem() = default;

	void OnUpdate() {
		updateCount_++;

		if (updateCount_ >= updateLoop_) {
			engine_.Stop();
			ASSERT_TRUE(CheckTestResult());
		}
		timer_--;
        if(timer_ <= 0) {
            if(!switchRotation_) {
				SwapRotation(firstRotation_, secondRotation_);
				switchRotation_ = true;
            }
			else {
				SwapRotation(secondRotation_, firstRotation_);
				switchRotation_ = false;
			}
			timer_ = kTimerTotal_;
        }
	}

	void InitTest() {
		SpawnObject();
		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	bool CheckTestResult() const {
		return false;
	}
private:

    void SpawnObject() {
		entityToRotate_ = ecsManager_.AddEntity();
		poke::graphics::Model model = modelsManager_.GetComponent(entityToRotate_);
		model.materialID = poke::math::HashString("reticule_neutral");
		model.meshID = poke::MeshManagerLocator::Get().GetCubeID();
		modelsManager_.SetComponent(entityToRotate_, model);
		ecsManager_.AddComponent(entityToRotate_, 
			poke::ecs::ComponentType::TRANSFORM |
			poke::ecs::ComponentType::MODEL
			);
    }

    void SwapRotation(poke::math::Vec3 oldRotation, poke::math::Vec3 newRotation) {
		poke::math::Transform transform = transformsManager_.GetComponent(entityToRotate_);
		transform.SetLocalRotation(
			poke::math::Quaternion::RotateTowards(
				poke::math::Quaternion::FromEulerRad(oldRotation * poke::math::kDeg2Rad),
				poke::math::Quaternion::FromEulerRad(newRotation * poke::math::kDeg2Rad),
				360 * poke::math::kDeg2Rad
			).GetEulerAngles()
		);
		transformsManager_.SetComponent(entityToRotate_, transform);
    }

	poke::ecs::TransformsManager& transformsManager_;
	poke::ecs::ModelsManager& modelsManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;

	poke::graphics::Model defaultModel_;
	poke::graphics::Model missileModel_;

	int updateLoop_ = 100000;
	int updateCount_ = 0;

	poke::math::Vec3 firstRotation_ =  {  0.0f, 0.0f, 0.0f };
	poke::math::Vec3 secondRotation_ = { 45.0f, 0.0f, 0.0f };

	bool switchRotation_ = false;
	int timer_ = 180;
	const int kTimerTotal_ = 180;

	poke::ecs::EntityIndex entityToRotate_;

};


TEST(Game, TestRotation)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK Test Rotation"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 0.0f, 0.0f, -20.0f });
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestRotationSystem testRotationSystem(engine, game);
	testRotationSystem.InitTest();

	// TEST	

	engine.Run();
}


class TestAimingSystem final : public poke::game::GameSystem {
public:
	explicit TestAimingSystem(poke::Engine& engine, poke::game::Game& game)
		: GameSystem(engine, game),
		transformsManager_(ecsManager_.GetComponentsManager<poke::ecs::TransformsManager>()),
		colliderManager_(ecsManager_.GetComponentsManager<poke::ecs::CollidersManager>()),
		splinesManager_(ecsManager_.GetComponentsManager<poke::ecs::SplineFollowersManager>()),
		destructibleElementManager_(ecsManager_.GetComponentsManager<poke::game::DestructibleElementManager>()),
		weaponManager_(ecsManager_.GetComponentsManager<poke::game::WeaponManager>()),
		enemiesManager_(ecsManager_.GetComponentsManager<poke::game::EnemiesManager>()),
		playerManager_(ecsManager_.GetComponentsManager<poke::game::PlayerManager>()),
		gizmoCommandBuffer_(poke::GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		gamePrefabsManager_(static_cast<poke::game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get())) {
		gamePrefabsManager_.AddPrefab("Destroyer");
	}

	~TestAimingSystem() = default;

	void OnUpdate() {
		updateCount_++;

		if (updateCount_ >= updateLoop_) {
			engine_.Stop();
			ASSERT_TRUE(CheckTestResult());
		}
	}

	void InitTest() {
		for (size_t i = 0; i < nbEnemyX; i++) {
			for (size_t j = 0; j < nbEnemyY; j++) {
				enemies_[i + j * nbEnemyY] = CreateEnemy(i, j);
			}
		}

		CreatePlayer({ 0.0f });
		engine_.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() { OnUpdate(); });
	}

	bool CheckTestResult() const {
		return false;
	}
private:
	poke::ecs::EntityIndex CreateEnemy(size_t x, size_t y) {
		poke::ecs::EntityIndex entityIndex = gamePrefabsManager_.InstantiateLightFighter()[0];

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex);
		transform.SetLocalPosition({
			(nbEnemyX * -kDistBetweenEach_) / 2 + kDistBetweenEach_ * x,
			(nbEnemyX * -kDistBetweenEach_) / 2 + kDistBetweenEach_ * y,
			 kOffsetFromCamera_ + 50 });

		transformsManager_.SetComponent(entityIndex, transform);
        
		ecsManager_.RemoveComponent(entityIndex, poke::ecs::ComponentType::SPLINE_FOLLOWER);

		return entityIndex;
	}

	poke::ecs::EntityIndex CreatePlayer(poke::math::Vec3 position) {
		poke::ecs::EntityIndex entityIndex = gamePrefabsManager_.InstantiatePlayer()[1];

		poke::game::Player player = playerManager_.GetComponent(entityIndex);
		player.movementArea = { 100.0f, 100.0f };
		playerManager_.SetComponent(entityIndex, player);

		poke::math::Transform transform = transformsManager_.GetComponent(entityIndex);
		transform.SetLocalPosition(poke::math::Vec3(0.0f, 0.0f, kOffsetFromCamera_));
		transformsManager_.SetComponent(entityIndex, transform);


		return entityIndex;
	};

	poke::ecs::TransformsManager& transformsManager_;
	poke::ecs::CollidersManager& colliderManager_;
	poke::ecs::SplineFollowersManager& splinesManager_;
	poke::game::DestructibleElementManager& destructibleElementManager_;
	poke::game::WeaponManager& weaponManager_;
	poke::game::EnemiesManager& enemiesManager_;
	poke::game::PlayerManager& playerManager_;
	poke::graphics::GizmoCommandBuffer& gizmoCommandBuffer_;
	poke::game::GamePrefabsManager& gamePrefabsManager_;

	poke::graphics::Model defaultModel_;
	poke::graphics::Model missileModel_;

	int updateLoop_ = 100000;
	int updateCount_ = 0;

	inline static const std::size_t nbEnemyX = 5;
	inline static const std::size_t nbEnemyY = 5;
	const float kDistBetweenEach_ = 15.0f;

	const float kOffsetFromCamera_ = 80.0f;

	std::array<poke::ecs::EntityIndex, nbEnemyX * nbEnemyY> enemies_;
};

TEST(Game, TestAiming)
{
	const char* projectName = "pokEngine";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::EDITOR,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK Test Rotation"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	poke::game::Game& game = editor.GetGame();

	editor.StartGame();

	engine.Init();

	MovingGameCameraSystem mgcs(engine, game);

	mgcs.SetBasePosition({ 0.0f, 0.0f, -20.0f });
	mgcs.EnableCameraMovement(false);
	mgcs.Init();

	// TEST

	TestAimingSystem testAimingSystem(engine, game);
	testAimingSystem.InitTest();

	// TEST	

	engine.Run();
}