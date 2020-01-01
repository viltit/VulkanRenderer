
#include "MoeVkPipeline.hpp"
#include "../Exceptions/InitException.hpp"

#include <fstream>

namespace moe {

MoeVkPipeline::MoeVkPipeline() { }

MoeVkPipeline::~MoeVkPipeline() { }

void MoeVkPipeline::create(MoeVkLogicalDevice& device) {

    // TODO later: Do not hardcode shader path
    auto vertexCode = readShader("Shaders/triangle.vert.spv");
    auto fragmentCode = readShader("Shaders/triangle.frag.spv");
    VkShaderModule vertexModule = createShader(device, vertexCode);
    VkShaderModule fragmentModule = createShader(device, fragmentCode);

    VkPipelineShaderStageCreateInfo vertexCreateInfo { };
    vertexCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexCreateInfo.pNext          = nullptr;
    vertexCreateInfo.flags          = 0;
    vertexCreateInfo.stage          = VK_SHADER_STAGE_VERTEX_BIT;
    vertexCreateInfo.module         = vertexModule;
    // This would allow for multiple shaders in one module:
    vertexCreateInfo.pName          = "main";   // TODO: Let the calling code decide the name of the entry function
    vertexCreateInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo fragmentCreateInfo { };
    fragmentCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentCreateInfo.pNext          = nullptr;
    fragmentCreateInfo.flags          = 0;
    fragmentCreateInfo.stage          = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentCreateInfo.module         = fragmentModule;
    fragmentCreateInfo.pName          = "main";   // TODO: Let the calling code decide the name of the entry function
    fragmentCreateInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexCreateInfo, fragmentCreateInfo };

    

    // TODO: These should be destroyed even if we get an Exception above !!
    vkDestroyShaderModule(device.device(), vertexModule, nullptr);
    vkDestroyShaderModule(device.device(), fragmentModule, nullptr);
}

std::vector<char> MoeVkPipeline::readShader(const std::string& filename) {

    // seek to the end of file when opening and read in binary mode:
    std::ifstream instream { filename, std::ios::ate | std::ios::binary };
    if (!instream) {
        throw InitException("Could not find file " + filename, __FILE__, __FUNCTION__, __LINE__);
    }
    if (!instream.is_open()) {
        throw InitException("Could not open file " + filename, __FILE__, __FUNCTION__, __LINE__);
    }

    // because we opened the file at its end, we can directly get the files size:
    size_t filesize = (size_t)instream.tellg();
    std::vector<char> buffer(filesize);

    // put the reader back at the files beginning:
    instream.seekg(0);
    instream.read(buffer.data(), filesize);
    instream.close();

    return buffer;
}

VkShaderModule MoeVkPipeline::createShader(MoeVkLogicalDevice& device, const std::vector<char> &bytecode) {

    VkShaderModuleCreateInfo createInfo { };
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext    = nullptr;
    createInfo.flags    = 0;
    createInfo.codeSize = bytecode.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(bytecode.data());

    VkShaderModule module;
    if (vkCreateShaderModule(device.device(), &createInfo, nullptr, &module) != VK_SUCCESS) {
        throw InitException("Failed to create a shader module.", __FILE__, __FUNCTION__, __LINE__);
    }

    return module;
}
}