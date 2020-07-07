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
// Co-Author :
// Date : 31.01.20
//----------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>

#include <Scenes/scene.h>
#include <CoreEngine/settings.h>

namespace poke::scene {
class ISceneManager {
public:
	ISceneManager() = default;
	virtual ~ISceneManager() = default;

	virtual void AddScene(const Scene& scene) = 0;
	virtual bool RemoveScene(SceneIndex sceneIndex) = 0;
	virtual bool RemoveScene(std::string sceneName) = 0;
	virtual void LoadScene(std::string& name) = 0;
	virtual void LoadScene(SceneIndex sceneIndex) = 0;

	virtual std::vector<Scene>& GetScenes() = 0;
	virtual Scene& GetActiveScene() = 0;
	virtual SceneIndex GetActiveSceneIndex() const = 0;
	virtual SceneIndex GetHighestSceneIndex() const = 0;

	virtual void SaveActiveScene() const = 0;

	virtual void UnLoadActiveScene() = 0;

	//---------------------------------- OBSERVER ---------------------------------
	virtual void AddOnLoadObserver(const std::function<void()>& observerCallback) = 0;

	virtual void AddOnUnloadObserver(const std::function<void()>& observerCallback) = 0;
	//-----------------------------------------------------------------------------
protected:
	virtual void LoadActiveScene() = 0;
};
} //namespace poke::scene
