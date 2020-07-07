#include <GraphicsEngine/Devices/window.h>
#include <CoreEngine/engine.h>
#include <CoreEngine/cassert.h>

namespace poke {
namespace graphics {
Window::Window(const math::Vec2Int windowSize, const char* windowName)
    : subjectsContainer_({observer::WindowSubject::WINDOW_RESIZE}, {})
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    size_ = windowSize;

#ifdef NN_SDK_BUILD_DEBUG
	size_ = math::Vec2Int(512, 320);
#endif

    window_ = SDL_CreateWindow(
        windowName,
        // window title 
        SDL_WINDOWPOS_CENTERED,
        // initial x position 
        SDL_WINDOWPOS_CENTERED,
        // initial y position
        size_.x,
        // width, in pixels 
        size_.y,
        // height, in pixels
        SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI |
        SDL_WINDOW_RESIZABLE
    );

    cassert(window_ != nullptr, "Could not create window: " << SDL_GetError());

    SDL_GetWindowData(window_, "PokEngine");

	subjectsContainer_.AddObserver(observer::WindowSubject::WINDOW_RESIZE, [this]() {OnWindowResize(); });
}

Window::~Window()
{
#ifndef NN_SDK_BUILD_DEBUG
    SDL_DestroyWindow(window_);
    SDL_Quit();
#endif
}

void Window::PollEvents() { SDL_PollEvent(&event_); }

void Window::WaitEvents() { SDL_WaitEvent(&event_); }

void Window::ShutDown() { running_ = false; }

bool Window::IsRunning() const { return running_; }

math::Vec2Int Window::GetSize() const { return size_; }

math::Vec2Int Window::GetFramebufferSize()
{
    SDL_Vulkan_GetDrawableSize(window_, &size_.x, &size_.y);

    return size_;
}

void Window::CreateSurface(
    const VkInstance& instance,
    const VkAllocationCallbacks* allocator,
    VkSurfaceKHR* surface) const
{
    SDL_Vulkan_CreateSurface(window_, instance, surface);
}

void Window::SetTitle(const char* newTitle)
{
    SDL_SetWindowTitle(window_, newTitle);
}

void Window::AddResizeObserver(const std::function<void()> observerCallback)
{
    subjectsContainer_.AddObserver(
        observer::WindowSubject::WINDOW_RESIZE,
        observerCallback);
}

void Window::NotifyResize() const
{
    subjectsContainer_.NotifySubject(observer::WindowSubject::WINDOW_RESIZE);
}

void Window::OnWindowResize()
{
	SDL_Vulkan_GetDrawableSize(window_, &size_.x, &size_.y);
}
} //namespace graphics
} //namespace poke
