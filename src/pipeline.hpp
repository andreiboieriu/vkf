#pragma once

#include <device.hpp>

// std
#include <string>
#include <vector>
#include <memory>

class Pipeline {

public:
  struct ConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };

  Pipeline(std::shared_ptr<Device>& device,
            const std::string& vertShaderPath,
            const std::string& fragShaderPath,
            const Pipeline::ConfigInfo& configInfo);

  ~Pipeline();

  static Pipeline::ConfigInfo defaultConfigInfo(uint32_t width, uint32_t height);

private:
  static std::vector<char> readFile(const std::string& filePath);

  void createPipeline(const std::string& vertShaderPath,
                      const std::string& fragShaderPath,
                      const Pipeline::ConfigInfo& configInfo);

  void createShaderModule(const std::vector<char>& code,
                          VkShaderModule* shaderModule);

  std::shared_ptr<Device> mDevice;
  VkPipeline mPipeline;
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;
};