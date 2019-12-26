#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "../Colors/ColorRGB.hpp"

namespace moe {

enum class WindowOptions {
    none            = 0x00,
    fullscreen      = 0x01,
    maximized       = 0x02,
    borderless      = 0x04
};

class VkWindow {
public:
    VkWindow(const std::string& name, uint width, uint height,
             const ColorRGB& background = ColorRGB::black(),
             WindowOptions options = WindowOptions ::none);

    ~VkWindow();

    SDL_Window* sdlWindow() const { return _window; }

    void resize(int w, int h) {
        SDL_SetWindowSize(_window, w, h);
    }

    void clear() const;
    void swap() const;

private:
    SDL_Window*     _window;
    uint            _w;
    uint            _h;
};
}