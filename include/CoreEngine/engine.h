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
// Date : 15.12.19
//----------------------------------------------------------------------------------
#pragma once

#include <functional>

#include <CoreEngine/module_container.h>
#include <CoreEngine/Observer/subjects_container.h>
#include <CoreEngine/settings.h>
#include <Utility/worker_thread.h>
#include <CoreEngine/engine_application.h>
#include <CoreEngine/core_systems_container.h>
#include <CoreEngine/World/world.h>

namespace poke {
namespace observer {
enum class MainLoopSubject : uint8_t {
    //----------------------------- ENGINE SPECIFIC -------------------------------

    //Called once a the engine constructor
    ENGINE_BUILD = 0,
    //Called once on the app construction
    APP_BUILD,
    //Called once on the engine initialization
    ENGINE_INIT,
    //Called once on the app initialization 
    APP_INIT,
    //Called once on the engine destroy
    ENGINE_DESTROY,
    //-----------------------------------------------------------------------------

    //---------------------------- MAIN UPDATE LOOP -------------------------------

    //Called every frame
    UPDATE,
    //Called when needed to update physics
    PHYSICS_UPDATE,
    //Called every frame when drawing
    DRAW,
	//Called every frame when rendering
	CULLING,
    //Called every frame when rendering
    RENDER,
    //Called every end of frame
    END_FRAME,
    //-----------------------------------------------------------------------------

    //---------------------------------- INPUTS -----------------------------------

    //User inputs
    //Called every frame after input's capture
    INPUTS,
    //-----------------------------------------------------------------------------

    LENGTH
};
} //namespace observer

enum class ThreadType : uint8_t {
    MAIN,
    RENDER,
    WORKER
};

/**
 * \brief Main class operating app
 */
class Engine final {
public:
    /**
     * \brief Constructor of engine with pre-made engine settings
     * \param engineSettings 
     */
    Engine(const EngineSetting& engineSettings);

    ~Engine();

	/**
     * \brief Called once to initialize the systems necessary for the engine.
     */
	void Build();
    /**
     * \brief Called once to initialize the engine and cores module.
     */
    void Init();
    /**
     * \brief Called to run the engine after the initialization
     */
    void Run();
    /**
     * \brief Shut down the engine and force to stop the main loop
     */
    void Stop() const;
    /**
     * \brief Assure that the engine and all cores modules are destroy correctly
     */
    void Destroy();

    /**
     * \brief 
     * \param callbackType type of observerCallback @CallbackType
     * \param observerCallback function
     */
    void AddObserver(
        observer::MainLoopSubject callbackType,
        const std::function<void()>& observerCallback);

    /**
     * \brief Register a specific callback
     * \param type
     */
    void Notify(observer::MainLoopSubject type) const;

    /**
     * \brief Add task for worker.
     * \param task 
     */
    void AddTask(const std::function<void()>& task);

    void AddAsync(std::function<void()> function, ThreadType type);

    void AddSync(std::function<void()> function, ThreadType type);

    void Wait(ThreadType type);

    //Getters
    ModuleContainer& GetModuleManager();

    const EngineSetting& GetEngineSettings() const { return engineSettings_; }

    void SetApp(std::unique_ptr<EngineApplication>&& app);

	EngineApplication& GetApp() { return *app_; }
private:
    EngineSetting engineSettings_;

    //Threads
    WorkerThread mainThread_;
    WorkerThread drawThread_;
    WorkerThread workerThread_;

    //Callbacks
    observer::SubjectsContainer<observer::MainLoopSubject> subjectsContainer_;

    //Modules
    ModuleContainer moduleContainer_;

    //Core Systems
	CoreSystemsContainer coreSystemsContainer_;

    //Application launched by the engine
    std::unique_ptr<EngineApplication> app_;

    //Running values
    bool isRunning_;

	World world_;
};
} //namespace poke
