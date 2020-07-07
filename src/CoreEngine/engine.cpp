#include <CoreEngine/engine.h>

#include <Utility/log.h>
#include <Utility/time_custom.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
Engine::Engine(const EngineSetting& engineSettings)
    : engineSettings_(engineSettings),
      subjectsContainer_(
          {
              observer::MainLoopSubject::ENGINE_BUILD,
              observer::MainLoopSubject::APP_BUILD,
              observer::MainLoopSubject::ENGINE_INIT,
              observer::MainLoopSubject::APP_INIT,
              observer::MainLoopSubject::ENGINE_DESTROY,
          }),
      moduleContainer_(*this),
      coreSystemsContainer_(*this),
      isRunning_(false)
{
    Time::Get().StartRecordingTime();

	//Initialization Services Locator
	GraphicsEngineLocator::Initialize();
	TextureManagerLocator::Initialize();
	MeshManagerLocator::Initialize();
	SceneManagerLocator::Initialize();
	EcsManagerLocator::Initialize();
	InputManagerLocator::Initialize();
	ChunksManagerLocator::Initialize();
	CameraLocator::Initialize();
	MaterialsManagerLocator::Initialize();
	WorldLocator::Initialize();
	PhysicsEngineLocator::Initialize();
	TagManagerLocator::Initialize();
	ArchetypesManagerLocator::Initialize();
	AudioEngineLocator::Initialize();
	SoundsManagerLocator::Initialize();
	PrefabsManagerLocator::Initialize();

	Build();

	Notify(observer::MainLoopSubject::ENGINE_BUILD);
}

Engine::~Engine(){}

void Engine::Build() {
    //Assign Services Locator
	GraphicsEngineLocator::Assign(&moduleContainer_.graphicsEngine);
	TextureManagerLocator::Assign(&moduleContainer_.resourcesManagerContainer.textureManager);
	MeshManagerLocator::Assign(&moduleContainer_.resourcesManagerContainer.meshManager);
	SceneManagerLocator::Assign(&moduleContainer_.sceneManager);
	//EcsManagerLocator::Assign(&moduleContainer_.ecsManager);
	InputManagerLocator::Assign(&moduleContainer_.inputManager);
	ChunksManagerLocator::Assign(&moduleContainer_.chunksManager);
	MaterialsManagerLocator::Assign(&moduleContainer_.resourcesManagerContainer.materialsManager);
	WorldLocator::Assign(&world_);
	PhysicsEngineLocator::Assign(&moduleContainer_.physicsEngine);
	TagManagerLocator::Assign(&moduleContainer_.tagManager);
	ArchetypesManagerLocator::Assign(&moduleContainer_.archetypesManager);
	AudioEngineLocator::Assign(&moduleContainer_.audioEngine);
	SoundsManagerLocator::Assign(&moduleContainer_.resourcesManagerContainer.soundsManager);
	PrefabsManagerLocator::Assign(&moduleContainer_.resourcesManagerContainer.prefabsManager);
}

void Engine::Init()
{
    //Call callback EngineInit
    moduleContainer_.resourcesManagerContainer.Init();

    Notify(observer::MainLoopSubject::ENGINE_INIT);

    //App Init, call x function later

	Notify(observer::MainLoopSubject::APP_INIT);

	moduleContainer_.sceneManager.OnAppInit();

    //Clean non used modules
	if(&TextureManagerLocator::Get() != &moduleContainer_.resourcesManagerContainer.textureManager) {
		moduleContainer_.resourcesManagerContainer.textureManager.Clear();
		moduleContainer_.resourcesManagerContainer.textureManager.Resize(0);
	}

    if(&SceneManagerLocator::Get() != &moduleContainer_.sceneManager) {
		LogError("The scene manager locator is not using the CoreSceneManager, but the CoreSceneManager is not unloaded.");
    }

	if (&MeshManagerLocator::Get() != &moduleContainer_.resourcesManagerContainer.meshManager) {
		moduleContainer_.resourcesManagerContainer.meshManager.Clear();
		moduleContainer_.resourcesManagerContainer.meshManager.Resize(0);
	}

	//if (&EcsManagerLocator::Get() != &moduleContainer_.ecsManager) {
	//	moduleContainer_.ecsManager.Destroy();
	//}
	
	if (&InputManagerLocator::Get() != &moduleContainer_.inputManager) {
		LogError("The input manager locator is not using the CoreInputManager, but the CoreInputManager is not unloaded.");
	}

	if (&ChunksManagerLocator::Get() != &moduleContainer_.chunksManager) {
		LogError("The chunk manager locator is not using the CoreChunkManager, but the CoreChunkManager is not unloaded.");
	}

	if (&MaterialsManagerLocator::Get() != &moduleContainer_.resourcesManagerContainer.materialsManager) {
		moduleContainer_.resourcesManagerContainer.materialsManager.Clear();
		moduleContainer_.resourcesManagerContainer.materialsManager.Resize(0);
	}

	if (&WorldLocator::Get() != &world_) {
		LogError("The world locator is not using the CoreChunkManager, but the WorldManager is not unloaded.");
	}

	if (&PhysicsEngineLocator::Get() != &moduleContainer_.physicsEngine) {
		LogError("The physics engine locator is not using the PhysicsEngine, but the PhysicsEngine is not unloaded.");
	}

	if (&TagManagerLocator::Get() != &moduleContainer_.tagManager) {
		LogError("The tag manager locator is not using the CoreTagManager, but the CoreTagManager is not unloaded.");
	}

	if (&ArchetypesManagerLocator::Get() != &moduleContainer_.archetypesManager) {
		LogError("The archetype manager locator is not using the CoreArchetpyeManager, but the CoreArchetypeManager is not unloaded.");
	}

	if (&AudioEngineLocator::Get() != &moduleContainer_.audioEngine) {
		moduleContainer_.audioEngine.Destroy();
	}

	if (&SoundsManagerLocator::Get() != &moduleContainer_.resourcesManagerContainer.soundsManager) {
		moduleContainer_.resourcesManagerContainer.soundsManager.Clear();
		moduleContainer_.resourcesManagerContainer.soundsManager.Resize(0);
	}

	if (&PrefabsManagerLocator::Get() != &moduleContainer_.resourcesManagerContainer.prefabsManager) {
		moduleContainer_.resourcesManagerContainer.prefabsManager.Clear();
		moduleContainer_.resourcesManagerContainer.prefabsManager.Resize(0);
	}
}

void Engine::Run()
{
	app_->Run();

    Destroy();
}

void Engine::Stop() const { app_->Stop(); }

void Engine::Destroy()
{
    moduleContainer_.sceneManager.UnLoadActiveScene();

	//Save project settings

    //We don't want to save test projects settings
    if(engineSettings_.GetProjectName().substr(0, 4) != "test") {
	    const json savedSettings = GetEngineSettings().SaveSettingsToJson();
	    PokFileSystem::WriteFile(
		    engineSettings_.GetProjectName(),
		    savedSettings,
		    FileType::ENGINE_SETTING,
		    FolderType::SAVE_IN_ROM);
    }

    subjectsContainer_.NotifySubject(observer::MainLoopSubject::ENGINE_DESTROY);

    //Must be the last one
    Logger::Get().WriteLogsInFile();
}

void Engine::AddObserver(
    const observer::MainLoopSubject callbackType,
    const std::function<void()>& observerCallback)
{
    switch(callbackType) {
        case observer::MainLoopSubject::ENGINE_BUILD: 
        case observer::MainLoopSubject::APP_BUILD: 
        case observer::MainLoopSubject::ENGINE_INIT: 
        case observer::MainLoopSubject::APP_INIT: 
        case observer::MainLoopSubject::ENGINE_DESTROY:
			subjectsContainer_.AddObserver(callbackType, observerCallback);
        break;
        case observer::MainLoopSubject::UPDATE:
			app_->RegisterObserverUpdate(observerCallback);
        break;
        case observer::MainLoopSubject::PHYSICS_UPDATE:
			app_->RegisterObserverPhysicsUpdate(observerCallback);
        break;
        case observer::MainLoopSubject::DRAW:
			app_->RegisterObserverDraw(observerCallback);
        break;
        case observer::MainLoopSubject::CULLING:
			app_->RegisterObserverCulling(observerCallback);
        break;
        case observer::MainLoopSubject::RENDER:
			app_->RegisterObserverRender(observerCallback);
        break;
        case observer::MainLoopSubject::END_FRAME:
			app_->RegisterObserverEndFrame(observerCallback);
        break;    
        case observer::MainLoopSubject::INPUTS:
			app_->RegisterObserverInput(observerCallback);
        break;
    default: ; }
}

ModuleContainer& Engine::GetModuleManager() { return moduleContainer_; }

void Engine::SetApp(std::unique_ptr<EngineApplication>&& app)
{
    app_ = std::move(app);

	app_->Build();

	Notify(observer::MainLoopSubject::APP_BUILD);
}

void Engine::Notify(const observer::MainLoopSubject type) const
{
    subjectsContainer_.NotifySubject(type);
}

void Engine::AddTask(const std::function<void()>& task)
{
    workerThread_.DoAsync(task);
}

void Engine::AddAsync(const std::function<void()> function, const ThreadType type)
{
    switch (type) {
    case ThreadType::MAIN:
        mainThread_.DoAsync(function);
        break;
    case ThreadType::RENDER:
        drawThread_.DoAsync(function);
        break;
    case ThreadType::WORKER:
        workerThread_.DoAsync(function);
        break;
    default: ;
    }
}

void Engine::AddSync(const std::function<void()> function, const ThreadType type)
{
    switch (type) {
    case ThreadType::MAIN:
        mainThread_.DoSync(function);
        break;
    case ThreadType::RENDER:
        drawThread_.DoSync(function);
        break;
    case ThreadType::WORKER:
        workerThread_.DoSync(function);
        break;
    default: ;
    }
}

void Engine::Wait(const ThreadType type)
{
    switch (type) {
    case ThreadType::MAIN:
        mainThread_.Wait();
        break;
    case ThreadType::RENDER:
        drawThread_.Wait();
        break;
    case ThreadType::WORKER:
        workerThread_.Wait();
        break;
    default: ;
    }
}
} //namespace poke
