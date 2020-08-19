#include <iostream>
#include "vkRenderer/MoeVkRenderer.hpp"
#include "vkRenderer/VkWindow.hpp"
#include "Exceptions/InitException.hpp"
#include "Timer.hpp"
#include "vkRenderer/MoeDrawable.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

using namespace moe;

Drawable getCube(glm::vec3 size);

int main(int argc, char* argv[]) {

    // TODO long term: Map allowed arguments to enum
    if (argc > 1) {
        std::string argument = argv[1];
        if (argument == "debug") {
            spdlog::set_level(spdlog::level::debug);
        }
        else if (argument == "trace") {
            spdlog::set_level(spdlog::level::trace);
        }
        else if (argument == "info") {
            spdlog::set_level(spdlog::level::info);
        }
    }
    else {
        spdlog::set_level(spdlog::level::warn);
    }

    try {
        spdlog::info("App is starting");

        Drawable drawable = getCube(glm::vec3{ 1.f, 1.f, 1.f });

        VkWindow window = VkWindow("Vulkan Barebones", 500, 500, ColorRGB::black());
        MoeVkRenderer vkApp = MoeVkRenderer(&window, drawable, RendererOptions::validation);

        Timer timer;
        timer.start();

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
                            // TODO: Calling resize leads to unexpected window height growth -> it seems unnecessary??
                            // window.resize(e.window.data1, e.window.data2);
                            // vulkan->resize();
                            // TODO: Some drivers may not set VK_ERROR_OUT_OF_DATE_KHR when the window resized -> explicitly tell the renderer
                        }
                        else if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                            // skip drawing
                            std::cout << "Window minimized\n";
                            continue;
                        }
                        // TODO: Window hidden?
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
            auto time = timer.elapsed();
            drawable.update(time);
            vkApp.draw();
        }
        spdlog::info("Shutting down.");
        // SDL_Quit();


    }
    catch(InitException e) {
        std::cout << "Unhandeld error: " << e.what() << std::endl;
        std::cout << "In File " << e.file << " on Line " << e.line << " in function " << e.func << std::endl;
    }
    return 0;
}


Drawable getCube(glm::vec3 size) {

    //we will need this variables at a later point:
    float w = size.x / 2.0f;
    float h = size.y / 2.0f;
    float d = size.z / 2.0f;

    float uv_w = 1.f;
    float uv_h = 1.f;

    std::vector<Vertex> vertices;
    vertices.resize(36);
    size_t index = 0;

    vertices[index].pos = glm::vec3{-w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };	/* front */
    vertices[index].pos = glm::vec3{w, -h, d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };
    vertices[index].pos = glm::vec3{w, h, d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };
    vertices[index].pos = glm::vec3{w, h, d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };
    vertices[index].pos = glm::vec3{-w, h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{-w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };

    vertices[index].pos = glm::vec3{-w, h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };	/* left */
    vertices[index].pos = glm::vec3{-w, h, -d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };
    vertices[index].pos = glm::vec3{-w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };
    vertices[index].pos = glm::vec3{-w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };
    vertices[index].pos = glm::vec3{-w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };
    vertices[index].pos = glm::vec3{-w, h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };

    vertices[index].pos = glm::vec3{w, h, -d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };	/* back */
    vertices[index].pos = glm::vec3{w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };
    vertices[index].pos = glm::vec3{-w, -h, -d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };
    vertices[index].pos = glm::vec3{-w, -h, -d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };
    vertices[index].pos = glm::vec3{-w, h, -d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{w, h, -d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };

    vertices[index].pos = glm::vec3{w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };	/* right */
    vertices[index].pos = glm::vec3{w, h, -d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };
    vertices[index].pos = glm::vec3{w, h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{w, h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };
    vertices[index].pos = glm::vec3{w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };

    vertices[index].pos = glm::vec3{w, h, d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };	/* top */
    vertices[index].pos = glm::vec3{w, h, -d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };
    vertices[index].pos = glm::vec3{-w, h, -d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{-w, h, -d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{-w, h, d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };
    vertices[index].pos = glm::vec3{w, h, d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };

    vertices[index].pos = glm::vec3{-w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };	/* bottom */
    vertices[index].pos = glm::vec3{w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, uv_h };
    vertices[index].pos = glm::vec3{w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, uv_h };
    vertices[index].pos = glm::vec3{-w, -h, d }; vertices[index++].uv = glm::vec2{0.0f, 0.0f };
    vertices[index].pos = glm::vec3{-w, -h, -d }; vertices[index++].uv = glm::vec2{uv_w, 0.0f };

    std::vector<uint32_t> indices;
    indices.resize(36);
    for (size_t i = 0; i < 36; i++) {
        indices[i] = i;
    }

    return Drawable(vertices, indices);
}