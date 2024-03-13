#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include <device.hpp>

class Texture {

public:
    Texture(std::shared_ptr<Device> device, const std::string& filePath);
    ~Texture();

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