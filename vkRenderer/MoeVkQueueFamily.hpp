#pragma once

namespace moe {
    struct MoeVkQueueFamily {
        std::vector<unsigned int> graphics;
        std::vector<unsigned int> presentation;
        std::vector<unsigned int> numQueues;

        bool hasGraphics() const {
            return graphics.size() > 0;
        }
        bool hasPresentation() const {
            return presentation.size() > 0;
        }
    };
}