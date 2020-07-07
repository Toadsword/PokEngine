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
// Co-Author :
// Date : 08.01.20
//----------------------------------------------------------------------------------
#pragma once

#include <memory>

#include <CoreEngine/engine_application.h>
#include <CoreEngine/Observer/subject.h>
#include <Game/app_systems_container.h>
#include <Game/ResourcesManager/resource_managers_container.h>
#include <Game/Ecs/game_ecs_manager.h>
#include <Game/Ecs/game_archetypes_manager.h>
#include <Game/prefabs_container.h>

namespace poke {
//-----------------------------FORWARD DECLARATION----------------------------------
class Engine;

//----------------------------------------------------------------------------------
namespace game {
class Game : public EngineApplication {
public:
    //----------------------------------CONSTRUCTOR-------------------------------------
    explicit Game(Engine& engine, const std::string& fileName);
    ~Game() = default;
    //----------------------------------------------------------------------------------

    //---------------------------------EVENT OVERRIDE-----------------------------------
    void Build() override;
    void OnUpdate();
    //----------------------------------------------------------------------------------

    //---------------------------------APP OVERRIDE-------------------------------------
    void LoadApp() override;
    void Pause() override;
    void Run() override;
    void Stop() override;
    void RegisterObserverUpdate(std::function<void()> callback) override;
    void RegisterObserverPhysicsUpdate(std::function<void()> callback) override;
    void RegisterObserverDraw(std::function<void()> callback) override;
    void RegisterObserverCulling(std::function<void()> callback) override;
    void RegisterObserverRender(std::function<void()> callback) override;
    void RegisterObserverEndFrame(std::function<void()> callback) override;
    void RegisterObserverInput(std::function<void()> callback) override;

    void NotifyUpdate() const;

    void NotifyPhysicsUpdate() const;

    void NotifyDraw() const;

    void NotifyCulling() const;

    void NotifyRender() const;

    void NotifyEndFrame() const;

    void NotifyInput() const;

	ResourceManagersContainer& GetResourceManagersContainer() {
		return resourcesManagerContainer_;
	}
	GameArchetypesManager& GetGameArchetypesManager() {
		return gameArchetypesManager_;
	}
    //----------------------------------------------------------------------------------

    bool IsPaused() const { return state_ == AppState::PAUSE; }

    void UnPause() { state_ = AppState::RUNNING; }
private:
	AppSystemsContainer appSystemsContainer_;
	//Callbacks
	observer::Subject<> subjectAppBuild_;
	observer::Subject<> subjectAppInit_;
	observer::Subject<> subjectPhysicsUpdate_;
	observer::Subject<> subjectUpdate_;
	observer::Subject<> subjectDraw_;
	observer::Subject<> subjectCulling_;
	observer::Subject<> subjectRender_;
	observer::Subject<> subjectEndFrame_;
	observer::Subject<> subjectInputs_;

    //ResourceManager
    ResourceManagersContainer resourcesManagerContainer_;

	GameArchetypesManager gameArchetypesManager_;

	GameEcsManager gameEcsManager_;
};
} //namespace game
} //namespace poke
