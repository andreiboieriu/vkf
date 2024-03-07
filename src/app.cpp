#include "pipeline.hpp"
#include <app.hpp>
#include <memory>
#include <stdexcept>
#include <array>

App::App() {
    mWindow = std::make_shared<Window>(WIDTH, HEIGHT, NAME);
    mDevice = std::make_shared<Device>(mWindow);
    mSwapChain = std::make_shared<SwapChain>(mDevice, mWindow->GetExtent());

    CreatePipelineLayout();
    CreatePipeline();
    CreateCommandBuffers();
}

App::~App() {
    vkDestroyPipelineLayout(mDevice->GetDevice(), mPipelineLayout, nullptr);
}

void App::Run() {
    while (!mWindow->ShouldClose()) {
        mWindow->PollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(mDevice->GetDevice());
}

void App::CreatePipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(mDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout");
    }
}

void App::CreatePipeline() {
    auto pipelineConfig = Pipeline::DefaultConfigInfo(mSwapChain->GetWidth(),
                                                      mSwapChain->GetHeight());
    pipelineConfig.renderPass = mSwapChain->GetRenderPass();
    pipelineConfig.pipelineLayout = mPipelineLayout;

    mPipeline = std::make_shared<Pipeline>(mDevice,
                                           "../shaders/simple_shader.vert.spv",
                                           "../shaders/simple_shader.frag.spv",
                                           pipelineConfig);
}

void App::CreateCommandBuffers() {
    mCommandBuffers.resize(mSwapChain->GetImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mDevice->GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

    if (vkAllocateCommandBuffers(mDevice->GetDevice(), &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
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
        renderPassInfo.renderPass = mSwapChain->GetRenderPass();
        renderPassInfo.framebuffer = mSwapChain->GetFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = mSwapChain->GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        // clear color
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};

        // max depth
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        mPipeline->Bind(mCommandBuffers[i]);
        
        // draw 3 vertices in 1 instance
        // multiple instances can be used for instanced drawing
        vkCmdDraw(mCommandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(mCommandBuffers[i]);

        if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }
}

void App::DrawFrame() {
    uint32_t imageIndex;
    auto result = mSwapChain->AcquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    result = mSwapChain->SubmitCommandBuffers(&mCommandBuffers[imageIndex], &imageIndex);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffers");
    }
}
