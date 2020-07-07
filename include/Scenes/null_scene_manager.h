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
// Author : Simon Canas
// Co-Author : Nicolas Schneider
// Date : 20.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>

#include <Scenes/scene.h>
#include <Scenes/interface_scene_manager.h>

namespace poke::scene {
class NullSceneManager final : public ISceneManager
{
public:
	NullSceneManager() = default;
	~NullSceneManager() override = default;

    void AddScene(const Scene& scene) override
    {
		scene;
    }

	bool RemoveScene(SceneIndex sceneIndex) override {
		sceneIndex;
		cassert(false, "Impossible to remove scene from a null SceneManager");
		return false;
    }
	bool RemoveScene(std::string sceneName) override {
		sceneName;
		cassert(false, "Impossible to remove scene from a null SceneManager");
		return false;
	}
	void LoadScene(std::string& name) override
    {
		name;
    }
	void LoadScene(SceneIndex sceneIndex) override
    {
		sceneIndex;
    }

	std::vector<Scene>& GetScenes() override { 
		cassert(false, "Impossible to get scenes from a null SceneManager");
		abort();
	}
	Scene& GetActiveScene() override {
		cassert(false, "Impossible to get active scene from a null SceneManager");
		abort();
	}
	SceneIndex GetActiveSceneIndex() const override	{
		return 0;
	}
	SceneIndex GetHighestSceneIndex() const override {
		return 0;
    }    

	void SaveActiveScene() const override {};

	void UnLoadActiveScene() override {}

	//---------------------------------- OBSERVER ---------------------------------
	void AddOnLoadObserver(const std::function<void()>& observerCallback) override
	{
		observerCallback;
	}

	void AddOnUnloadObserver(const std::function<void()>& observerCallback) override
	{
		observerCallback;
	}
	//-----------------------------------------------------------------------------

protected:
    void LoadActiveScene() override {}
};
} //namespace poke::scene
