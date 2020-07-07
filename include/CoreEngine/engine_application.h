//-----------------------------------------------------------------------------
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
// Date : 18.11.19
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include <CoreEngine/module.h>
#include <CoreEngine/Camera/core_camera.h>

namespace poke {
//-----------------------------FORWARD DECLARATION-----------------------------
class Engine;

//-----------------------------------------------------------------------------

enum class AppState : uint8_t {
    RUNNING = 0,
    PAUSE,
    STOP
};

/**
 * \brief Engine application, specific execution of the application is in the engine application
 */
class EngineApplication : public Module {
public:
    explicit EngineApplication(Engine& engine, const std::string& fileName);
    ~EngineApplication() = default;

    virtual void LoadApp() = 0;
    virtual void Build() = 0;

    virtual void Pause() = 0;
    virtual void Run() = 0;
    virtual void Stop() = 0;

    virtual void RegisterObserverUpdate(std::function<void()> callback) = 0;

    virtual void RegisterObserverPhysicsUpdate(std::function<void()> callback) = 0;

    virtual void RegisterObserverDraw(std::function<void()> callback) = 0;

    virtual void RegisterObserverCulling(std::function<void()> callback) = 0;

    virtual void RegisterObserverRender(std::function<void()> callback) = 0;

    virtual void RegisterObserverEndFrame(std::function<void()> callback) = 0;

    virtual void RegisterObserverInput(std::function<void()> callback) = 0;

    AppState GetState() const { return state_; }

	CoreCamera* GetCameraPtr() { return &camera_; }
protected:
    std::string appFileName_;

    AppState state_;

    CoreCamera camera_;
};
} //namespace poke
