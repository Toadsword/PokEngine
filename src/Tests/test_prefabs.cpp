#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/log.h>
#include <fstream>
#include <Ecs/Prefabs/engine_prefab.h>

json GetTestEnginePrefabJson()
{
	using namespace poke;
	json prefabJson;
    //name
	prefabJson[0]["objectName"] = "Player";

    //transform
	prefabJson[0]["transform"]["position"] = math::Vec3(10, 10, 10).ToJson();
	prefabJson[0]["transform"]["rotation"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[0]["transform"]["scale"] = math::Vec3(1, 1, 1).ToJson();
	prefabJson[0]["transform"]["parent"] = -1;

    //rigidbody
	prefabJson[0]["rigidbody"]["angularDrag"] = 0.0f;
	prefabJson[0]["rigidbody"]["angularVelocity"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[0]["rigidbody"]["linearDrag"] = 0.0f;
	prefabJson[0]["rigidbody"]["linearVelocity"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[0]["rigidbody"]["type"] = physics::RigidbodyType::STATIC;

	//collider
	prefabJson[0]["collider"]["isTrigger"] = true;
	prefabJson[0]["collider"]["shape"]["extent"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[0]["collider"]["shape"]["positionOffset"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[0]["collider"]["shapeType"] = physics::ShapeType::BOX;

	//model
	prefabJson[0]["model"]["meshHash"] = 2;
	prefabJson[0]["model"]["materialHash"] = poke::math::HashString("default_material_name");

	//name
	prefabJson[1]["objectName"] = "weapon_projectile";

	//transform
	prefabJson[1]["transform"]["position"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[1]["transform"]["rotation"] = math::Vec3(0, 0, 0).ToJson();
	prefabJson[1]["transform"]["scale"] = math::Vec3(1, 1, 1).ToJson();
	prefabJson[1]["transform"]["parent"] = 0;

	return prefabJson;
}

json GetTestPrefabsFile()
{
	json prefabs;
	prefabs["prefabs"][0]["name"] = "test";
	prefabs["prefabs"][0]["objects"] = GetTestEnginePrefabJson();
	return prefabs;
}

TEST(Prefab, SetFromJson)
{
	poke::EngineSetting engineSettings{
        "testPrefabSetFromJson",
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

	poke::ecs::EnginePrefab prefab;  

	prefab.SetFromJson(GetTestEnginePrefabJson());

	auto parents = prefab.GetParentIndexes();

	ASSERT_TRUE(parents.size() == 2);
	ASSERT_TRUE(parents[0] == -1);
}

TEST(Prefab, ToJson)
{
	poke::EngineSetting engineSettings{
		"testPrefabToJson",
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
	poke::ecs::EnginePrefab prefab;
	const json jsonContent = GetTestEnginePrefabJson();

	prefab.SetFromJson(jsonContent);
	json copy = prefab.ToJson();

	std::cout << copy;

	ASSERT_TRUE(jsonContent == copy);
}

TEST(Prefab, Intantiate)
{
	poke::EngineSetting engineSettings{
		"testPrefabIntantiate",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"[Test] Prefabs: Intantiate",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	poke::ecs::EnginePrefab prefab;
	const json jsonContent = GetTestEnginePrefabJson();

	prefab.SetFromJson(jsonContent);

	engine.Init();

	auto entities = prefab.Instantiate();
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[0], poke::ecs::ComponentType::TRANSFORM));
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[1], poke::ecs::ComponentType::TRANSFORM));
}

TEST(Prefab, PrefabManager_SetFromJson)
{
	poke::EngineSetting engineSettings{
		"testPrefabPrefabManager_SetFromJson",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"[Test] PrefabsManager: SetFromJson",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST
    json jsonContent;
	std::ifstream jsonFile("E:/PokEngine/AppData/resources/prefabs/test.pokprefab");

	jsonFile >> jsonContent;
	jsonFile.close();

	poke::PrefabsManagerLocator::Get().SetFromJson(jsonContent["prefabs"]);

	auto ids = poke::PrefabsManagerLocator::Get().GetPrefabsIDs();

	ASSERT_TRUE(ids.size() == 1);
}

TEST(Prefab, PrefabManager_Instantiate)
{
	poke::EngineSetting engineSettings{
		"testPrefabPrefabManager_Instantiate",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"[Test] PrefabsManager: Intantiate",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	engine.Init();

	// TEST
	json jsonContent = GetTestPrefabsFile();
	poke::PrefabsManagerLocator::Get().SetFromJson(jsonContent["prefabs"]);
	auto& prefab = poke::PrefabsManagerLocator::Get().GetPrefab("test");

	auto entities = prefab.Instantiate();

	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[0], poke::ecs::ComponentType::TRANSFORM));
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[0], poke::ecs::ComponentType::RIGIDBODY));
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[0], poke::ecs::ComponentType::COLLIDER));
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[0], poke::ecs::ComponentType::MODEL));
	ASSERT_TRUE(poke::EcsManagerLocator::Get().HasComponent(entities[1], poke::ecs::ComponentType::TRANSFORM));

	engine.Run();
}

TEST(Prefab, PrefabManager_InstantiateNonExistingPrefab)
{
	poke::EngineSetting engineSettings{
		"testPrefabPrefabManager_InstantiateNonExistingPrefab",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"[Test] PrefabsManager: Intantiate",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	engine.Init();

	// TEST
	json jsonContent = GetTestPrefabsFile();
	poke::PrefabsManagerLocator::Get().SetFromJson(jsonContent["prefabs"]);

	const auto entities = poke::PrefabsManagerLocator::Get().Instantiate("ici");

	ASSERT_TRUE(entities.size() == 0);
}

TEST(Prefab, PrefabManager_EditorDisplayName)
{
	poke::EngineSetting engineSettings{
		"testPrefabPrefabManager_EditorDisplayName",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"[Test] PrefabsManager: Intantiate",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	engine.Init();

	// TEST
	json jsonContent = GetTestPrefabsFile();
	poke::PrefabsManagerLocator::Get().SetFromJson(jsonContent["prefabs"]);

	auto& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	const auto entities = poke::PrefabsManagerLocator::Get().Instantiate("test");

	ASSERT_TRUE(editor.GetEditorEcsManager().GetEntityName(entities[0]) == "Player");
	ASSERT_TRUE(editor.GetEditorEcsManager().GetEntityName(entities[1]) == "weapon_projectile");
}
