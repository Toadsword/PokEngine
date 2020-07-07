#include <Tests/TestNico/test_system.h>

#include <CoreEngine/engine.h>
#include <Utility/log.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

TestModule::TestModule(Engine& engine)
    : System(engine),
      engine_(engine),
      modelsManager_(ecsManager_.GetComponentsManager<ecs::ModelsManager>()),
      transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>())
{
    ObserveEngineInit();
	engine_.AddObserver(observer::MainLoopSubject::UPDATE, [this]() {OnUpdate(); });
	engine_.AddObserver(observer::MainLoopSubject::PHYSICS_UPDATE, [this]() {OnPhysicUpdate(); });
    ObserveEntityInit();
    ObserveLoadScene();
}

TestModule::~TestModule() {}

void TestModule::OnAppBuild()
{
    modelCommandBuffer_.emplace(
        GraphicsEngineLocator::Get().GetModelCommandBuffer());
    gizmoCommandBuffer_.emplace(
        GraphicsEngineLocator::Get().GetGizmoCommandBuffer());
}

void TestModule::OnLoadScene()
{
	
}

void TestModule::OnUpdate()
{
	static scene::SceneIndex sceneIndex = 0;

    if(engine_.GetModuleManager().inputManager.IsKeyDown(input::KeyCode::X)) {
        sceneIndex = sceneIndex == 0 ? 1 : 0;
		if (sceneIndex == 0) {
			std::string name = "MainMenuParsed";
			SceneManagerLocator::Get().LoadScene(name);
			sceneIndex = 1;
		}else {
			std::string name = "Default";
			SceneManagerLocator::Get().LoadScene(name);
			sceneIndex = 0;
		}
    }
}

void TestModule::OnPhysicUpdate() {}

void TestModule::OnDraw() {}

void TestModule::OnEntityInit(ecs::EntityIndex entityIndex) { }
} //namespace poke
