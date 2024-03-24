#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include <device.hpp>

class Texture {

public:
    Texture(std::shared_ptr<Device> device, const std::string& filePath);
    ~Texture();

    VkDescriptorImageInfo DescriptorInfo() const {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mImageView;
        imageInfo.sampler = mSampler;

        return imageInfo;
    }

private:
    void CreateImage(const std::string& filePath);
    void CreateImageView();
    void CreateSampler();

    std::shared_ptr<Device> mDevice;

    VkImage mImage;
    VkDeviceMemory mImageMemory;
    VkImageView mImageView;
    VkSampler mSampler;
};
