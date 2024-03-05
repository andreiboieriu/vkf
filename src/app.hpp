#pragma once

#include <window.hpp>
#include <pipeline.hpp>
#include <device.hpp>
#include <swap_chain.hpp>

// std
#include <memory>
#include <vector>

class App {

public:
    static constexpr uint32_t WIDTH = 1280;
    static constexpr uint32_t HEIGHT = 720;
    static constexpr std::string NAME = "Vulkan";

    App();
    ~App();

    void run();

private:
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<Pipeline> mPipeline;
    std::shared_ptr<SwapChain> mSwapChain;

    VkPipelineLayout mPipelineLayout;
    std::vector<VkCommandBuffer> mCommandBuffers;
};