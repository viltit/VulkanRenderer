#include "MoeVkShader.hpp"

#include <fstream>

#include "MoeExceptions.hpp"
#include "MoeVkLogicalDevice.hpp"

namespace moe {

MoeVkShader::MoeVkShader() { }

MoeVkShader::~MoeVkShader() {
    destroy();
}

void MoeVkShader::create(MoeVkLogicalDevice &device, const std::string &filename,
                         const moe::MoeVkShaderStage stage) {
    _device = &device;
    auto byteCode = readShader(filename);
    _module = createShader(device, byteCode);
    _stage = stage;
}

void MoeVkShader::destroy() {
    if (_device) {
        vkDestroyShaderModule(_device->device(), _module, nullptr);
    }
}

std::vector<char> MoeVkShader::readShader(const std::string& filename) {

    // seek to the end of file when opening and read in binary mode:
    std::ifstream instream { filename, std::ios::ate | std::ios::binary };
    if (!instream) {
        throw MoeFileNotFoundError(filename, __FILE__, __FUNCTION__, __LINE__);
    }
    if (!instream.is_open()) {
        throw MoeFileNotOpenedError(filename, __FILE__, __FUNCTION__, __LINE__);
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


VkShaderModule MoeVkShader::createShader(MoeVkLogicalDevice& device, const std::vector<char> &bytecode) {

    VkShaderModuleCreateInfo createInfo { };
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext    = nullptr;
    createInfo.flags    = 0;
    createInfo.codeSize = bytecode.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(bytecode.data());

    VkShaderModule module;
    if (vkCreateShaderModule(device.device(), &createInfo, nullptr, &module) != VK_SUCCESS) {
        throw MoeInitError("Failed to create a shader module.", __FILE__, __FUNCTION__, __LINE__);
    }
    return module;
}
}