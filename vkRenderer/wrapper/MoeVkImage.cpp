
#include "MoeVkImage.hpp"
// See https://github.com/nothings/stb
#include "../stb_image.h"

#include "../../Exceptions/InitException.hpp"
#include "MoeVkBuffer.hpp"
#include "MoeVkUtils.hpp"

#include <iostream>

// TODO: Differentiate erros. We do not need to shut down the app when a texture could not be loaded

namespace moe {

    MoeVkImage::MoeVkImage()
        : _w      {0 },
          _h     {0 },
          _channels   { 0 },
          _sampler      { VK_NULL_HANDLE },
          _device     { nullptr }
    {}

    MoeVkImage::MoeVkImage(const std::string &filename)
          : _sampler      { VK_NULL_HANDLE },
            _device     {nullptr }
    {
        load(filename);
    }

    MoeVkImage::~MoeVkImage() {
        destroy();
    }

    void MoeVkImage::load(const std::string &filename) {
        if (_pixels.has_value()) {
            throw InitException("Image was already loaded", __FILE__, __FUNCTION__, __LINE__);
        }
        // always add an alpha channel, even if the image does not have one
        _pixels = stbi_load(filename.c_str(), &_w, &_h, &_channels, STBI_rgb_alpha);
        if (_pixels == nullptr) {
            throw InitException("Failed to load image " + filename, __FILE__, __FUNCTION__, __LINE__);
        }
    }

    void MoeVkImage::upload(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
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

        if (vkCreateImage(device.device(), &imageInfo, nullptr, &_image) != VK_SUCCESS) {
            throw InitException("Failed to create image", __FILE__, __FUNCTION__, __LINE__);
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device.device(), _image, &memoryRequirements);
        VkMemoryAllocateInfo memoryInfo { };
        memoryInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryInfo.pNext            = nullptr;
        memoryInfo.allocationSize   = memoryRequirements.size;
        memoryInfo.memoryTypeIndex  = MoeVkUtils::getMemoryType(physicalDevice, memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (vkAllocateMemory(device.device(), &memoryInfo, nullptr, &_memory) != VK_SUCCESS) {
            throw InitException("Failed to allocate memory for texture", __FILE__, __FUNCTION__, __LINE__);
        }

        // bind memory to image
        // offset: We could put several images into one block of image memory
        vkBindImageMemory(device.device(), _image, _memory, 0);

        setLayout(commandPool, queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        transferBuffer(device, commandPool, queue, stagingBuffer);
        setLayout(commandPool, queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VkImageViewCreateInfo viewCreateInfo { };
        viewCreateInfo.sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.pNext        = nullptr;
        viewCreateInfo.flags        = 0;
        viewCreateInfo.image        = _image;
        viewCreateInfo.viewType     = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format       = VK_FORMAT_R8G8B8A8_UNORM;
        // we could switch channels here:
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        // TODO: Same code as in function "setLayout"
        viewCreateInfo.subresourceRange.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel    = 0;
        viewCreateInfo.subresourceRange.levelCount      = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer  = 0;
        viewCreateInfo.subresourceRange.layerCount      = 1;

        if (vkCreateImageView(device.device(), &viewCreateInfo, nullptr, &_view) != VK_SUCCESS) {
            throw InitException("Failed to create image view", __FILE__, __FUNCTION__, __LINE__);
        }

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

    void MoeVkImage::setLayout(MoeVkCommandPool& commandPool, VkQueue& queue, VkImageLayout layout) {

        MoeVkCommandBuffer buffer { };
        buffer.create(*_device, commandPool, 1);
        buffer.startRecording(commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 0);

        // start layouting the image. This happens with so called "image-memory-barrier".
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destStage;
        VkImageMemoryBarrier barrier;
        barrier.sType           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext           = nullptr;
        // transition from preloaded image to image an image we can copy into
        if (_layout == VK_IMAGE_LAYOUT_PREINITIALIZED && layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        // transition from image we copied into to shader optimized image
        else if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            throw InitException("Invalid layout transition", __FILE__, __FUNCTION__, __LINE__);
        }
        barrier.oldLayout       = _layout;
        barrier.newLayout       = layout;
        // we could change the queue an image belongs to here
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = _image;
        // TODO: Use mipmapping
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        vkCmdPipelineBarrier(buffer.at(0),
                sourceStage,
                destStage,
                0, 0, nullptr,
                0, nullptr,
                1,
                &barrier);

        buffer.stopRecording(queue, commandPool, 0);

        // TODO: Wait for fence ??  --> Would make sence if we access this class in a multihtreaded environment
        vkQueueWaitIdle(queue);
        buffer.destroy();
        _layout = layout;
    }

    void MoeVkImage::transferBuffer(MoeVkLogicalDevice& device, MoeVkCommandPool& pool, VkQueue& queue, MoeVkBuffer& stagingBuffer) {

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
                _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &imageCopy);

        commandBuffer.stopRecording(queue, pool, 0);

        // TODO: Wait for fence ??  --> Would make sence if we access this class in a multihtreaded environment
        vkQueueWaitIdle(queue);
    }

    void MoeVkImage::destroy() {
        if (_pixels.has_value()) {
            stbi_image_free(_pixels.value());
            _pixels.reset();
        }
        if (_sampler != VK_NULL_HANDLE) {
            vkDestroySampler(_device->device(), _sampler, nullptr);
        }
        if (_view != VK_NULL_HANDLE) {
            vkDestroyImageView(_device->device(), _view, nullptr);
        }
        if (_image != VK_NULL_HANDLE) {
            vkDestroyImage(_device->device(), _image, nullptr);
            vkFreeMemory(_device->device(), _memory, nullptr);
        }
    }
}