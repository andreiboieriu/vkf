#pragma once

#include <device.hpp>

// std
#include <memory>
#include <unordered_map>
#include <vector>

class DescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(std::shared_ptr<Device> device) : mDevice{device} {}

        Builder &AddBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> Build() const;

    private:
        std::shared_ptr<Device> mDevice;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> mBindings{};
    };

    DescriptorSetLayout(std::shared_ptr<Device> device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    VkDescriptorSetLayout GetDescriptorSetLayout() const { return mDescriptorSetLayout; }

private:
    std::shared_ptr<Device> mDevice;
    VkDescriptorSetLayout mDescriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> mBindings;

    friend class DescriptorWriter;
};

class DescriptorWriter {
public:
    // DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool);
    DescriptorWriter(DescriptorSetLayout &setLayout);
    // DescriptorWriter() {}
    DescriptorWriter &WriteBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    DescriptorWriter &WriteImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    std::vector<VkWriteDescriptorSet> GetWrites() const { return mWrites; }

    bool Build(VkDescriptorSet &set);
    void Overwrite(VkDescriptorSet &set);

private:
    DescriptorSetLayout &mSetLayout;
    // DescriptorPool &mPool;
    std::vector<VkWriteDescriptorSet> mWrites;
};
