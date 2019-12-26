#include <iostream>
#include "vkRenderer/MoeVkRenderer.hpp"
#include "vkRenderer/VkWindow.hpp"
#include "Exceptions/InitException.hpp"

using namespace moe;

int main() {
    try {
        VkWindow window = VkWindow("Vulkan Barebones", 500, 500, ColorRGB::black());
        MoeVkRenderer vkApp = MoeVkRenderer(&window, RendererOptions::validation);
    }
    catch(InitException e) {
        std::cout << "Unhandeld error: " << e.what() << std::endl;
        std::cout << "In File " << e.file << " on Line " << e.line << " in function " << e.func << std::endl;
    }
    return 0;
}