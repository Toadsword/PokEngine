#include <CoreEngine/engine.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <Editor/editor.h>
#include <Editor/editor_utility.h>

int main() {
	const char* projectName = "pokShowroom";

	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

	if (!foundSettings) {
		engineSettings = poke::EngineSetting(
			projectName,
			poke::AppType::SHOWROOM,
			std::chrono::duration<double, std::milli>(16.66f),
			720,
			640,
			"POK showroom",
			{},
			"",
			poke::editor::TOOL_FLAG_STATS | poke::editor::TOOL_FLAG_LOG
		);
	}

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load game's graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(std::make_unique<poke::graphics::RendererEditor>(engine));

	engine.Init();
	engine.Run();

	return EXIT_SUCCESS;
}
