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
// Date : 20.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <string>

#include <Math/vector.h>
#include <Utility/color.h>

namespace poke {
namespace scene {

using SceneIndex = uint8_t;

/**
 * \brief This class loads/unloads a scene by instructing managers to load/unload resources. Has some basic data about a Scene.
 */
class Scene {
public:

	explicit Scene(
        SceneIndex sceneIndex,
		const std::string& sceneName = "sampleScene", // TODO @Oleg: ask Nico if this needs renaming
	    const std::string& fileName = "Default");
	
	~Scene() = default;

	/*
	* \brief Loads scene from json specified by Scene::fileName_ .
	*/
    void Load();
	/*
	* \brief Doesn't do anything currently.
	*/
    void UnLoad();
	/*
	* \brief Returns a .pokscene.
	*/
    json ToJson() const;

    //Getter / Setter
	/*
	* \brief Returns the scene's index. WARNING: not the same as InnerSceneManager::activeSceneIndex_ !
	*/
	SceneIndex GetSceneIndex() const { return sceneIndex_; }
	/*
	* \brief Sets the scene's index. WARNING: not the same as InnerSceneManager::activeSceneIndex_ !
	*/
    void SetSceneIndex(const SceneIndex index) { sceneIndex_ = index; }

	/*
	* \brief Returns the scene's name, does not necesserely match the .pokscene file name.
	*/
    const std::string& GetSceneName() const { return name_; }
	/*
	* \brief Returns the scene's .pokscene json path, without file extension.
	*/
    const std::string& GetSceneFileName() const { return fileName_; }

private:
	// Warning: not the same as InnerSceneManager::activeSceneIndex_ !
	SceneIndex sceneIndex_;

    std::string fileName_ = "Default"; // Without extension.
    std::string name_ = "sampleScene"; // Not necessarily the same as the file name.

	// Used for Scene::ToJson().
	std::vector<std::string> meshNames_;
	std::vector<std::string> materialNames_;
};
} //namespace scene
} //namespace poke
