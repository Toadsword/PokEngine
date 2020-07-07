//----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author : Simon Canas
// Date : 31.01.20
//----------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>

#include <CoreEngine/Observer/subjects_container.h>
#include <Scenes/scene.h>
#include <Scenes/interface_scene_manager.h>

namespace poke {
class Engine;

namespace observer {
/**
 * \brief Subjects specifics to scene manager
 */
enum class SceneManagerSubjects : uint8_t {
    LOAD_SCENE = 0,
    UNLOAD_SCENE
};
} //namespace observer

namespace scene {
class InnerSceneManager final : public ISceneManager {
public:
    explicit InnerSceneManager(
        Engine& engine);

    ~InnerSceneManager() = default;

    void AddScene(const Scene& newScene) override;
	bool RemoveScene(SceneIndex sceneIndex) override;
	bool RemoveScene(std::string sceneName) override;
    void LoadScene(std::string& name) override;
    void LoadScene(SceneIndex sceneIndex) override;

    void SaveActiveScene() const override;

    void UnLoadActiveScene() override;

    //-------------------------------- OBSERVER -------------------------------
    void AddOnLoadObserver(const std::function<void()>& observerCallback) override;

    void AddOnUnloadObserver(const std::function<void()>& observerCallback) override;
    //-------------------------------------------------------------------------

    //---------------------------- GETTER / SETTER ----------------------------
	std::vector<Scene>& GetScenes() override { return scenes_; }

    Scene& GetActiveScene() override;

	SceneIndex GetActiveSceneIndex() const override;
	SceneIndex GetHighestSceneIndex() const override;
    //-------------------------------------------------------------------------
    void OnAppInit();

private:
    void LoadActiveScene() override;

    void OnEndOfFrame();

    std::vector<Scene> scenes_{}; // Only one active loaded scene from json at a time.

	size_t activeSceneIndex_ = 0; // Warning: this is equal to Scene::sceneIndex_ !
	size_t nextIndex_ = kEmptyNextIndex; // Next scene in the game, not in memory.

	inline static const size_t kEmptyNextIndex = -1;

    observer::SubjectsContainer<observer::SceneManagerSubjects> subjectContainer_;
};
} //namespace scene
} //namespace poke
