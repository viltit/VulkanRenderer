#pragma once

namespace moe {

    struct MoeVkQueueFamily {

        std::vector<unsigned int> graphics;
        std::vector<unsigned int> presentation;
        std::vector<unsigned int> numQueues;

        unsigned int selectedGraphicsIndex;
        unsigned int selectedPresentationIndex;

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
                        selectedGraphicsIndex = graphicsIndex;
                        selectedPresentationIndex = graphicsIndex;
                        return { graphicsIndex };
                    }
                }
            }
            // if we reach this point, there is no queue family that supports both graphics and presentation and we need to use two
            // distinct queues
            selectedGraphicsIndex = graphics[0];
            selectedPresentationIndex = presentation[0];
            return { graphics[0], presentation[0] };
        }
    };
}