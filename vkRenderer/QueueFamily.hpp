#pragma once
#include <vector>
#include <cstdint>

namespace moe {
namespace internal {

struct QueueFamily {

    std::vector<uint8_t> graphics;
    std::vector<uint8_t> presentation;
    std::vector<uint8_t> numQueues;

    bool hasGraphics() const {
        return graphics.size() > 0;
    }
    bool hasPresentation() const {
        return presentation.size() > 0;
    }
};
}}