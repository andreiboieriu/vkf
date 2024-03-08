#include "pipeline.hpp"
#include <app.hpp>
#include <memory>
#include <stdexcept>
#include <array>

App::App() {
    mWindow = std::make_shared<Window>(WIDTH, HEIGHT, NAME);
    mDevice = std::make_shared<Device>(mWindow);

    LoadModels();

    CreatePipelineLayout();
    RecreateSwapChain();
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
    assert(mSwapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(mPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    Pipeline::ConfigInfo pipelineConfig{};
    Pipeline::DefaultConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = mSwapChain->GetRenderPass();
    pipelineConfig.pipelineLayout = mPipelineLayout;

    mPipeline = std::make_unique<Pipeline>(mDevice,
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
}

void App::FreeCommandBuffers() {
    vkFreeCommandBuffers(
        mDevice->GetDevice(),
        mDevice->GetCommandPool(),
        static_cast<uint32_t>(mCommandBuffers.size()),
        mCommandBuffers.data());

    mCommandBuffers.clear();
}


void App::LoadModels() {
    std::vector<Model::Vertex> vertices {
        {glm::vec2(0.0f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec2(0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)},
        {glm::vec2(-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}
    };

    mModel = std::make_unique<Model>(mDevice, vertices);
}

void App::DrawFrame() {
    uint32_t imageIndex;
    auto result = mSwapChain->AcquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    RecordCommandBuffer(imageIndex);
    result = mSwapChain->SubmitCommandBuffers(&mCommandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindow->WasResized()) {
        mWindow->ResetResizedFlag();
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffers");
    }
}

void App::RecreateSwapChain() {
    auto extent = mWindow->GetExtent();

    // can occur during minimization
    while (extent.width == 0 || extent.height == 0) {
        extent = mWindow->GetExtent();
        glfwWaitEvents();
    }

    // wait until swap chain is no longer being used
    vkDeviceWaitIdle(mDevice->GetDevice());
    
    if (mSwapChain == nullptr) {
        mSwapChain = std::make_unique<SwapChain>(mDevice, extent);
    } else {
        mSwapChain = std::make_unique<SwapChain>(mDevice, extent, std::move(mSwapChain));

        if (mSwapChain->GetImageCount() != mCommandBuffers.size()) {
            FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }

    
    // TODO do nothing if render passes are compatible
    CreatePipeline();
}

void App::RecordCommandBuffer(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(mCommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = mSwapChain->GetFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    // clear color
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};

    // max depth
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(mCommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(mSwapChain->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(mSwapChain->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, mSwapChain->GetSwapChainExtent()};
    vkCmdSetViewport(mCommandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(mCommandBuffers[imageIndex], 0, 1, &scissor);

    mPipeline->Bind(mCommandBuffers[imageIndex]);
    mModel->Bind(mCommandBuffers[imageIndex]);
    mModel->Draw(mCommandBuffers[imageIndex]);

    vkCmdEndRenderPass(mCommandBuffers[imageIndex]);

    if (vkEndCommandBuffer(mCommandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer");
    }
}
