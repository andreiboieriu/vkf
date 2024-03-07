#pragma once

#include <window.hpp>
#include <pipeline.hpp>
#include <device.hpp>
#include <swap_chain.hpp>
#include <model.hpp>

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

    void Run();

private:
    void CreatePipelineLayout();
    void CreatePipeline();
    void CreateCommandBuffers();
    void LoadModels();
    void DrawFrame();

    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<Pipeline> mPipeline;
    std::shared_ptr<SwapChain> mSwapChain;

    VkPipelineLayout mPipelineLayout;
    std::vector<VkCommandBuffer> mCommandBuffers;

    std::unique_ptr<Model> mModel;
};