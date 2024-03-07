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

    static Pipeline::ConfigInfo DefaultConfigInfo(uint32_t width, uint32_t height);

    void Bind(VkCommandBuffer commandBuffer);

private:
    static std::vector<char> ReadFile(const std::string& filePath);

    void CreatePipeline(const std::string& vertShaderPath,
                        const std::string& fragShaderPath,
                        const Pipeline::ConfigInfo& configInfo);

    void CreateShaderModule(const std::vector<char>& code,
                            VkShaderModule* shaderModule);

    std::shared_ptr<Device> mDevice;
    VkPipeline mPipeline;
    VkShaderModule mVertShaderModule;
    VkShaderModule mFragShaderModule;
};
