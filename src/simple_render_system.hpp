#pragma once

#include <pipeline.hpp>
#include <device.hpp>
#include <model.hpp>

// std
#include <memory>
#include <vector>

class SimpleRenderSystem {

public:
    SimpleRenderSystem(std::shared_ptr<Device> device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    void RenderGameObjects(VkCommandBuffer commandBuffer, std::shared_ptr<Model> model);

private:
    void CreatePipelineLayout();
    void CreatePipeline(VkRenderPass renderPass);

    std::shared_ptr<Device> mDevice;

    std::unique_ptr<Pipeline> mPipeline;
    VkPipelineLayout mPipelineLayout;
};
