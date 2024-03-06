#include "pipeline.hpp"
#include <app.hpp>
#include <memory>
#include <stdexcept>
#include <array>

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
        drawFrame();
    }

    vkDeviceWaitIdle(mDevice->device());
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
    mCommandBuffers.resize(mSwapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mDevice->getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

    if (vkAllocateCommandBuffers(mDevice->device(), &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers");
    }

    for (int i = 0; i < mCommandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = mSwapChain->getRenderPass();
        renderPassInfo.framebuffer = mSwapChain->getFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = mSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        // clear color
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};

        // max depth
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        mPipeline->bind(mCommandBuffers[i]);
        
        // draw 3 vertices in 1 instance
        // multiple instances can be used for instanced drawing
        vkCmdDraw(mCommandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(mCommandBuffers[i]);

        if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }
}

void App::drawFrame() {
    uint32_t imageIndex;
    auto result = mSwapChain->acquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    result = mSwapChain->submitCommandBuffers(&mCommandBuffers[imageIndex], &imageIndex);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffers");
    }
}