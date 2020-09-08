#include "MoeVkSurfaceSDL2.hpp"

#include <SDL_vulkan.h>
#include "../VkWindow.hpp"
#include "MoeVkInstance.hpp"
#include "MoeExceptions.hpp"

namespace moe {

MoeVkSurfaceSDL2::MoeVkSurfaceSDL2(MoeVkInstance& instance, VkWindow& window)
    : _instance { instance }
{
    if (!SDL_Vulkan_CreateSurface(window.sdlWindow(), instance.instance(), &_surface)) {
        throw MoeInitError("SDL Failed to create a vulkan surface. Message: " + std::string(SDL_GetError()),
                __FILE__, __FUNCTION__, __LINE__);
    }
}

MoeVkSurfaceSDL2::~MoeVkSurfaceSDL2() {
    vkDestroySurfaceKHR(_instance.instance(), _surface, nullptr);
}
}