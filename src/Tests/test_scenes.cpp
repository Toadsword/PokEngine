#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <Utility/file_system.h>
#include <Tests/TestNico/test_system.h>
#include <Tests/TestNico/test_spline.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

#include <Utility/json_utility.h>

TEST(Scenes, Default)
{
	poke::EngineSetting engineSettings{
        "testScenesDefault",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,// Width.
		640, // Height.
		"POK engine", // Window name.
		{
			{0, "Default", "Default"},
		}
	};

	poke::Engine engine(engineSettings);
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	engine.Init();
	engine.Run();
}

TEST(Scenes, CheckSceneSaveLoadSame)
{
	std::string sceneName = "TestAllElementScene";

	json baseSceneJson = poke::LoadJson(sceneName, poke::FileType::SCENE, poke::FolderType::ROM);
	json baseSceneJson2 = poke::LoadJson(sceneName, poke::FileType::SCENE, poke::FolderType::ROM);
	EXPECT_EQ(baseSceneJson, baseSceneJson2);
    
	poke::EngineSetting engineSettings{
		"testScenesDefault",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,// Width.
		640, // Height.
		"POK engine", // Window name.
		{
			{0, sceneName, sceneName},
		}
	};

	poke::Engine engine(engineSettings);
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

    //Save scene
	engine.Init();
	engine.GetModuleManager().sceneManager.SaveActiveScene();

    //Read back
	json endSceneJson = poke::LoadJson(sceneName, poke::FileType::SCENE, poke::FolderType::ROM);

    //Since we changed nothing in the scene, we except it to be the exact same !
	EXPECT_EQ(baseSceneJson, endSceneJson);
}