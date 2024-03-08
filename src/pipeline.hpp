#pragma once

#include <device.hpp>

// std
#include <string>
#include <vector>
#include <memory>

class Pipeline {

public:
    struct ConfigInfo {
        // ConfigInfo(const ConfigInfo&) = delete;
        // ConfigInfo& operator=(const ConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    Pipeline(std::shared_ptr<Device>& device,
            const std::string& vertShaderPath,
            const std::string& fragShaderPath,
            const Pipeline::ConfigInfo& configInfo);

    ~Pipeline();

    static void DefaultConfigInfo(Pipeline::ConfigInfo& configInfo);

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
