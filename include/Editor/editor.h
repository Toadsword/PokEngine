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
// Co-Author : Luca Floreau / Duncan Bourquard
// Date : 28.03.20
//----------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <memory>

#include <imgui.h>

#include <CoreEngine/engine_application.h>
#include <CoreEngine/engine.h>
#include <Editor/Tools/tool.h>
#include <Inputs/input_manager.h>
#include <Scenes/scene.h>
#include <Scenes/scene_manager.h>
#include <Editor/editor_utility.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Editor/ResourcesManagers/resource_managers_container.h>
#include <Editor/Ecs/editor_ecs_manager.h>
#include <Game/game.h>
#include <CoreEngine/Camera/core_camera.h>

namespace poke {
//-----------------------------FORWARD DECLARATION-----------------------------
class Engine;

//-----------------------------------------------------------------------------
namespace editor {
class Editor : public EngineApplication
{
public:
	//----------------------------------CONSTRUCTOR----------------------------
	explicit Editor(Engine& engine, const std::string& fileName);
	~Editor();

	void Build() override;
	//-------------------------------------------------------------------------

	//---------------------------------EVENT OVERRIDE--------------------------
	void OnDrawImGui();
	void OnAppBuild();
	void OnUpdate();
	void OnEndOfFrame();
	//-------------------------------------------------------------------------

	//---------------------------------APP OVERRIDE----------------------------
	void LoadApp() override;
	void Pause() override;
	void Run() override;
	void Stop() override;
	//-------------------------------------------------------------------------

	//----------------------------------UTILITY--------------------------------
	Engine& GetEngine() const { return engine_; }

	game::Game& GetGame() { return game_; }

	ResourcesManagerContainer& GetResourcesManagerContainer();

	EditorEcsManager& GetEditorEcsManager();

	/**
     * \brief Start the game.
     */
	void StartGame();
	/**
     * \brief Stop the game. 
     */
	void StopGame();
	/**
	 * \brief Pause the game.
	 */
	void PauseGame();
	//-------------------------------------------------------------------------

	//--------------------------------OBSERVERS--------------------------------
	void RegisterObserverUpdate(std::function<void()> callback) override;

	void RegisterObserverPhysicsUpdate(std::function<void()> callback) override;

	void RegisterObserverDraw(std::function<void()> callback) override;

	void RegisterObserverDrawImGui(std::function<void()> callback);

	void RegisterObserverCulling(std::function<void()> callback) override;

	void RegisterObserverRender(std::function<void()> callback) override;

	void RegisterObserverEndFrame(std::function<void()> callback) override;

	void RegisterObserverInput(std::function<void()> callback) override;
	//-------------------------------------------------------------------------

	ecs::EntityIndex GetSelectedEntityIndex() const {
		return selectedEntityIndex_;
	}
    void SetSelectedEntityIndex(ecs::EntityIndex newIndex) {
		selectedEntityIndex_ = newIndex;
	}
private:
	Engine& engine_;

	std::vector<std::unique_ptr<Tool>> tools_;

	ImGuiDockNodeFlags dockSpaceFlags_;
	ImGuiWindowFlags windowFlags_;

	ToolFlags toolFlagsPrefs_;
    ecs::EntityIndex selectedEntityIndex_ = 0;

    bool showMainWindow_ = true;
    bool showDemoWindow_ = false;
	bool showAddSceneMenu_ = false;

	bool mustStartGame_ = false;
	bool mustStopGame_ = false;

	input::InputManager& inputManager_;
	scene::InnerSceneManager& sceneManager_;

	//Control Parameter
	math::Vec2 dragPosition_;

	const float kCameraSpeed_ = 0.5f;
	const float kCameraSpeedModifier_ = 2.0f;
	const float kCameraMaxOrientation_ = 89.0f;

	//Scene Option
	std::vector<scene::Scene>& scenes_;

	int sceneToLoad_;

	std::string newSceneName_ = "";

	ResourcesManagerContainer resourcesManagerContainer_;
	game::Game game_;

	EditorEcsManager editorEcsManager_;

	CoreCamera gameCameraCopy_;

	//Callbacks
	observer::Subject<> subjectAppBuild_;
	observer::Subject<> subjectAppInit_;
	observer::Subject<> subjectPhysicsUpdate_;
	observer::Subject<> subjectUpdate_;
	observer::Subject<> subjectDraw_;
	observer::Subject<> subjectDrawImGui_;
	observer::Subject<> subjectCulling_;
	observer::Subject<> subjectRender_;
	observer::Subject<> subjectEndFrame_;
	observer::Subject<> subjectInputs_;
};
} //namespace editor
} //namespace poke
