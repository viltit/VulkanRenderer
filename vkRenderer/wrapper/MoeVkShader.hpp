#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <optional>

namespace moe {

class MoeVkLogicalDevice;

enum MoeVkShaderStage {
    vertex      = VK_SHADER_STAGE_VERTEX_BIT,
    fragment    = VK_SHADER_STAGE_FRAGMENT_BIT,
    geometry    = VK_SHADER_STAGE_GEOMETRY_BIT
};

class MoeVkShader {

public:
    MoeVkShader();
    ~MoeVkShader();

    MoeVkShader(const MoeVkShader&) = delete;
    MoeVkShader& operator = (const MoeVkShader&) = delete;

    void create(MoeVkLogicalDevice& device, const std::string& filename, const MoeVkShaderStage stage);
    void destroy();

    VkShaderModule& module()   { return _module; }
    MoeVkShaderStage stage() const  { return _stage; }

private:
    std::vector<char> readShader(const std::string& filename);
    VkShaderModule createShader(MoeVkLogicalDevice& device, const std::vector<char>& bytecode);

    VkShaderModule                          _module;
    MoeVkShaderStage                        _stage;
    MoeVkLogicalDevice*                     _device;
};
}