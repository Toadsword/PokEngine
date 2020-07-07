#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

class SimulateInput {
public:
	SimulateInput(poke::Engine& engine) :
		engine_(engine)
	{

	}

	void OnUpdate()
	{
		poke::InputManagerLocator::Get().SimulateKeyDown(poke::input::KeyCode::A);
		poke::InputManagerLocator::Get().SimulateKeyUp(poke::input::KeyCode::B);
		poke::InputManagerLocator::Get().SimulateMouseDown(poke::input::MouseButtonCode::LEFT);
		poke::InputManagerLocator::Get().SimulateMouseUp(poke::input::MouseButtonCode::RIGHT);
		poke::InputManagerLocator::Get().SimulateActionDown(poke::input::Action::MAIN_SHOOT);
		poke::InputManagerLocator::Get().SimulateActionUp(poke::input::Action::UP);

		if (poke::InputManagerLocator::Get().IsKeyDown(poke::input::KeyCode::A)) {
			keyDown_ = true;
		}

		if (poke::InputManagerLocator::Get().IsKeyUp(poke::input::KeyCode::B)) {
			keyUp_ = true;
		}

		if (poke::InputManagerLocator::Get().IsMouseButtonDown(poke::input::MouseButtonCode::LEFT)) {
			mouseDown_ = true;
		}

		if (poke::InputManagerLocator::Get().IsMouseButtonUp(poke::input::MouseButtonCode::RIGHT)) {
			mouseUp_ = true;
		}

		if (poke::InputManagerLocator::Get().IsActionDown(poke::input::Action::MAIN_SHOOT)) {
			actionDown_ = true;
		}

		if (poke::InputManagerLocator::Get().IsActionUp(poke::input::Action::UP)) {
			actionUp_ = true;
		}

		count_++;

		if (count_ == 10) {
			engine_.Stop();
		}
	}

    bool HasSucceed() const
	{
		return keyDown_ && keyUp_ && mouseDown_ && mouseUp_ && actionDown_ && actionUp_;
	}
private:

	bool keyDown_ = false;
	bool keyUp_ = false;
	bool mouseDown_ = false;
	bool mouseUp_ = false;
	bool actionDown_ = false;
	bool actionUp_ = false;

	int count_ = 0;

	poke::Engine& engine_;
};

TEST(Input, Simulate)
{

	poke::EngineSetting engineSettings{
		"testInputSimulate",
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
	SimulateInput test(engine);
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {test.OnUpdate(); });
	// ------------------------------------------------------------------------

	engine.Init();

	//Spline Test

	engine.Run();

	ASSERT_TRUE(test.HasSucceed());
}