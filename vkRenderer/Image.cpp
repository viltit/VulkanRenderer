
#include "Image.hpp"
// See https://github.com/nothings/stb
#include "stb_image.h"

#include "../Exceptions/InitException.hpp"
#include "wrapper/MoeVkBuffer.hpp"
#include "wrapper/MoeVkUtils.hpp"

// TODO: Differentiate erros. We do not need to shut down the app when a texture could not be loaded

namespace moe {

    Image::Image()
        : _w      {0 },
          _h     {0 },
          _channels   { 0 },
          _device     { VK_NULL_HANDLE },
          _image      { VK_NULL_HANDLE }
    {}

    Image::Image(const std::string &filename)
        : _device   { VK_NULL_HANDLE },
          _image      { VK_NULL_HANDLE }
    {
        load(filename);
    }

    Image::~Image() {
        destroy();
    }

    void Image::load(const std::string &filename) {
        if (_pixels.has_value()) {
            throw InitException("Image was already loaded", __FILE__, __FUNCTION__, __LINE__);
        }
        // always add an alpha channel, even if the image does not have one
        _pixels = stbi_load(filename.c_str(), &_w, &_h, &_channels, STBI_rgb_alpha);
        if (_pixels == nullptr) {
            throw InitException("Failed to load image " + filename, __FILE__, __FUNCTION__, __LINE__);
        }
    }

    void Image::upload(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
            MoeVkCommandPool& commandPool, VkQueue& queue) {

        if (!_pixels.has_value()) {
            throw InitException("Trying to upload an image that was not loaded from disk yet", __FILE__, __FUNCTION__, __LINE__);
        }
        if (_image != VK_NULL_HANDLE) {
            throw InitException("Trying to upload an image that was already uploaded before", __FILE__, __FUNCTION__, __LINE__);
        }

        _device = device.device();

        // As for vertices, we use a staging buffer for better performance here
        MoeVkBuffer stagingBuffer(physicalDevice, device, byteSize(),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data;
        vkMapMemory(_device, stagingBuffer.memory(), 0, byteSize(), 0, &data);
        memcpy(data, _pixels.value(), byteSize());
        vkUnmapMemory(_device, stagingBuffer.memory());

        VkImageCreateInfo imageInfo { };
        imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext             = nullptr;
        imageInfo.flags             = 0;
        imageInfo.imageType         = VK_IMAGE_TYPE_2D;
        imageInfo.format            = VK_FORMAT_R8G8B8A8_UNORM;     // format that stb image is loading
        imageInfo.extent.width      = static_cast<uint32_t>(_w);
        imageInfo.extent.height     = static_cast<uint32_t>(_h);
        imageInfo.extent.depth      = 1;
        imageInfo.mipLevels         = 1;  // TODO: Mipmapping
        imageInfo.arrayLayers       = 1;
        imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling            = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage             = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;  // to copy data from staging buffer
        imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.queueFamilyIndexCount = 0;
        imageInfo.pQueueFamilyIndices = nullptr;
        imageInfo.initialLayout     = _layout;

        if (vkCreateImage(_device, &imageInfo, nullptr, &_image) != VK_SUCCESS) {
            throw InitException("Failed to create image", __FILE__, __FUNCTION__, __LINE__);
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(_device, _image, &memoryRequirements);
        VkMemoryAllocateInfo memoryInfo { };
        memoryInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryInfo.pNext            = nullptr;
        memoryInfo.allocationSize   = memoryRequirements.size;
        memoryInfo.memoryTypeIndex  = MoeVkUtils::getMemoryType(physicalDevice, memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (!vkAllocateMemory(_device, &memoryInfo, nullptr, &_memory) != VK_SUCCESS) {
            throw InitException("Failed to allocate memory for texture", __FILE__, __FUNCTION__, __LINE__);
        }

        // bind memory to image
        // offset: We could put several images into one block of image memory
        vkBindImageMemory(_device, _image, _memory, 0);

    }

    void Image::setLayout(MoeVkCommandPool& commandPool, VkQueue& queue, VkImageLayout& layout) {

        VkCommandBufferAllocateInfo bufferAllocInfo { };

    }

    void Image::destroy() {
        if (_pixels.has_value()) {
            stbi_image_free(_pixels.value());
            _pixels.reset();
        }
    }
}