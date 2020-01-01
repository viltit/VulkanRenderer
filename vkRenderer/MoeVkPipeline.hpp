#pragma once

#include <vector>
#include <string>

#include "MoeVkLogicalDevice.hpp"

namespace moe {

class MoeVkPipeline {
public:
    MoeVkPipeline();
    ~MoeVkPipeline();

    void create(MoeVkLogicalDevice& device);

private:
    /// reads a compiled shader file (ie a .spv-file)
    std::vector<char> readShader(const std::string& filename);
    VkShaderModule createShader(MoeVkLogicalDevice& device, const std::vector<char>& bytecode);

};
}
