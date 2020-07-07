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
// Date : 07.12.19
//-----------------------------------------------------------------------------
#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan/vulkan.h>

#include <Math/vector.h>
#include <CoreEngine/Observer/subjects_container.h>

namespace poke {

namespace observer {
enum class WindowSubject : uint8_t {
    //Called when the window is resized
    WINDOW_RESIZE = 0,
    LENGTH
};
}

class Engine;

namespace graphics {
/**
 * \brief It's the window created by SDL
 */
class Window {
public:
    /**
     * \brief
     * \param windowSize
     * \param windowName
     */
    explicit Window(
        math::Vec2Int windowSize = math::Vec2Int(600, 800),
        const char* windowName = "PokEngine");
    ~Window();

    operator const SDL_Window &() const { return *window_; }

    const SDL_Window& GetWindow() const { return *window_; }
    SDL_Window& GetWindow() { return *window_; }
    SDL_Window* GetWindowPtr() const { return window_; }

    /**
     * \brief Get all event
     */
    void PollEvents();

    /**
     * \brief Can be used to set in a waiting mod until next event
     */
    void WaitEvents();

    /**
     * \brief close the window and end the engine (close the window only in windows mode)
     */
    void ShutDown();

    /**
     * \brief Check if the window is still runnning
     * \return 
     */
    bool IsRunning() const;

    /**
     * \brief Get the height and width of the window
     * \return 
     */
    math::Vec2Int GetSize() const;

    /**
     * \brief Return the framebuffer current size. it also update the size stored in memory if it has changed
     */
    math::Vec2Int GetFramebufferSize();

    /**
     * \brief Link the window to Vulkan via a Surface.
     * \param instance 
     * \param allocator 
     * \param surface 
     */
    void CreateSurface(
        const VkInstance& instance,
        const VkAllocationCallbacks* allocator,
        VkSurfaceKHR* surface) const;

    void SetTitle(const char* newTitle);

	bool IsMinimized() const { return isMinimized_; }

	void SetMinimized(bool isMinimized) { isMinimized_ = isMinimized; }
    //---------------------------------- OBSERVER ---------------------------------
    /**
	 * \brief Add an observe for the resizing of the window
	 * \param observerCallback 
	 */
    void AddResizeObserver(std::function<void()> observerCallback);

    /**
     * \brief Notify the resize subject fo the window
     */
    void NotifyResize() const;
    //-----------------------------------------------------------------------------
private:
    void OnWindowResize();

    SDL_Window* window_ = nullptr;
    SDL_Event event_{};

    math::Vec2Int size_ = math::Vec2Int(0, 0);

    bool running_ = true;

	bool isMinimized_ = false;

    observer::SubjectsContainer<observer::WindowSubject> subjectsContainer_;
};
} //namespace graphics
} //namespace poke
