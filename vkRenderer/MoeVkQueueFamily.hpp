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

        std::vector<unsigned int> pickBest() {
            /*
            TODO: Take the best queueFamily !! What is the best ?
            Do not overthink: Just take a dedicated transfer queue if there is one
            */
            for (unsigned int graphicsIndex : graphics) {
                for (unsigned int presentationIndex : presentation) {
                    if (graphicsIndex == presentationIndex) {
                        return { graphicsIndex };
                    }
                }
            }
            // if we reach this point, there is no queue family that supports both graphics and presentation and we need to use two
            // distinct queues
            return { graphics[0], presentation[0] };
        }
    };
}