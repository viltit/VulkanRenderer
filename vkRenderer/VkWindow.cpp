#include "VkWindow.hpp"
#include "MoeExceptions.hpp"
#include <spdlog/spdlog.h>

namespace moe {

VkWindow::VkWindow(const std::string& name, unsigned int width, unsigned int height,
                           const ColorRGB& background, WindowOptions options)
        :   _window    { nullptr },
            _w         { width },
            _h         { height }
{
    spdlog::trace("Creating SDL2-Window...");
    //put together the desired window style with bitwise ops
    auto windowOptions = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;
    if (uint(WindowOptions::borderless) & uint(options)) {
        windowOptions |= SDL_WINDOW_BORDERLESS;
    }
    if (uint(WindowOptions::fullscreen) & uint(options)) {
        windowOptions |= SDL_WINDOW_FULLSCREEN;
    }
    if (uint(WindowOptions::maximized) & uint(options)) {
        windowOptions |= SDL_WINDOW_MAXIMIZED;
    }

    // TODO: Do not initialize SDL here -> we might want to add Audio Systems too
    if (SDL_Init(SDL_INIT_VIDEO) > 1) {
        throw MoeInitError("Could not initialize SDL Video.", __FILE__, __FUNCTION__, __LINE__);
    }

    //if the window is fullsize, adjust width and height:
    if (windowOptions & SDL_WINDOW_FULLSCREEN || windowOptions & SDL_WINDOW_MAXIMIZED) {
        //TODO: Maximized != Fullscreen !!
        SDL_DisplayMode resolution;
        SDL_GetDisplayMode(0, 0, &resolution);
        _w = resolution.w;
        _h = resolution.h;
    }

    _window = SDL_CreateWindow(
            name.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            _w, _h,
            windowOptions);
    if (!_window) {
        std::string message { "SDL failed to initialize a Window. Error message: "};
        std::string error { SDL_GetError() };
        throw MoeInitError(message + error, __FILE__, __FUNCTION__, __LINE__);
    }
    spdlog::trace("Finished.");
}

VkWindow::~VkWindow() {
    SDL_DestroyWindow(_window);
    _window = nullptr;

    // TODO: Do not quit SDL here
    SDL_Quit();
}

void VkWindow::clear() const {
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void VkWindow::swap() const {
    // SDL_GL_SwapWindow(_window);
}

}