#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <Utility/file_system.h>
#include <Tests/TestNico/test_system.h>
#include <Tests/TestNico/test_spline.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Game/game_system.h>
#include "Editor/ComponentManagers/editor_component_manager.h"
#include "Math/quaternion.h"

TEST(Nico, Default)
{
    poke::EngineSetting engineSettings{
		"testNicoDefault",
        poke::AppType::EDITOR,
        std::chrono::duration<double, std::milli>(16.66f),
        720,
        640,
        "POK engine",
		{{0, "Default", "Default"}, {1, "MainMenu", "MainMenuParsed"}}
		//{{0, "Default", "MainMenuParsed"}, {1, "MainMenu", "Default"}}
    };

    poke::Engine engine(engineSettings);

    //Load editor application
    engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

    //Load editor graphics renderer
    engine.GetModuleManager().graphicsEngine.SetRenderer(
        std::make_unique<poke::graphics::RendererEditor>(engine));

	poke::TestModule testModule(engine);

    // TEST

    engine.Init();

    engine.Run();
}

TEST(Nico, Spline)
{
	poke::EngineSetting engineSettings{
		"testNicoSpline",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine"
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST
	poke::TestSpline testSpline(engine);

	// TEST

	engine.Init();

	//Spline Test

	engine.Run();
}

class ChunkTest {
public:
	ChunkTest(poke::Engine& engine) :
		engine_(engine)
	{

	}

    void Test()
	{
		engine_.GetModuleManager().inputManager.SimulateKeyDown(poke::input::KeyCode::J);
		engine_.GetModuleManager().inputManager.SimulateKeyUp(poke::input::KeyCode::J);
		engine_.GetModuleManager().inputManager.SimulateKeyDown(poke::input::KeyCode::J);
		engine_.GetModuleManager().inputManager.SimulateKeyDown(poke::input::KeyCode::J);
	}

	void OnUpdate()
	{

		if(engine_.GetModuleManager().inputManager.IsKeyDown(poke::input::KeyCode::J)) {
			poke::ChunksManagerLocator::Get().SetActiveChunk(++activeChunk_);
		}

		count++;

        if(count == 10) {
			poke::MaterialsManagerLocator::Get();
        }
	}
private:

	int count = 0;
	int activeChunk_ = 0;

	poke::Engine& engine_;
};

TEST(Nico, Chunks)
{

    poke::EngineSetting engineSettings{
		"testNicoChunks",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "MainMenuParsed"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// -------------------------------- TEST ----------------------------------
	ChunkTest chunkTest(engine);
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {chunkTest.OnUpdate(); });
	engine.AddObserver(poke::observer::MainLoopSubject::END_FRAME, [&]() {chunkTest.Test(); });
	// ------------------------------------------------------------------------

	engine.Init();

	//Spline Test

	engine.Run();
}

TEST(Nico, MatInverse)
{
	poke::math::Matrix3 mat3({ 3, 2, 0 }, { 0, 0, 1 }, { 2, -2, 1 });

	std::cout << mat3.ToString() << "\n==========================\n";

	poke::math::Matrix3 inv3 = poke::math::Matrix3::GetInverse(mat3);

	std::cout << inv3.ToString() << "\n==========================\n";
    
	auto c = mat3 * inv3;

	std::cout << c.ToString() << "\n==========================\n";

	poke::math::Matrix4 mat({ -1, 88, 9, 1 }, { 1, -2, 1, 5 }, { 5, 8, 1, -1 }, { 1, 5, 1, -1 });

	std::cout << mat.ToString() << "\n==========================\n";

	poke::math::Matrix4 inv = poke::math::Matrix4::GetInverse(mat);

	std::cout << inv.ToString() << "\n==========================\n";

	auto b = mat * inv;

	std::cout << b.ToString() << "\n==========================\n";

	ASSERT_TRUE(true);
}

class TestGameSystem : poke::game::GameSystem {
public:
    TestGameSystem(poke::Engine& engine, poke::game::Game& game) : GameSystem(engine, game)
    {
		game.RegisterObserverUpdate([this]() {OnUpdate(); });
    }

    void OnUpdate()
    {
		static int i = 0;
		std::cout << "ICI " << i++ << "\n"; 

		int b = 0;
        while(b < 10000000) {
		    b++;
        }
    }
};

TEST(Nico, PauseGame)
{
	poke::EngineSetting engineSettings{
		"testNicoPauseGame",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "MainMenuParsed"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// -------------------------------- TEST ----------------------------------

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	editor.StartGame();

	poke::game::Game& game = editor.GetGame();

	TestGameSystem system = TestGameSystem(engine, game);

	// ------------------------------------------------------------------------

	engine.Init();

	//Spline Test

	engine.Run();

	ASSERT_TRUE(true);
}

TEST(Nico, CameraFollowSpline)
{
	poke::EngineSetting engineSettings{
		"testNicoPauseGame",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine"
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// -------------------------------- TEST ----------------------------------

	poke::editor::Editor& editor = static_cast<poke::editor::Editor&>(engine.GetApp());

	editor.StartGame();

	poke::game::Game& game = editor.GetGame();

	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {
		using namespace poke;

		static bool once = false;

		if (!once) {
			once = true;
			auto& splineManager = EcsManagerLocator::Get().GetComponentsManager<ecs::SplineFollowersManager>();
			auto& editorComponentManager = EcsManagerLocator::Get().GetComponentsManager<ecs::EditorComponentManager>();

			EcsManagerLocator::Get().SetActive(2, ecs::EntityStatus::ACTIVE);
            EcsManagerLocator::Get().AddComponent(2, ecs::ComponentType::SPLINE_FOLLOWER);
			EcsManagerLocator::Get().AddComponent(2, ecs::ComponentType::EDITOR_COMPONENT);
			auto editorComponent = editorComponentManager.GetComponent(2);
			editorComponent.name = "Spline";
			editorComponentManager.SetComponent(2, editorComponent);

			auto splineComponent = splineManager.GetComponent(2);
			splineComponent.spline.AddPoint({ 0, 0, 0 });
			splineComponent.spline.AddPoint({ 0, 0, 10 });
			splineComponent.spline.AddPoint({ 0, 0, 20 });
			splineComponent.spline.AddPoint({ 0, 0, 30 });
			splineComponent.spline.AddPoint({ 0, 0, 40 });
			//splineComponent.spline.AddPoint({ 0, 10, 10 });
			splineManager.SetComponent(2, splineComponent);

			EcsManagerLocator::Get().SetActive(3, ecs::EntityStatus::ACTIVE);
			EcsManagerLocator::Get().AddComponent(3, ecs::ComponentType::TRANSFORM);
			auto editorComponent3 = editorComponentManager.GetComponent(3);
			editorComponent3.name = "entityInCamera";
			editorComponentManager.SetComponent(3, editorComponent3);

			EcsManagerLocator::Get().SetActive(4, ecs::EntityStatus::ACTIVE);
			EcsManagerLocator::Get().AddComponent(4, ecs::ComponentType::TRANSFORM);
			auto editorComponent4 = editorComponentManager.GetComponent(4);
			editorComponent4.name = "entityInFrontOfCamera";
			editorComponentManager.SetComponent(4, editorComponent4);

			EcsManagerLocator::Get().SetActive(5, ecs::EntityStatus::ACTIVE);
			EcsManagerLocator::Get().AddComponent(5, ecs::ComponentType::SPLINE_FOLLOWER);
			EcsManagerLocator::Get().AddComponent(5, ecs::ComponentType::EDITOR_COMPONENT);
			auto editorComponent5 = editorComponentManager.GetComponent(5);
			editorComponent5.name = "SplineToLookAt";
			editorComponentManager.SetComponent(5, editorComponent5);

			auto splineComponent2 = splineManager.GetComponent(5);
			splineComponent2.spline.AddPoint({ 0, -5, 10 });
			splineComponent2.spline.AddPoint({ 0, -5, 20 });
			splineComponent2.spline.AddPoint({ 0, -5, 30 });
			splineComponent2.spline.AddPoint({ 0, -5, 40 });
			splineComponent2.spline.AddPoint({ 0, -5, 50 });
			splineManager.SetComponent(5, splineComponent2);
		}
		auto& transformManager = EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>();

        //entity in camera
		{
			auto transform = transformManager.GetComponent(3);

			transform.SetLocalPosition(CameraLocator::Get().GetPosition());
			transform.SetLocalRotation(CameraLocator::Get().GetRotation());
			transformManager.SetComponent(3, transform);
		}

        //entity in front of camera
		{
			auto transform = transformManager.GetComponent(4);

			transform.SetLocalPosition(CameraLocator::Get().GetPosition() - CameraLocator::Get().GetFront());

			auto& gizmoCommandBuffer = GraphicsEngineLocator::Get().GetGizmoCommandBuffer();

			gizmoCommandBuffer.DrawWireSphere(transform.GetLocalPosition(), 0.1f );
			transformManager.SetComponent(4, transform);
		}

        //camera movement following spline
		{
			auto& splineManager = EcsManagerLocator::Get().GetComponentsManager<ecs::SplineFollowersManager>();
			const auto spline = splineManager.GetComponent(2);
			const auto splineLookAt = splineManager.GetComponent(5);

			auto& camera = CameraLocator::Get();

            //position
			camera.SetPosition(spline.spline.Lerp(spline.lastPoint, spline.segmentPercentage));

            //rotation

			const auto looPos = splineLookAt.spline.Lerp(splineLookAt.lastPoint, splineLookAt.segmentPercentage);
			auto rotation = math::Quaternion::LookRotation((camera.GetPosition() - looPos).Normalize()).GetEulerAngles();
			//rotation.z = 0;
			camera.SetRotation(rotation);
			camera.Update();
		}
	});

	// ------------------------------------------------------------------------

	engine.Init();

	//Spline Test

	engine.Run();

	ASSERT_TRUE(true);
}
