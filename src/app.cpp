#include "pipeline.hpp"
#include <app.hpp>
#include <memory>
#include <stdexcept>

App::App() {
    mWindow = std::make_shared<Window>(WIDTH, HEIGHT, NAME);
    mDevice = std::make_shared<Device>(mWindow);
    mSwapChain = std::make_shared<SwapChain>(mDevice, mWindow->getExtent());

    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

App::~App() {
    vkDestroyPipelineLayout(mDevice->device(), mPipelineLayout, nullptr);
}

void App::run() {
    while (!mWindow->shouldClose()) {
        mWindow->pollEvents();
    }
}

void App::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(mDevice->device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout");
    }
}

void App::createPipeline() {
    auto pipelineConfig = Pipeline::defaultConfigInfo(mSwapChain->width(), mSwapChain->height());
    pipelineConfig.renderPass = mSwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = mPipelineLayout;

    mPipeline = std::make_shared<Pipeline>(mDevice,
                                           "../shaders/simple_shader.vert.spv",
                                           "../shaders/simple_shader.frag.spv",
                                           pipelineConfig);
}

void App::createCommandBuffers() {

}

void App::drawFrame() {

}