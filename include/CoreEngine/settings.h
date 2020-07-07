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
// Co-Author : Duncan Bourquard
// Date : 25.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <chrono>
#include <vector>

#include <Utility/file_system.h>
#include <Scenes/scene.h>

namespace poke {
enum class AppType : uint8_t {
    EDITOR = 0,
    GAME,
    SHOWROOM,
    LENGTH
};

struct SceneSetting {
	scene::SceneIndex index;
	std::string name;
	std::string fileName;
};

class EngineSetting {
public:
	EngineSetting() = default;
	EngineSetting(
		const std::string& projectName,
		AppType appType, 
		std::chrono::duration<double, std::milli> deltaTime, 
		int windowWidth, 
		int windowHeight, 
		const std::string& windowName,
		const std::vector<SceneSetting>& sceneSettings = {},
        const std::string& tagFileName = "",
		int availableToolFlag = 511,
		size_t defaultPoolSize = 8192);

	~EngineSetting() = default;

	/**
	 * \brief Load an EngineSetting from a given file name
	 * \param fileName name of the file
	 * \param fileType type of the file
	 * \param folderType type of the folder to look for the file
	 * \return true if successful, false otherwise
	 */
	bool LoadSettingsFromFile(
		const char* fileName, 
		FileType fileType, 
		FolderType folderType);
	/**
	 * \brief Load an EngineSetting from a json
	 * \param json data containing the EngineSettings
	 * \return true if successful, false otherwise
	 */
	bool LoadSettingsFromJson(json& json);
	/**
	 * \brief Get a premade Json with all the parameters of the engine.
	 * \return json data
	 */
	json SaveSettingsToJson() const;

	const std::string& GetProjectName() const { return projectName_; }
	AppType GetAppType() const { return appType_; }
	std::chrono::duration<double, std::milli> GetFrameRate() const { return frameRate_; }
	int GetWindowWidth() const { return windowWidth_; }
	int GetWindowHeight() const { return windowHeight_; }
	int GetAvailableToolsFlag() const { return availableToolFlag_; }
	const char* GetWindowName() const { return windowName_.c_str(); }
	void SetWindowName(std::string& newWindowName) { windowName_ = newWindowName; }
	const std::vector<SceneSetting>& GetSceneSettings() const { return scenesSetting_; }
	const std::string& GetTagFileName() const { return tagFileName_; }
	const size_t GetDefaultPoolSize() const { return defaultPoolSize_; }

private:

	std::string projectName_;

    /**
     * \brief type of the application, depending on the type, the engine will load different type of modules
     */
    AppType appType_ = AppType::EDITOR;

    /**
     * \brief frame rate in milliseconds
     */
    std::chrono::duration<double, std::milli> frameRate_ = std::chrono::duration<double, std::milli>(16.66f);
    //Window
    int windowWidth_ = 720;
    int windowHeight_ = 640;

    /**
     * \brief Name for the window
     */
	std::string windowName_ = "PokEngine window";

	// Setting tool available
	int availableToolFlag_ = 8191;
	std::vector<SceneSetting> scenesSetting_;

    //Tags
	std::string tagFileName_;

    //Entity pool size
	size_t defaultPoolSize_;
};

} // namespace poke
