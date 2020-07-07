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
// Date : 24.11.19
//-----------------------------------------------------------------------------
#pragma once

#include <functional>

namespace poke {
//-----------------------------FORWARD DECLARATION----------------------------------
namespace observer {
enum class MainLoopSubject : uint8_t;
}

class Engine;

//----------------------------------------------------------------------------------
/**
 * \brief Sub system of the Engine, is implemented using the main delegates systems
 */
class Module {
public:
    explicit Module(Engine& engine);
    virtual ~Module();

    /**
    * \brief Initialize the core module. Call only once
    */
    virtual void OnAppBuild() {}

	/**
	* \brief Initialize the core module. Call only once
	*/
	virtual void OnAppInit() {}

	/**
	* \brief Called when a new scene is loaded
	*/
	virtual void OnLoadScene() {}

	/**
	* \brief Called when a new scene is unloaded
	*/
	virtual void OnUnloadScene() {}

   	/**
	* \brief call when window is resized
	*/
	virtual void OnWindowResize() {}

	/**
    * \brief Destroy the core module. Call only once
    */
	virtual void OnEngineDestroy() {}

protected:
    /**
     * \brief Register as an observer for the Engine Init 
     */
    void ObserveEngineInit();
	/**
	 * \brief Register as an observer for the Engine Init
	 */
	void ObserveAppInit();
    /**
     * \brief Register as an observer for the Engine Destroy 
     */
    void ObserveEngineDestroy();

	/**
	 * \brief Register as an observer for the loading of a new scene 
	 */
	void ObserveLoadScene();

	/**
	 * \brief Register as an observer for unloading of the current scene
	 */
	void ObserveUnloadScene();

    /**
     * \brief Register as an observer for the window resize subject
     */
    void ObserveWindowResize();

    Engine& engine_;

private:
    void ObserveMainLoopSubject(
		observer::MainLoopSubject type, 
		const std::function<void()>& callback);

};
} //namespace poke
