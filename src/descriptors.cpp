#include <descriptors.hpp>

// std
#include <cassert>
#include <stdexcept>

// *************** Descriptor Set Layout Builder *********************

DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count) {
  assert(mBindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  mBindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const {
  return std::make_unique<DescriptorSetLayout>(mDevice, mBindings);
}

// *************** Descriptor Set Layout *********************

DescriptorSetLayout::DescriptorSetLayout(
    std::shared_ptr<Device> device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : mDevice{device}, mBindings{bindings} {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
  // Setting this flag tells the descriptor set layouts that no actual descriptor sets are allocated but instead pushed at command buffer creation time
	descriptorSetLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;

  if (vkCreateDescriptorSetLayout(
          mDevice->GetDevice(),
          &descriptorSetLayoutInfo,
          nullptr,
          &mDescriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(mDevice->GetDevice(), mDescriptorSetLayout, nullptr);
}

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(DescriptorSetLayout &setLayout)
    : mSetLayout{setLayout} {}

DescriptorWriter &DescriptorWriter::WriteBuffer(
    uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
  assert(mSetLayout.mBindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &bindingDescription = mSetLayout.mBindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;

  mWrites.push_back(write);
  return *this;
}

DescriptorWriter &DescriptorWriter::WriteImage(
    uint32_t binding, VkDescriptorImageInfo *imageInfo) {
  assert(mSetLayout.mBindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &bindingDescription = mSetLayout.mBindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;

  mWrites.push_back(write);
  return *this;
}

bool DescriptorWriter::Build(VkDescriptorSet &set) {
  // bool success = mPool.AllocateDescriptor(mSetLayout.GetDescriptorSetLayout(), set);
  // if (!success) {
  //   return false;
  // }
  // Overwrite(set);
  return true;
}

void DescriptorWriter::Overwrite(VkDescriptorSet &set) {
  // for (auto &write : mWrites) {
  //   write.dstSet = set;
  // }
  // vkUpdateDescriptorSets(mPool.mDevice->GetDevice(), mWrites.size(), mWrites.data(), 0, nullptr);
}
