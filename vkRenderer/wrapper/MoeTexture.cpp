
#include "MoeTexture.hpp"
// See https://github.com/nothings/stb
#include "../stb_image.h"

#include "../../Exceptions/InitException.hpp"
#include "MoeVkBuffer.hpp"
#include "MoeVkUtils.hpp"

#include <iostream>

// TODO: Differentiate erros. We do not need to shut down the app when a texture could not be loaded

namespace moe {

    MoeTexture::MoeTexture()
        : _w      {0 },
          _h     {0 },
          _channels   { 0 },
          _sampler      { VK_NULL_HANDLE },
          _device     { nullptr }
    {}

    MoeTexture::MoeTexture(const std::string &filename)
          : _sampler      { VK_NULL_HANDLE },
            _device     {nullptr }
    {
        load(filename);
    }

    MoeTexture::~MoeTexture() {
        destroy();
    }

    void MoeTexture::load(const std::string &filename) {
        if (_pixels.has_value()) {
            throw InitException("Image was already loaded", __FILE__, __FUNCTION__, __LINE__);
        }
        // always add an alpha channel, even if the image does not have one
        _pixels = stbi_load(filename.c_str(), &_w, &_h, &_channels, STBI_rgb_alpha);
        if (_pixels == nullptr) {
            throw InitException("Failed to load image " + filename, __FILE__, __FUNCTION__, __LINE__);
        }
    }

    void MoeTexture::upload(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
                            MoeVkCommandPool& commandPool, VkQueue& queue) {

        if (!_pixels.has_value()) {
            throw InitException("Trying to upload an image that was not loaded from disk yet", __FILE__, __FUNCTION__, __LINE__);
        }
        if (_sampler != VK_NULL_HANDLE) {
            throw InitException("Trying to upload an image that was already uploaded before", __FILE__, __FUNCTION__, __LINE__);
        }

        _device = &device;

        // As for vertices, we use a staging buffer for better performance here
        MoeVkBuffer stagingBuffer(physicalDevice, device, byteSize(),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                "image staging bugger");
        void* data;
        vkMapMemory(device.device(), stagingBuffer.memory(), 0, byteSize(), 0, &data);
        memcpy(data, _pixels.value(), byteSize());
        vkUnmapMemory(device.device(), stagingBuffer.memory());

        _image.create(device, physicalDevice, _w, _h,
                VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                _layout,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );


        _image.changeLayout(commandPool, queue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        transferBuffer(device, commandPool, queue, stagingBuffer);
        _image.changeLayout(commandPool, queue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        _image.createView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

        // create the sampler
        VkSamplerCreateInfo samplerInfo { };
        samplerInfo.sType           = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext           = nullptr;
        samplerInfo.flags           = 0;
        // TODO Impartant
        samplerInfo.magFilter       = VK_FILTER_LINEAR;
        samplerInfo.minFilter       = VK_FILTER_LINEAR;
        // TODO: Mipmapping
        samplerInfo.mipmapMode      = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU    = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV    = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW    = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.mipLodBias      = 0.f;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy   = 16;           // TODO: Paramterisize
        samplerInfo.compareEnable   = VK_FALSE;     // TODO when adding shadow maps
        samplerInfo.compareOp       = VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod          = 0;
        samplerInfo.maxLod          = 0;
        samplerInfo.borderColor     = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS) {
            throw InitException("Failed to create image sampler", __FILE__, __FUNCTION__, __LINE__);
        }

    }

    void MoeTexture::transferBuffer(MoeVkLogicalDevice& device, MoeVkCommandPool& pool, VkQueue& queue, MoeVkBuffer& stagingBuffer) {

        MoeVkCommandBuffer commandBuffer { };
        commandBuffer.create(*_device, pool, 1);
        commandBuffer.startRecording(pool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 0);

        // copy image from staging buffer to discrete memory
        VkBufferImageCopy imageCopy;
        imageCopy.bufferOffset         = 0;
        // TODO: For very big images do not use 0
        imageCopy.bufferRowLength      = 0;
        imageCopy.bufferImageHeight    = 0;
        imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // TODO: Add mipmapping
        imageCopy.imageSubresource.mipLevel   = 0;
        imageCopy.imageSubresource.baseArrayLayer = 0;
        imageCopy.imageSubresource.layerCount = 1;
        imageCopy.imageOffset = { 0, 0, 0};
        imageCopy.imageExtent = { static_cast<uint32_t >(_w), static_cast<uint32_t >(_h), 1 };

        vkCmdCopyBufferToImage(commandBuffer.at(0), stagingBuffer.buffer(),
                _image.image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &imageCopy);

        commandBuffer.stopRecording(queue, pool, 0);

        // TODO: Wait for fence ??  --> Would make sence if we access this class in a multihtreaded environment
        vkQueueWaitIdle(queue);
    }

    void MoeTexture::destroy() {
        if (_pixels.has_value()) {
            stbi_image_free(_pixels.value());
            _pixels.reset();
        }
        if (_sampler != VK_NULL_HANDLE) {
            vkDestroySampler(_device->device(), _sampler, nullptr);
        }
        _image.destroy();
    }
}