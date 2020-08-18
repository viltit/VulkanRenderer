#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "MoeVertex.hpp"

namespace moe {
struct Drawable {
   // Drawable() { }
    Drawable(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : vertices { vertices },
          indices  { indices },
          M        { glm::mat4 { 1.f } }
    { }

    void update(int time) {
        M = glm::rotate(glm::mat4{ 1.f }, glm::radians(20.f * float(time) / 1000.f), glm::vec3{ 0.f, 0.f, 1.f});
    }

    // TODO: Model matrix should not be part of drawable when we want one drawable for different entities in the future
    glm::mat4 M;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};


}