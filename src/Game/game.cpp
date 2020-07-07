#include <Game/game.h>

#include <CoreEngine/engine.h>
#include <Utility/file_system.h>
#include <Utility/profiler.h>
#include <Utility/time_custom.h>

#include <Game/app_components_managers_container.h>

namespace poke {
namespace game {
Game::Game(Engine& engine, const std::string& fileName)
    : EngineApplication(engine, fileName),
	appSystemsContainer_(engine, *this),
	resourcesManagerContainer_(engine),
    gameEcsManager_(engine, 8000){
    ObserveEngineInit();
	subjectUpdate_.AddObserver([this]() {OnUpdate(); });

	EcsManagerLocator::Assign(&gameEcsManager_);
	PrefabsManagerLocator::Assign(&resourcesManagerContainer_.prefabsManager);
}

void Game::Build() {
	ArchetypesManagerLocator::Assign(&gameArchetypesManager_);
	resourcesManagerContainer_.Init();
	CameraLocator::Assign(&camera_);
	ecs::AppComponentsManagersContainer::Init();
	PrefabsContainer::Init();


}

void Game::OnUpdate()
{
	
}

void Game::LoadApp() {
    if (appFileName_.empty()) { return; }

    std::string file = PokFileSystem::ReadFile(appFileName_, FileType::DATA);
}

void Game::Pause()
{
	state_ = AppState::PAUSE;
}
void Game::Run()
{
	state_ = AppState::RUNNING;

	while (state_ != AppState::STOP) {
		if (state_ == AppState::PAUSE) continue;

		std::vector<std::string> loadingTitle = { "[=          ]", "[ =         ]", "[  =        ]", "[   =       ]", "[    =      ]", "[     =     ]", "[      =    ]", "[       =   ]", "[        =  ]", "[         = ]", "[          =]", "[         = ]", "[        =  ]", "[       =   ]", "[      =    ]", "[     =     ]", "[    =      ]", "[   =       ]", "[  =        ]", "[ =         ]" };
		std::string title = SceneManagerLocator::Get().GetActiveScene().GetSceneName();

		static int iteration = 0;
		GraphicsEngineLocator::Get().SetTitle((loadingTitle[iteration++ % loadingTitle.size()] + " " + title).c_str());

		pok_BeginFrame(0);

		Time::Get().StartFrame();
		subjectInputs_.Notify();

		//Begin profiling threads

		//Main thread
		//Being profiling main thread
		engine_.AddAsync([] { pok_BeginProfiling(App_Thread, 0) }, ThreadType::MAIN);

		//PhyscisUpdate
		engine_.AddAsync([this] {
			pok_BeginProfiling(Update_Physics, 0);
			subjectPhysicsUpdate_.Notify();
			pok_EndProfiling(Update_Physics);
		}, ThreadType::MAIN);

		//Update
		engine_.AddAsync([this] {
			pok_BeginProfiling(Update, 0);
			subjectUpdate_.Notify();
			pok_EndProfiling(Update);
		}, ThreadType::MAIN);

		//Draw
		engine_.AddAsync([this] {
			pok_BeginProfiling(Draw, 0);
			subjectDraw_.Notify();
			pok_EndProfiling(Draw);
		}, ThreadType::MAIN);

		//End profiling main thread
		engine_.AddAsync([this] {
			pok_EndProfiling(App_Thread);
		}, ThreadType::MAIN);

		//Draw thread
		//Begin profiling render thread
		engine_.AddAsync([] { pok_BeginProfiling(Render_Thread, 0) }, ThreadType::RENDER);

		//Culling
		engine_.AddAsync([this] {
			pok_BeginProfiling(Culling, 0);
			subjectCulling_.Notify();
			pok_EndProfiling(Culling);
		}, ThreadType::RENDER);

		//Render
		engine_.AddAsync([this] {
			pok_BeginProfiling(Render, 0);
			subjectRender_.Notify();
			pok_EndProfiling(Render);
		}, ThreadType::RENDER);

		//End profiling render thread
		engine_.AddAsync([] { pok_EndProfiling(Render_Thread) }, ThreadType::RENDER);

		//End profiling threads
		//Wait end of frame
		engine_.Wait(ThreadType::MAIN);
		engine_.Wait(ThreadType::RENDER);
		engine_.Wait(ThreadType::WORKER);

		//End frame
		subjectEndFrame_.Notify();

		//There is more constancy doing this than doing a Sleep().
		while (Time::Get().GetFrameElapsedTime() < 16.66f) {}

		Time::Get().EndFrame();

		pok_EndFrame();
	}
}
void Game::Stop()
{
	state_ = AppState::STOP;
}

void Game::RegisterObserverUpdate(const std::function<void()> callback)
{
	subjectUpdate_.AddObserver(callback);
}
void Game::RegisterObserverPhysicsUpdate(const std::function<void()> callback)
{
	subjectPhysicsUpdate_.AddObserver(callback);
}
void Game::RegisterObserverDraw(const std::function<void()> callback)
{
	subjectDraw_.AddObserver(callback);
}
void Game::RegisterObserverCulling(const std::function<void()> callback)
{
	subjectCulling_.AddObserver(callback);
}
void Game::RegisterObserverRender(const std::function<void()> callback)
{
	subjectRender_.AddObserver(callback);
}
void Game::RegisterObserverEndFrame(const std::function<void()> callback)
{
	subjectEndFrame_.AddObserver(callback);
}
void Game::RegisterObserverInput(const std::function<void()> callback)
{
	subjectInputs_.AddObserver(callback);
}

void Game::NotifyUpdate() const
{
    subjectUpdate_.Notify();
}

void Game::NotifyDraw() const { subjectDraw_.Notify(); }

void Game::NotifyRender() const
{
    subjectRender_.Notify();
}

void Game::NotifyCulling() const
{
    subjectCulling_.Notify();
}

void Game::NotifyInput() const
{
    subjectInputs_.Notify();
}

void Game::NotifyEndFrame() const
{
    subjectEndFrame_.Notify();
}

void Game::NotifyPhysicsUpdate() const
{
    subjectPhysicsUpdate_.Notify();
}
} //namespace editor
} //namespace poke
