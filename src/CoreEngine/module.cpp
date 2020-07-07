#include <CoreEngine/module.h>

#include <CoreEngine/engine.h>

namespace poke {
Module::Module(Engine& engine) : engine_(engine) { }

Module::~Module() {}

void Module::ObserveEngineInit()
{
    ObserveMainLoopSubject(
        observer::MainLoopSubject::ENGINE_INIT,
        [this]() { this->OnAppBuild(); });
}

void Module::ObserveAppInit()
{
    ObserveMainLoopSubject(
        observer::MainLoopSubject::APP_INIT,
        [this]() { this->OnAppInit(); });
}

void Module::ObserveEngineDestroy()
{
    ObserveMainLoopSubject(
        observer::MainLoopSubject::ENGINE_DESTROY,
        [this]() { this->OnEngineDestroy(); });
}

void Module::ObserveLoadScene()
{
    engine_.GetModuleManager().sceneManager.
            AddOnLoadObserver([this]() { this->OnLoadScene(); });
}

void Module::ObserveUnloadScene()
{
    engine_.GetModuleManager().
            sceneManager.AddOnUnloadObserver([this]() { this->OnUnloadScene(); });
}

void Module::ObserveWindowResize()
{
    engine_.GetModuleManager().graphicsEngine.AddWindowResizeObserver(
        [this]() {
            this->
                OnWindowResize();
        });
}

void Module::ObserveMainLoopSubject(
    const observer::MainLoopSubject type,
    const std::function<void()>& callback) { engine_.AddObserver(type, callback); }
} //namespace poke
