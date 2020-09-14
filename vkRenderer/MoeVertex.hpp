#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace moe {


struct SimpleVertex {
    glm::vec3 pos;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription description { };
        description.binding = 0;
        description.stride = sizeof(SimpleVertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // vertex or instance for instanced rendering
        return description;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescription() {

        std::vector<VkVertexInputAttributeDescription> descriptions;
        descriptions.resize(2);

        descriptions[0].binding = 0;
        descriptions[0].location = 0;   // identical with shaders "layout(location=...)"
        descriptions[0].offset = offsetof(SimpleVertex, pos);
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[3].binding = 0;
        descriptions[3].location = 3;
        descriptions[3].offset = offsetof(SimpleVertex, uv);
        descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        return descriptions;
    }
};

// TODO long term: allow either colored OR textured vertices
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription description { };
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // vertex or instance for instanced rendering
        return description;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescription() {

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

        std::vector<VkVertexInputAttributeDescription> descriptions;
        descriptions.resize(4);

        descriptions[0].binding = 0;
        descriptions[0].location = 0;   // identical with shaders "layout(location=...)"
        descriptions[0].offset = offsetof(Vertex, pos);
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[1].binding = 0;
        descriptions[1].location = 1;
        descriptions[1].offset = offsetof(Vertex, color);
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[2].binding = 0;
        descriptions[2].location = 2;
        descriptions[2].offset = offsetof(Vertex, normal);
        descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[3].binding = 0;
        descriptions[3].location = 3;
        descriptions[3].offset = offsetof(Vertex, uv);
        descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        return descriptions;
    }
};
}
