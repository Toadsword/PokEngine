#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include "Ecs/Utility/entity_vector.h"
#include <algorithm>

//---------------------------------Add/Remove Entity --------------------------
TEST(ECS, AddRemoveEntity1)
{
    poke::EngineSetting engineSettings{
		"testECSAddRemoveEntity1",
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

    engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		static int count = 0;
		count++;
        if(count > 10) {
			engine.Stop();
        }
	});

	auto& escManager = poke::EcsManagerLocator::Get();
    poke::ecs::EntityIndex current = escManager.AddEntity();
	escManager.DestroyEntity(current);

	ASSERT_TRUE(true);
}

TEST(ECS, AddRemoveEntity1000)
{
    poke::EngineSetting engineSettings{
		"testECSAddRemoveEntity1000",
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

    engine.Init();


    std::vector<poke::ecs::EntityIndex> createdEntities;
    createdEntities.resize(1000);
    // TEST
    for (int i = 0; i < 1000; i++) {
        createdEntities[i] = poke::EcsManagerLocator::Get().AddEntity();
    }
    for (unsigned int i = 0; i < 1000; i++) {
        poke::EcsManagerLocator::Get().DestroyEntity(createdEntities[i]);
    }
    //TEST

	ASSERT_TRUE(true);
}

TEST(ECS, AddRemoveEntity10000)
{
	poke::EngineSetting engineSettings{
		"testECSAddRemoveEntity10000",
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

	engine.Init();

	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		static int count = 0;
		count++;
		if (count > 10) {
			engine.Stop();
		}
	});

	const size_t nbEntities = 10'000;
	std::vector<poke::ecs::EntityIndex> createdEntities;
	createdEntities.resize(nbEntities);
	// TEST
	for (size_t i = 0; i < nbEntities; i++) {
		createdEntities[i] = poke::EcsManagerLocator::Get().AddEntity();

		ASSERT_TRUE(createdEntities[i] < nbEntities);
	}

	poke::EcsManagerLocator::Get().DestroyEntities(createdEntities);
	// TEST

	engine.Run();

	ASSERT_TRUE(true);
}
//-----------------------------------------------------------------------------

//---------------------------------Add/Remove Components-----------------------
TEST(ECS, Add1EntityWith1Transform)
{
	poke::EngineSetting engineSettings{
		"testECSAddEntity1Transform",
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

	poke::ecs::EntityIndex current = poke::EcsManagerLocator::Get().AddEntity();
	poke::EcsManagerLocator::Get().AddComponent(current, poke::ecs::ComponentType::TRANSFORM);
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(current, poke::ecs::ComponentType::TRANSFORM));
}

TEST(ECS, AddRemove1EntityWith1Transform)
{
	poke::EngineSetting engineSettings{
		"testECSAddRemoveEntity1Transform",
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

	engine.Init();

	poke::ecs::EntityIndex current = poke::EcsManagerLocator::Get().AddEntity();
	poke::EcsManagerLocator::Get().AddComponent(current, poke::ecs::ComponentType::TRANSFORM);
	poke::EcsManagerLocator::Get().RemoveComponent(current, poke::ecs::ComponentType::TRANSFORM);
	ASSERT_TRUE(!poke::EcsManagerLocator::Get().HasComponent(current, poke::ecs::ComponentType::TRANSFORM));
}

TEST(ECS, Add1EntityWith2Transforms)
{
	poke::EngineSetting engineSettings{
		"testECSAddEntity2Transform",
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

	engine.Init();

	poke::ecs::EntityIndex current = poke::EcsManagerLocator::Get().AddEntity();
	ASSERT_TRUE(!poke::EcsManagerLocator::Get().HasComponent(current, poke::ecs::ComponentType::TRANSFORM));
	poke::EcsManagerLocator::Get().AddComponent(current, poke::ecs::ComponentType::TRANSFORM);
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(current, poke::ecs::ComponentType::TRANSFORM));
	poke::EcsManagerLocator::Get().AddComponent(current, poke::ecs::ComponentType::TRANSFORM);
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(current, poke::ecs::ComponentType::TRANSFORM));
}
//-----------------------------------------------------------------------------

//----------------------------------- ARCHETYPE -------------------------------
TEST(ECS, Add1Archetype)
{

	poke::EngineSetting engineSettings{
		"testECSAddOneArchetype",
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
	engine.Init();

	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", 100);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

    for(int i = 0; i < 100; i++) {
        const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = 
			poke::EcsManagerLocator::Get().
                GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(
		    entityIndex, 
			poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
    }
}

TEST(ECS, Add2Archetypes)
{
	poke::EngineSetting engineSettings{
		"testECSAddTwoArchetype",
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
	engine.Init();

	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });
	const int sizeArchetype = 100;
	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", sizeArchetype);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < sizeArchetype; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
	}

	poke::ecs::Archetype tmpArchetype2;
	tmpArchetype2.AddComponent(poke::ecs::ComponentType::TRANSFORM);

	tmpArchetype2.AddComponent(poke::ecs::ComponentType::RIGIDBODY);;

	const int sizeArchetype2 = 200;

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype2, "test2", sizeArchetype2);

	const auto archetype2ID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test2");

	for (int i = 0; i < sizeArchetype2; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetype2ID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::RIGIDBODY));
	}
}

TEST(ECS, Add1ArchetypeHalfFiled)
{
	poke::EngineSetting engineSettings{
		"testECSAddOneArchetypeHalfFiled",
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
	engine.Init();

	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	const int size = 100;
	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", size);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < size / 2; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
	}

	auto pools = poke::EcsManagerLocator::Get().GetPools();

    for(int i = 0; i < size / 2; i++) {
		ASSERT_TRUE(poke::EcsManagerLocator::Get().IsEntityActive(pools[1].firstEntity + i));
    }

	for (int i = size / 2; i < size; i++) {
		ASSERT_FALSE(poke::EcsManagerLocator::Get().IsEntityActive(pools[1].firstEntity + i));
	}
}

TEST(ECS, UpdateArchetypesAddComponent)
{
	poke::EngineSetting engineSettings{
		"testECSUpdateArchetypesAddComponent",
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
	engine.Init();

	const auto size = 100;
	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.size = size;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", size);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < size; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
		ASSERT_FALSE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY));
	}

	const int frameNmbUpdateArchetype = 5;
	const int maxEngineFrameNmb = 10;

	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		static int count = 0;
		count++;

        if(count == frameNmbUpdateArchetype) {
			tmpArchetype.AddComponent(poke::ecs::ComponentType::RIGIDBODY);
			poke::ArchetypesManagerLocator::Get().UpdateArchetype(archetypeID, tmpArchetype);

			const auto pool = poke::EcsManagerLocator::Get().GetPools()[archetypeID];

            for (poke::ecs::EntityIndex i = pool.firstEntity; i < pool.lastEntity; i++){
				ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(i, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER | poke::ecs::ComponentType::RIGIDBODY));
            }
        }

		if (count > maxEngineFrameNmb) {
			engine.Stop();
		}
	});

	engine.Run();

	ASSERT_TRUE(true);
}

TEST(ECS, UpdateArchetypesAddAndRemoveComponent)
{
	poke::EngineSetting engineSettings{
		"testECSUpdateArchetypesAddAndRemoveComponent",
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
	engine.Init();

	const int size = 100;
	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.size = size;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", size);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < size; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
		ASSERT_FALSE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY));
	}

	const int maxEngineFrameNmb = 10;
	const int frameUpdateArchetype = 5;
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		static int count = 0;
		count++;

		if (count == frameUpdateArchetype) {
			tmpArchetype.AddComponent(poke::ecs::ComponentType::RIGIDBODY);
			poke::ArchetypesManagerLocator::Get().UpdateArchetype(archetypeID, tmpArchetype);

			auto pool = poke::EcsManagerLocator::Get().GetPools()[archetypeID];

			for (size_t i = pool.firstEntity; i < pool.lastEntity; i++) {
				ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(i, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER | poke::ecs::ComponentType::RIGIDBODY));
			}
		}

		if (count == maxEngineFrameNmb) {
			tmpArchetype.RemoveComponent(poke::ecs::ComponentType::RIGIDBODY);
			poke::ArchetypesManagerLocator::Get().UpdateArchetype(archetypeID, tmpArchetype);

			auto pool = poke::EcsManagerLocator::Get().GetPools()[archetypeID];

			for (size_t i = pool.firstEntity; i < pool.lastEntity; i++) {
				ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(i, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
				ASSERT_FALSE(poke::EcsManagerLocator::Get().HasComponent(i, poke::ecs::ComponentType::RIGIDBODY));
			}
		}

		if (count > 10) {
			engine.Stop();
		}
	});

	engine.Run();

	ASSERT_TRUE(true);
}

TEST(ECS, OneArchetypesResizeBigger)
{
	poke::EngineSetting engineSettings{
		"testECSOneArchetypesResizeBigger",
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
	engine.Init();

	const auto size = 100;
	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.size = size;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", size);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < size; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
		ASSERT_FALSE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY));
	}

	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		static int count = 0;
		count++;

		if (count == 5) {
			tmpArchetype.AddComponent(poke::ecs::ComponentType::RIGIDBODY);
			poke::ArchetypesManagerLocator::Get().ResizeArchetype(archetypeID, 200);

			auto pool = poke::EcsManagerLocator::Get().GetPools()[archetypeID];
			ASSERT_TRUE(pool.lastEntity - pool.firstEntity == 200);
		}

		if (count > 10) {
			engine.Stop();
		}
	});

	engine.Run();

	ASSERT_TRUE(true);
}

TEST(ECS, OneArchetypesResizeSmaller)
{
	poke::EngineSetting engineSettings{
        "testECSOneArchetypesResizeSmaller",
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
	engine.Init();

	const auto size = 100;
	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.size = size;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", size);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < size; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
		ASSERT_FALSE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::RIGIDBODY));
	}

	engine.AddObserver(poke::observer::MainLoopSubject::END_FRAME, [&]() {
		static int count = 0;
		count++;

		if (count == 5) {
			poke::ArchetypesManagerLocator::Get().ResizeArchetype(archetypeID, 50);

			auto pool = poke::EcsManagerLocator::Get().GetPools()[archetypeID];
			ASSERT_TRUE(pool.lastEntity - pool.firstEntity == 50);
		}

		if (count > 10) {
			engine.Stop();
		}
	});

	engine.Run();

	ASSERT_TRUE(true);
}

TEST(ECS, ArchetypemanagerGetPool)
{
	poke::EngineSetting engineSettings{
		"testECSArchetypemanagerGetPool",
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
	engine.Init();

	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", 100);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	for (int i = 0; i < 100; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
	}

	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		static int count = 0;
		count++;
		if (count > 10) {
			auto pool = poke::ArchetypesManagerLocator::Get().GetEntityPool(archetypeID);

			ASSERT_TRUE(pool.lastEntity - pool.firstEntity == 100);

			engine.Stop();
		}
	});

	engine.Run();
}

TEST(ECS, ArchetypeManagerDefaultValueAfterDestroy)
{
	poke::EngineSetting engineSettings{
		"testECSArchetypemanagerGetPool",
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
	engine.Init();

	poke::ecs::Archetype tmpArchetype;
	tmpArchetype.AddComponent(poke::ecs::ComponentType::TRANSFORM);
	tmpArchetype.transform.SetLocalPosition({ 0, 0, 0 });

	tmpArchetype.AddComponent(poke::ecs::ComponentType::COLLIDER);
	tmpArchetype.collider.shapeType = poke::physics::ShapeType::BOX;
	tmpArchetype.collider.boxShape.SetCenter({ 0,0,0 });
	tmpArchetype.collider.boxShape.SetExtent({ 1,1,1 });

	const size_t archetypeSize = 100;
	poke::ArchetypesManagerLocator::Get().AddArchetype(tmpArchetype, "test", archetypeSize);

	const auto archetypeID = poke::ArchetypesManagerLocator::Get().GetArchetypeID("test");

	auto& transformManager = poke::EcsManagerLocator::Get().GetComponentsManager<poke::ecs::TransformsManager>();

	for (int i = 0; i < archetypeSize; i++) {
		const auto entityIndex = poke::EcsManagerLocator::Get().AddEntity(archetypeID);

		auto transform = transformManager.GetComponent(entityIndex);
		const auto randX = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randY = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		const auto randZ = poke::math::Random::Get().RandomRange(-50.0f, 50.0f);
		transform.SetLocalPosition(poke::math::Vec3{ randX, randY, randZ });
		transformManager.SetComponent(entityIndex, transform);

		ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entityIndex, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
	}

	auto pools = poke::EcsManagerLocator::Get().GetPools();

	const poke::ecs::EntityIndex entityToDestroy = pools[1].firstEntity + ((pools[1].lastEntity - pools[1].firstEntity) / 2);
	poke::EcsManagerLocator::Get().DestroyEntity(entityToDestroy);
	ASSERT_FALSE(poke::EcsManagerLocator::Get().HasComponent(entityToDestroy, poke::ecs::ComponentType::TRANSFORM | poke::ecs::ComponentType::COLLIDER));
	ASSERT_FALSE(poke::EcsManagerLocator::Get().IsEntityActive(entityToDestroy));
	const auto newEntity = poke::EcsManagerLocator::Get().AddEntity(archetypeID);
	ASSERT_TRUE(newEntity == entityToDestroy);
	ASSERT_TRUE(transformManager.GetComponent(newEntity).GetLocalPosition() == (0, 0, 0));
}
//-----------------------------------------------------------------------------

//------------------------------- ENTITY VECTOR ----------------------------
TEST(ECS, EntityVectorIsSorted)
{
	size_t maxEntitiesNmb = 8000;
	poke::ecs::EntityVector entities(maxEntitiesNmb);

    for(size_t i = maxEntitiesNmb; i > 0; i--) {
		entities.insert(i);
    }

	std::vector<poke::ecs::EntityIndex> copy;
	copy.resize(maxEntitiesNmb);

	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		copy[i] = entities[i];
	}
	std::sort(copy.begin(), copy.end());


	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		ASSERT_TRUE(copy[i] == entities[i]);
	}
}

TEST(ECS, EntityVectorIsSortedWithRandom)
{
	std::random_device rd;
	std::mt19937 g(rd());

	size_t maxEntitiesNmb = 8000;
	std::vector<poke::ecs::EntityIndex> nonSorted;
	nonSorted.resize(maxEntitiesNmb);
	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		nonSorted[i] = i;
	}

	std::shuffle(nonSorted.begin(), nonSorted.end(), g);

	poke::ecs::EntityVector entities(maxEntitiesNmb);

	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		entities.insert(nonSorted[i]);
	}

	std::vector<poke::ecs::EntityIndex> copy;
	copy.resize(maxEntitiesNmb);

	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		copy[i] = entities[i];
	}
	std::sort(copy.begin(), copy.end());


	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		ASSERT_TRUE(copy[i] == entities[i]);
	}
}

TEST(ECS, EntityVectorErase)
{
	std::random_device rd;
	std::mt19937 g(rd());

	size_t maxEntitiesNmb = 8000;
	std::vector<poke::ecs::EntityIndex> nonSorted;
	nonSorted.resize(maxEntitiesNmb);
	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		nonSorted[i] = i;
	}

	std::shuffle(nonSorted.begin(), nonSorted.end(), g);

	poke::ecs::EntityVector entities(maxEntitiesNmb);

	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		entities.insert(nonSorted[i]);
	}

	const int lowPassValue = 2000;
	const int highPassValue = 6000;
	for (auto it = entities.begin(); it != entities.end(); ) {
        if(*it >= lowPassValue && *it <= highPassValue) {
            it = entities.erase(it);
        } else {
            ++it;
        }
	}

	for (const poke::ecs::EntityIndex entity : entities) {
		ASSERT_TRUE(entity < lowPassValue || entity > highPassValue);
	}
}

TEST(ECS, EntityVectorSizeAndCapacity)
{
	std::random_device rd;
	std::mt19937 g(rd());

	size_t maxEntitiesNmb = 8000;
	std::vector<poke::ecs::EntityIndex> nonSorted;
	nonSorted.resize(maxEntitiesNmb);
	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		nonSorted[i] = i;
	}

	std::shuffle(nonSorted.begin(), nonSorted.end(), g);

	poke::ecs::EntityVector entities(maxEntitiesNmb);

	ASSERT_TRUE(entities.size() == 0);
	ASSERT_TRUE(entities.capacity() == maxEntitiesNmb);

	for (size_t i = 0; i < maxEntitiesNmb; i++) {
		entities.insert(nonSorted[i]);
	}

	ASSERT_TRUE(entities.size() == maxEntitiesNmb);
	ASSERT_TRUE(entities.capacity() == maxEntitiesNmb);

	entities.erase(entities.find(50));

	ASSERT_TRUE(entities.size() == maxEntitiesNmb - 1);
	ASSERT_TRUE(entities.capacity() == maxEntitiesNmb);
}

TEST(ECS, EntityVectorResize)
{
	size_t maxEntitiesNmb = 2000;

	poke::ecs::EntityVector entities(maxEntitiesNmb);


	ASSERT_TRUE(entities.size() == 0);
	ASSERT_TRUE(entities.capacity() == maxEntitiesNmb);

	entities.reserve(maxEntitiesNmb * 2);

	ASSERT_TRUE(entities.size() == 0);
	ASSERT_TRUE(entities.capacity() == maxEntitiesNmb * 2);
}
//-----------------------------------------------------------------------------

template <typename T>
class HasGetComponentIndex
{
private:
	typedef char YesType[1];
	typedef char NoType[2];

	template <typename C> static YesType& test(decltype(&C::GetComponentIndex));
	template <typename C> static NoType& test(...);


public:
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};
class test{};

TEST(ECS, ComponentManagerGetComponentIndex)
{
	std::cout << HasGetComponentIndex<poke::ecs::TransformsManager>::value << "\n";
	std::cout << HasGetComponentIndex<test>::value << "\n";
}