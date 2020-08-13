#pragma once

#include <vector>
#include "MoeVertex.hpp"

namespace moe {
struct Drawable {
    Drawable() { }
    Drawable(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : vertices { vertices },
          indices  { indices }
    { }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};


}