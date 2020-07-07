#include <CoreEngine/engine.h>
#include <Game/game.h>
#include <GraphicsEngine/Renderers/renderer_game.h>

int main() {
	const char* projectName = "pokGame";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::GAME,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK game"
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::game::Game>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererGame>(engine));

	engine.Init();
	engine.Run();

	return EXIT_SUCCESS;
}

int WinMain() {
	return main();
}
