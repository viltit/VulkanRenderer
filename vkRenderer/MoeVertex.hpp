#pragma once

#include <glm/glm.hpp>
#include <array>

namespace moe {
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    // TODO: Move this elsewhere.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription description { };
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // vertex or instance for instanced rendering
        return description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription() {

        /*
        format options:
        float: VK_FORMAT_R32_SFLOAT
        vec2: VK_FORMAT_R32G32_SFLOAT
        vec3: VK_FORMAT_R32G32B32_SFLOAT
        vec4: VK_FORMAT_R32G32B32A32_SFLOAT
        ivec2: VK_FORMAT_R32G32_SINT, a 2-component vector of 32-bit signed integers
        uvec4: VK_FORMAT_R32G32B32A32_UINT, a 4-component vector of 32-bit unsigned integers
        double: VK_FORMAT_R64_SFLOAT,
         */

        std::array<VkVertexInputAttributeDescription,2> descriptions;
        descriptions[0].binding = 0;
        descriptions[0].location = 0;   // identical with shaders "layout(location=...)"
        descriptions[0].offset = offsetof(Vertex, pos);
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[1].binding = 0;
        descriptions[1].location = 1;
        descriptions[1].offset = offsetof(Vertex, color);
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        return descriptions;
    }
};
}