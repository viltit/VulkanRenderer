#pragma once

#include <vulkan/vulkan.h>

namespace moe {

class MoeVkInstance;
class VkWindow;

class MoeVkSurfaceSDL2 {
public:
    MoeVkSurfaceSDL2(MoeVkInstance& instance, VkWindow& window);

    ~MoeVkSurfaceSDL2();

    VkSurfaceKHR surface() { return _surface; }

private:
    MoeVkInstance&  _instance;
    VkSurfaceKHR    _surface;
};
}