#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "MoeVertex.hpp"

namespace moe {
struct Drawable {
   // Drawable() { }
    Drawable(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
            const glm::vec3& pos = glm::vec3{ },
            const std::string& name = "")
        : vertices { vertices },
          indices  { indices },
          M        { glm::mat4 { 1.f } },
          name     { name },
          rotationSpeed { 1.f }
    {
        M = glm::translate(M, pos);
    }

    void update(int deltaTime) {
        M = glm::rotate(glm::mat4{ M }, glm::radians(20.f * float(deltaTime) * rotationSpeed / 1000.f), glm::vec3{ 0.f, 0.f, 1.f});
    }

    // TODO: Add a class "Transform" that supports translating, rotating, rotateSlow, lookAt, etc.
    glm::mat4 M;
    std::string name;

    // TODO: Just for testing
    float rotationSpeed;

    // TODO: Vertices and indices should be part of a class Mesh. Class drawable should only hold vulkan-stuff
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};


}