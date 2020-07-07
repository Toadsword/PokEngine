#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <SDL.h>

int main() {
	const char* projectName = "pokEngine";


	poke::EngineSetting engineSettings;
	const bool foundSettings = engineSettings.LoadSettingsFromFile(projectName, poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);

    if(!foundSettings) {
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
	
    engine.Init();
    engine.Run();
	  
#ifdef WIN32
	system("pause");                   
#endif
    return EXIT_SUCCESS;
}
  