#pragma once

#include <pipeline.hpp>
#include <device.hpp>
#include <model.hpp>
#include <texture.hpp>
#include <descriptors.hpp>
#include <buffer.hpp>

// std
#include <memory>
#include <vector>

#include <components/renderable.hpp>
#include <core/system.hpp>

class SimpleRenderSystem : public System {

public:
    struct Ubo {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
    };

    struct VertexPushData {
        alignas(16) glm::mat4 model;
    };

    struct FragmentPushData {
        alignas(16) glm::vec3 color;
        alignas(16) float opacity;
    };

    SimpleRenderSystem();
    ~SimpleRenderSystem();

    void Init(std::shared_ptr<Device> device, VkRenderPass renderPass);

    void Render(VkCommandBuffer commandBuffer, int frameIndex);

private:
    void CreateDescriptorSetLayouts();
    void CreateUniformBuffers();
    void CreatePipelineLayout();
    void CreatePipeline(VkRenderPass renderPass);

    std::shared_ptr<Device> mDevice;

    std::unique_ptr<Pipeline> mPipeline;
    VkPipelineLayout mPipelineLayout;

    std::unique_ptr<DescriptorSetLayout> mDescriptorSetLayout;

    std::vector<std::unique_ptr<Buffer>> mUboBuffers;
};
