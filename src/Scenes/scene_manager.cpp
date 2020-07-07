#include <Scenes/scene_manager.h>

#include <CoreEngine/engine.h>
#include <Utility/log.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke::scene {
InnerSceneManager::InnerSceneManager(
    Engine& engine)
    : ISceneManager(),
      subjectContainer_(
          {
              observer::SceneManagerSubjects::LOAD_SCENE,
              observer::SceneManagerSubjects::UNLOAD_SCENE
          },
          {})
{
    /*
    engine.AddObserver(
        observer::MainLoopSubject::APP_INIT,
        [this]() { this->OnAppInit(); });
    */
    engine.AddObserver(
	    observer::MainLoopSubject::ENGINE_INIT,
		[this]() {
		GraphicsEngineLocator::Get().GetEngine().AddObserver(
			observer::MainLoopSubject::END_FRAME,
			[this]() { this->OnEndOfFrame(); });
	});

	const std::vector<SceneSetting>& sceneSettings = engine.GetEngineSettings().GetSceneSettings();
	// If some additional scenes are to be loaded in addition to the ones in scenes_ already.
	if (!sceneSettings.empty()) {
		// Reserve to accomodate for additional scenes.
		scenes_.reserve(scenes_.size() + sceneSettings.size());
		for (auto& sceneSetting : sceneSettings) { // Generate new scenes from scene settings.
			scenes_.emplace_back(
				Scene(
					sceneSetting.index,
					sceneSetting.name,
					sceneSetting.fileName));
		}
	}
	else {
		scenes_.emplace_back(0);
	} //default scene
}

void InnerSceneManager::OnAppInit()
{
	const size_t sceneCount = scenes_.size();
	for (size_t i = 0; i < sceneCount; i++) {
        if(scenes_[i].GetSceneIndex() == 0) {

			activeSceneIndex_ = i;
        }
	}
    LoadActiveScene();
}

void InnerSceneManager::AddScene(const Scene& newScene)
{
    // Check if the scene is already imported or not.
    for(Scene scene : scenes_) {
        //Same filename means already loaded
        if(scene.GetSceneFileName() == newScene.GetSceneFileName() ||
			scene.GetSceneName() == newScene.GetSceneName()) {
			LogDebug("Scene '" + newScene.GetSceneFileName() + "' is aleady loaded !", LogType::SCENE_LOG);
			return;
        }
    }

	scenes_.emplace_back(newScene);   
}

void InnerSceneManager::LoadScene(std::string& name)
{
    //Switch active Scene
    for(size_t i = 0; i < scenes_.size(); i++){
        if (scenes_[i].GetSceneName() != name)
            continue;

        nextIndex_ = i;
        break;
    }
}

void InnerSceneManager::LoadScene(const SceneIndex sceneIndex)
{
	//Switch active Scene
	for (size_t i = 0; i < scenes_.size(); i++) {
		if (scenes_[i].GetSceneIndex() != sceneIndex)
			continue;

		nextIndex_ = i;
		break;
	}
}

Scene& InnerSceneManager::GetActiveScene()
{
    return scenes_[activeSceneIndex_];
}

SceneIndex InnerSceneManager::GetActiveSceneIndex() const {
    return scenes_[activeSceneIndex_].GetSceneIndex();
}

SceneIndex InnerSceneManager::GetHighestSceneIndex() const {
	SceneIndex highest = 0;
    for(const Scene& scene : scenes_) {
		if (scene.GetSceneIndex() > highest) {
			highest = scene.GetSceneIndex();
		}
    }
	return highest;
}

bool InnerSceneManager::RemoveScene(SceneIndex sceneIndex) {
    for(auto it = scenes_.begin(); it != scenes_.end(); ++it) {
		if (it->GetSceneIndex() == sceneIndex) {
			scenes_.erase(it);
			return true;
		}
    }
	return false;
}

bool InnerSceneManager::RemoveScene(std::string sceneName)
{
	for (auto it = scenes_.begin(); it != scenes_.end(); ++it) {
		if (it->GetSceneName() == sceneName) {
			scenes_.erase(it);
			return true;
		}
	}
	return false;
}

void InnerSceneManager::SaveActiveScene() const
{
    if (scenes_[activeSceneIndex_].GetSceneFileName().empty()) {
        LogError("Scene filename is not defined!");
        return;
    }

    const json sceneSave = scenes_[activeSceneIndex_].ToJson();

    PokFileSystem::WriteFile(
		scenes_[activeSceneIndex_].GetSceneFileName(),
        sceneSave,
        FileType::SCENE,
		FolderType::SAVE_IN_ROM);

	LogDebug("Scene " + scenes_[activeSceneIndex_].GetSceneFileName() + " saved !", LogType::SCENE_LOG);
}

void InnerSceneManager::UnLoadActiveScene()
{
    scenes_[activeSceneIndex_].UnLoad();
	EcsManagerLocator::Get().OnUnloadScene();
    subjectContainer_.NotifySubject(
        observer::SceneManagerSubjects::UNLOAD_SCENE);
}

void InnerSceneManager::AddOnLoadObserver(
    const std::function<void()>& observerCallback)
{
    subjectContainer_.AddObserver(
        observer::SceneManagerSubjects::LOAD_SCENE,
        observerCallback);
}

void InnerSceneManager::AddOnUnloadObserver(
    const std::function<void()>& observerCallback)
{
    subjectContainer_.AddObserver(
        observer::SceneManagerSubjects::UNLOAD_SCENE,
        observerCallback);
}

void InnerSceneManager::LoadActiveScene()
{
	scenes_[activeSceneIndex_].Load();

    subjectContainer_.NotifySubject(observer::SceneManagerSubjects::LOAD_SCENE);
}

void InnerSceneManager::OnEndOfFrame()
{
	// On each end of frame, if active scene is to be changed,
	// unload current scene and load next one.
    if(nextIndex_ != kEmptyNextIndex) {
		UnLoadActiveScene();

		activeSceneIndex_ = nextIndex_;

		LoadActiveScene();

		nextIndex_ = kEmptyNextIndex;
    }
}
} //namespace poke::scene
