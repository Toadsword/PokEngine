#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/log.h>

class PhysicsTriggerUnitTest {
public:
    PhysicsTriggerUnitTest(poke::Engine& engine) : engine_(engine) { }

    void OnInit()
    {
        using namespace poke;
        auto& ecsManager = EcsManagerLocator::Get();

        TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
        std::string materialName = "metal_test";
        auto materialHash = XXH64(
            &materialName[0],
            materialName.size(),
            math::kHashSeed);
        MaterialsManagerLocator::Get().AddMaterial(materialName);

        auto& modelsManager =
            ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

        auto& transformsManager =
            ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

        auto& collidersManager =
            ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

        //Entity 1
        {
            entity0_ = ecsManager.AddEntity();

			ecsManager.RegisterObserverTriggerEnter(
                entity0_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnTriggerEnter(entityIndex, collision);
                });

            ecsManager.AddComponent(entity0_, ecs::ComponentType::MODEL);

            const graphics::Model model(
                materialHash,
                MeshManagerLocator::Get().GetSphereID());
            modelsManager.SetComponent(entity0_, model);

            ecsManager.AddComponent(entity0_, ecs::ComponentType::TRANSFORM);
            auto transform = transformsManager.GetComponent(entity0_);
            transform.SetLocalPosition(math::Vec3(0, 0, 0));
            transformsManager.SetComponent(entity0_, transform);

            //Rigidbody objects
            ecsManager.AddComponent(entity0_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity0_, rigidbody);

            const physics::BoxShape boxShape(
                {0, 0, 0},
                {1, 1, 1});

            ecsManager.AddComponent(entity0_, ecs::ComponentType::COLLIDER);
            physics::Collider collider;
            collider.isTrigger = true;
            collider.SetShape(boxShape);
            collidersManager.SetComponent(entity0_, collider);
        }

        //Entity 1
        {
            entity1_ = ecsManager.AddEntity();

			ecsManager.RegisterObserverTriggerEnter(
                entity1_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnTriggerEnter(entityIndex, collision);
                });

            ecsManager.AddComponent(entity1_, ecs::ComponentType::MODEL);

            const graphics::Model model(
                materialHash,
                MeshManagerLocator::Get().GetSphereID());
            modelsManager.SetComponent(entity1_, model);

            ecsManager.AddComponent(entity1_, ecs::ComponentType::TRANSFORM);
            auto transform = transformsManager.GetComponent(entity1_);
            transform.SetLocalPosition(math::Vec3(0.5f, 0.0f, 0.0f));
            transformsManager.SetComponent(entity1_, transform);

            //Colliders objects
            ecsManager.AddComponent(entity1_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity1_, rigidbody);

            const physics::BoxShape boxShape(
                {0, 0, 0},
                {1, 1, 1});

            ecsManager.AddComponent(entity1_, ecs::ComponentType::COLLIDER);
            physics::Collider collider;
            collider.isTrigger = true;
            collider.SetShape(boxShape);
            collidersManager.SetComponent(entity1_, collider);
        }
    }

    void OnUpdate()
    {
        count_++;

		if (count_ == 10) { engine_.Stop(); }
    }

    void OnTriggerEnter(const poke::ecs::EntityIndex entityIndex, const poke::physics::Collision collision)
    {
        if (entityIndex == entity0_ && collision.otherEntity == entity1_) {
			trigger0_ = true;
        }

		if (entityIndex == entity1_ && collision.otherEntity == entity0_) {
			trigger1_ = true;
		}
    
    }

    bool HasSucceed() const
    {
		return trigger0_ && trigger1_;
    }

private:
    int count_ = 0;

    poke::Engine& engine_;

    poke::ecs::EntityIndex entity0_;
    poke::ecs::EntityIndex entity1_;

    bool trigger0_ = false;
    bool trigger1_ = false;
};

TEST(Physics, Triggers)
{
    poke::EngineSetting engineSettings{
		"testPhysicsTriggers",
        poke::AppType::EDITOR,
        std::chrono::duration<double, std::milli>(16.66f),
        720,
        640,
        "POK engine",
        {{0, "Default", "Default"}}
    };

    poke::Engine engine(engineSettings);

    //Load editor application
    engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

    //Load editor graphics renderer
    engine.GetModuleManager().graphicsEngine.SetRenderer(
        std::make_unique<poke::graphics::RendererEditor>(engine));

    // TEST

    PhysicsTriggerUnitTest emptyChunkTest(engine);
    engine.AddObserver(
        poke::observer::MainLoopSubject::ENGINE_INIT,
        [&]() { emptyChunkTest.OnInit(); });
    engine.AddObserver(
        poke::observer::MainLoopSubject::UPDATE,
        [&]() { emptyChunkTest.OnUpdate(); });
    //

    engine.Init();


    engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}

class PhysicsColliderInAndOut {
public:
	PhysicsColliderInAndOut(poke::Engine& engine) : engine_(engine) { }

	void OnInit()
	{
		using namespace poke;

		auto& ecsManager = EcsManagerLocator::Get();

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);
		MaterialsManagerLocator::Get().AddMaterial(materialName);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		{
			entity0_ = ecsManager.AddEntity();

			ecsManager.RegisterObserverColliderEnter(
                entity0_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderEnter(entityIndex, collision);
                });

			ecsManager.RegisterObserverColliderExit(
                entity0_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderExit(entityIndex, collision);
                });

			ecsManager.AddComponent(entity0_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity0_, model);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity0_);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformsManager.SetComponent(entity0_, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity0_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity0_, rigidbody);

			const physics::BoxShape boxShape(
				{ 0,0,0 },
				1.0f);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity0_, collider);
		}

		//Entity 1
		{
			entity1_ = ecsManager.AddEntity();

			ecsManager.RegisterObserverColliderEnter(
                entity1_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderEnter(entityIndex, collision);
                });

			ecsManager.RegisterObserverColliderExit(
                entity1_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderExit(entityIndex, collision);
                });

			ecsManager.AddComponent(entity1_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity1_, model);

			ecsManager.AddComponent(entity1_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity1_);
			transform.SetLocalPosition(math::Vec3(0.5f, 0.0f, 0.0f));
			transformsManager.SetComponent(entity1_, transform);

			//Colliders objects
			ecsManager.AddComponent(entity1_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbody.linearVelocity = math::Vec3(1.0f, 0, 0);
			rigidbodiesManager.SetComponent(entity1_, rigidbody);

			const physics::BoxShape boxShape(
				{0,0,0},
				1.0f);

			ecsManager.AddComponent(entity1_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity1_, collider);
		}
	}

	void OnUpdate()
	{
		count_++;

		if (count_ == 100000000) { engine_.Stop(); }
	}

	void OnColliderEnter(poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision)
	{
		if (entityIndex == entity0_ && collision.otherEntity == entity1_) {
			colliderIn0_ = true;
		}

		if (entityIndex == entity1_ && collision.otherEntity == entity0_) {
			colliderIn1_ = true;
		}
	}

	void OnColliderExit(poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision)
	{
		if (entityIndex == entity0_ && collision.otherEntity == entity1_) {
			colliderOut0_ = true;
		}

		if (entityIndex == entity1_ && collision.otherEntity == entity0_) {
			colliderOut1_ = true;
		}
	}

	bool HasSucceed()
	{
		return colliderIn0_ && colliderOut0_ && colliderIn1_ && colliderOut1_;
	}

private:
	int count_ = 0;

	poke::Engine& engine_;

	poke::ecs::EntityIndex entity0_;
	poke::ecs::EntityIndex entity1_;

	bool colliderIn0_ = false;
	bool colliderIn1_ = false;

	bool colliderOut0_ = false;
	bool colliderOut1_ = false;
};

TEST(Physics, Colliders)
{
	poke::EngineSetting engineSettings{
		"testPhysicsColliders",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PhysicsColliderInAndOut emptyChunkTest(engine);
	engine.AddObserver(
        poke::observer::MainLoopSubject::ENGINE_INIT,
        [&]() { emptyChunkTest.OnInit(); });
	engine.AddObserver(
        poke::observer::MainLoopSubject::UPDATE,
        [&]() { emptyChunkTest.OnUpdate(); });
	//

	engine.Init();


	engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}

class PhysicsObjectDestruction {
public:
	PhysicsObjectDestruction(poke::Engine& engine) : engine_(engine) { }

	void OnInit()
	{
		using namespace poke;

		auto& ecsManager = EcsManagerLocator::Get();

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);
		MaterialsManagerLocator::Get().AddMaterial(materialName);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		{
			entity0_ = ecsManager.AddEntity();

			ecsManager.RegisterObserverColliderEnter(
                entity0_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderEnter(entityIndex, collision);
                });

			ecsManager.RegisterObserverColliderExit(
                entity0_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderExit(entityIndex, collision);
                });

			ecsManager.AddComponent(entity0_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity0_, model);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity0_);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformsManager.SetComponent(entity0_, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity0_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity0_, rigidbody);

			const physics::BoxShape boxShape(
				{ 0,0,0 },
				1.0f);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity0_, collider);
		}

		//Entity 1
		{
			entity1_ = ecsManager.AddEntity();

			ecsManager.RegisterObserverColliderEnter(
                entity1_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderEnter(entityIndex, collision);
                });

			ecsManager.RegisterObserverColliderExit(
                entity1_,
                [this](ecs::EntityIndex entityIndex, physics::Collision collision) {
                    OnColliderExit(entityIndex, collision);
                });

			ecsManager.AddComponent(entity1_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity1_, model);

			ecsManager.AddComponent(entity1_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity1_);
			transform.SetLocalPosition(math::Vec3(0.5f, 0.0f, 0.0f));
			transformsManager.SetComponent(entity1_, transform);

			//Colliders objects
			ecsManager.AddComponent(entity1_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbody.linearVelocity = math::Vec3(1.0f, 0, 0);
			rigidbodiesManager.SetComponent(entity1_, rigidbody);

			const physics::BoxShape boxShape(
				{ 0,0,0 },
				1.0f);

			ecsManager.AddComponent(entity1_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity1_, collider);
		}
	}

	void OnUpdate()
	{
		count_++;

		if (count_ == 100) { engine_.Stop(); }
	}

	void OnColliderEnter(poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision)
	{
		if (entityIndex == entity0_ && collision.otherEntity == entity1_) {
			colliderIn0_ = true;
		}

		if (entityIndex == entity1_ && collision.otherEntity == entity0_) {
			colliderIn1_ = true;
		}
	}

	void OnColliderExit(poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision)
	{
		if (entityIndex == entity0_) {
			poke::EcsManagerLocator::Get().DestroyEntity(entity0_);
			entityDestroy0_ = true;
		}
	}

	bool HasSucceed()
	{
		return colliderIn0_ && entityDestroy0_ && colliderIn1_;
	}

private:
	int count_ = 0;

	poke::Engine& engine_;

	poke::ecs::EntityIndex entity0_;
	poke::ecs::EntityIndex entity1_;

	bool colliderIn0_ = false;
	bool colliderIn1_ = false;

	bool entityDestroy0_ = false;
};

TEST(Physics, EntityDestroy)
{
	poke::EngineSetting engineSettings{
		"testPhysicsEntityDestroy",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PhysicsObjectDestruction emptyChunkTest(engine);
	engine.AddObserver(
        poke::observer::MainLoopSubject::ENGINE_INIT,
        [&]() { emptyChunkTest.OnInit(); });
	engine.AddObserver(
        poke::observer::MainLoopSubject::UPDATE,
        [&]() { emptyChunkTest.OnUpdate(); });
	//

	engine.Init();

	engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}

class PhysicsRaycast {
public:
	PhysicsRaycast(poke::Engine& engine) : engine_(engine) { }

	void OnInit()
	{
		using namespace poke;

		auto& ecsManager = EcsManagerLocator::Get();

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);
		MaterialsManagerLocator::Get().AddMaterial(materialName);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		{
			entity0_ = ecsManager.AddEntity();

			ecsManager.AddComponent(entity0_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity0_, model);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity0_);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformsManager.SetComponent(entity0_, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity0_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity0_, rigidbody);

			const physics::BoxShape boxShape(
				{ 0,0,0 },
				1.0f);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity0_, collider);
		}

		//Entity 1
		{
			entity1_ = ecsManager.AddEntity();

			ecsManager.AddComponent(entity1_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity1_, model);

			ecsManager.AddComponent(entity1_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity1_);
			transform.SetLocalPosition(math::Vec3(0.5f, 0.0f, 0.0f));
			transformsManager.SetComponent(entity1_, transform);

			//Colliders objects
			ecsManager.AddComponent(entity1_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbody.linearVelocity = math::Vec3(1.0f, 0, 0);
			rigidbodiesManager.SetComponent(entity1_, rigidbody);

			const physics::BoxShape boxShape(
				{ 0,0,0 },
				1.0f);

			ecsManager.AddComponent(entity1_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity1_, collider);
		}
	}

	void OnUpdate()
	{
		count_++;

		auto entities = poke::PhysicsEngineLocator::Get().Raycast({ 0, 0, 0 }, { 10, 0, 0 });

        for (auto entity : entities) {
			if(entity == entity0_) {
				hit0_ = true;
			}

            if(entity == entity1_) {
				hit1_ = true;
            }
        }

		if (count_ == 10) { engine_.Stop(); }
	}

	bool HasSucceed()
	{
		return hit0_ && hit1_;
	}

private:
	int count_ = 0;

	poke::Engine& engine_;

	poke::ecs::EntityIndex entity0_;
	poke::ecs::EntityIndex entity1_;

	bool hit0_ = false;
	bool hit1_ = false;
};

TEST(Physics, Raycast)
{
	poke::EngineSetting engineSettings{
		"testPhysicsRaycast",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PhysicsRaycast emptyChunkTest(engine);
	engine.AddObserver(
        poke::observer::MainLoopSubject::ENGINE_INIT,
        [&]() { emptyChunkTest.OnInit(); });
	engine.AddObserver(
        poke::observer::MainLoopSubject::UPDATE,
        [&]() { emptyChunkTest.OnUpdate(); });
	//

	engine.Init();

	engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}

class PhysicsDestructionObjects {
public:
	PhysicsDestructionObjects(poke::Engine& engine) : engine_(engine) { }

    void OnEngineInit()
	{
		using namespace poke;

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");

		std::string materialName = "metal_test";
		MaterialsManagerLocator::Get().AddMaterial(materialName);
	    
	}

	void OnUpdate()
	{
		using namespace poke;

		auto& ecsManager = EcsManagerLocator::Get();

		std::string materialName = "metal_test";
		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		if(entities_.size() < 10){
		    auto entity = ecsManager.AddEntity();

			ecsManager.AddComponent(entity, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity, model);

			ecsManager.AddComponent(entity, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity);
			const auto randX = math::Random::Get().RandomRange(-50, 50);
			const auto randY = math::Random::Get().RandomRange(-50, 50);
			const auto randZ = math::Random::Get().RandomRange(-50, 50);
			transform.SetLocalPosition(math::Vec3(randX, randY, randZ));
			transformsManager.SetComponent(entity, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity, rigidbody);

			const physics::BoxShape boxShape({ 0,0,0 }, 1.0f);

			ecsManager.AddComponent(entity, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity, collider);

			entities_.push_back(entity);
		}

		static int i = 0;

		i++;

		if (i % 20 == 0) {
			std::sort(entities_.begin(), entities_.end());
			const auto rand = math::Random::Get().RandomRange(entities_[0], entities_.back());
			ecsManager.DestroyEntity(rand);
		}

		count_++;

        if(count_ == 100) {
			engine_.Stop();
        }
	}

	bool HasSucceed()
	{
		return true;
	}

private:
	int count_ = 0;

	poke::Engine& engine_;

	std::vector<poke::ecs::EntityIndex> entities_;
};

TEST(Physics, DestroyRandom)
{

	poke::EngineSetting engineSettings{
		"testPhysicsDestroyRandom",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PhysicsDestructionObjects emptyChunkTest(engine);
	engine.AddObserver(
        poke::observer::MainLoopSubject::ENGINE_INIT,
        [&]() { emptyChunkTest.OnEngineInit(); });

	engine.AddObserver(
        poke::observer::MainLoopSubject::UPDATE,
        [&]() { emptyChunkTest.OnUpdate(); });
	//

	engine.Init();

	engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}

class PhysicsMultipleComponent {
public:
	PhysicsMultipleComponent(poke::Engine& engine) : engine_(engine) { }

	void OnEngineInit()
	{
		using namespace poke;

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");

		std::string materialName = "metal_test";
		MaterialsManagerLocator::Get().AddMaterial(materialName);
		auto& ecsManager = EcsManagerLocator::Get();

		ecsManager.RegisterObserverAddComponent(
            [this](poke::ecs::EntityIndex entityIndex, poke::ecs::ComponentMask component) {
                this->OnAddComponent(entityIndex, component);
            }
        );

		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		{
			auto entity = ecsManager.AddEntity();

			ecsManager.AddComponent(entity, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity, model);

			ecsManager.AddComponent(entity, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformsManager.SetComponent(entity, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity, rigidbody);

			const physics::BoxShape boxShape({ 0,0,0 }, 1.0f);

			ecsManager.AddComponent(entity, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = true;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity, collider);

			entities_.push_back(entity);
			ecsManager.AddComponent(entity, ecs::ComponentType::LIGHT);
		}

		//Entity 0
		{
			auto entity = ecsManager.AddEntity();

			ecsManager.AddComponent(entity, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity, model);

			ecsManager.AddComponent(entity, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformsManager.SetComponent(entity, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity, rigidbody);

			const physics::BoxShape boxShape({ 0,0,0 }, 1.0f);

			ecsManager.AddComponent(entity, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = true;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity, collider);

			entities_.push_back(entity);
			ecsManager.AddComponent(entity, ecs::ComponentType::LIGHT);
		}

	}

	void OnUpdate()
	{
		using namespace poke;

		count_++;

		if (count_ == 10) {
			engine_.Stop();
		}
	}

    void OnAddComponent(poke::ecs::EntityIndex entityIndex, poke::ecs::ComponentMask component)
	{
	    if(component == poke::ecs::ComponentType::LIGHT) {
			poke::EcsManagerLocator::Get().RegisterObserverTriggerEnter(entityIndex, [this](poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision) {OnTriggerEnter(entityIndex, collision); });
	    }

        /*if(component == poke::ecs::ComponentType::LIGHT) {
			poke::EcsManager::Get().RegisterObserverTriggerEnter(entityIndex, [this](poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision) {OnTriggerEnter(entityIndex, collision); });
		}*/
	}

    void OnTriggerEnter(poke::ecs::EntityIndex entityIndex, poke::physics::Collision collision)
	{
	    if(entityIndex == entities_[0]) {
			test0_ = true;
	    }

		if (entityIndex == entities_[1]) {
			test1_ = true;
		}
	}

	bool HasSucceed()
	{
		return test0_ && test1_;
	}

private:
	int count_ = 0;

	poke::Engine& engine_;

	std::vector<poke::ecs::EntityIndex> entities_;

	bool test0_ = false , test1_ = false;
};

TEST(Physics, DifferentComponent)
{
	poke::EngineSetting engineSettings{
		"testPhysicsDifferentComponent",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

 	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PhysicsMultipleComponent emptyChunkTest(engine);
	engine.AddObserver(
        poke::observer::MainLoopSubject::ENGINE_INIT,
        [&]() { emptyChunkTest.OnEngineInit(); });

	engine.AddObserver(
        poke::observer::MainLoopSubject::UPDATE,
        [&]() { emptyChunkTest.OnUpdate(); });
	//

	engine.Init();

	engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}

class PhysicsMeshShapeForCollider {
public:
	PhysicsMeshShapeForCollider(poke::Engine& engine) : engine_(engine) { }

	void OnEngineInit()
	{
		using namespace poke;

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");

		std::string materialName = "metal_test";
		MaterialsManagerLocator::Get().AddMaterial(materialName);
		auto& ecsManager = EcsManagerLocator::Get();

		std::string meshName = "msh_astero1_low.obj";
		MeshManagerLocator::Get().AddMesh(meshName);
        auto meshHash = XXH64(
			&meshName[0],
			meshName.size(),
			math::kHashSeed);

		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		{
			auto entity = ecsManager.AddEntity();

            //Model
			const graphics::Model model(
				materialHash,
				meshHash);
			modelsManager.SetComponent(entity, model);

            //Transform
			auto transform = transformsManager.GetComponent(entity);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transform.SetLocalScale(math::Vec3(0.1f, 0.1f, 0.1f));
			transformsManager.SetComponent(entity, transform);

		    //Rigidbody objects
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity, rigidbody);

            //Collider
			const physics::MeshShape meshShape(MeshManagerLocator::Get().GetMesh(meshHash));

			physics::Collider collider;
			collider.isTrigger = true;
			collider.SetShape(meshShape);

			collidersManager.SetComponent(entity, collider);

			ecsManager.AddComponent(entity, ecs::ComponentType::TRANSFORM |
                                            ecs::ComponentType::MODEL |
				                            ecs::ComponentType::RIGIDBODY |
				                            ecs::ComponentType::COLLIDER 
			);

			entities_.push_back(entity);
		}
  	}

	void OnUpdate()
	{
		using namespace poke;

		count_++;

		if (count_ == 10) {
			engine_.Stop();
		}
	}

	bool HasSucceed()
	{
		return true;
	}

private:
	int count_ = 0;

	poke::Engine& engine_;

	std::vector<poke::ecs::EntityIndex> entities_;

	bool test0_ = false, test1_ = false;
};

TEST(Physics, MeshShapeForCollider)
{
	poke::EngineSetting engineSettings{
		"testPhysicsMeshShapeForCollider",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PhysicsMeshShapeForCollider emptyChunkTest(engine);
	engine.AddObserver(
		poke::observer::MainLoopSubject::ENGINE_INIT,
		[&]() { emptyChunkTest.OnEngineInit(); });

	engine.AddObserver(
		poke::observer::MainLoopSubject::UPDATE,
		[&]() { emptyChunkTest.OnUpdate(); });
	//

	engine.Init();

	engine.Run();

	ASSERT_TRUE(emptyChunkTest.HasSucceed());
}