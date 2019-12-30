#include <iostream>
#include "vkRenderer/MoeVkRenderer.hpp"
#include "vkRenderer/VkWindow.hpp"
#include "Exceptions/InitException.hpp"

using namespace moe;

int main() {
    try {
        VkWindow window = VkWindow("Vulkan Barebones", 500, 500, ColorRGB::black());
        MoeVkRenderer vkApp = MoeVkRenderer(&window, RendererOptions::validation);

        bool isRunning = true;
        while(isRunning) {
            //window->clear();
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch(e.type) {
                    case SDL_QUIT:
                        isRunning = false;
                        break;
                    case SDL_WINDOWEVENT:
                        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                            std::cout << "Window resize\n";
                            std::cout << e.window.data1 << " / " << e.window.data2 << std::endl;
                            window.resize(e.window.data1, e.window.data2);
                            // vulkan->resize();
                        }
                        break;
                    case SDL_KEYDOWN:
                        switch(e.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                isRunning = false;
                                break;
                        }
                        break;
                }
            }
            // vulkan->draw();
        }
        SDL_Quit();


    }
    catch(InitException e) {
        std::cout << "Unhandeld error: " << e.what() << std::endl;
        std::cout << "In File " << e.file << " on Line " << e.line << " in function " << e.func << std::endl;
    }
    return 0;
}