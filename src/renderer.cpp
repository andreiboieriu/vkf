#include <renderer.hpp>

#include <array>

#include "core/coordinator.hpp"


extern Coordinator gCoordinator;


Renderer::Renderer(std::shared_ptr<Window> window, std::shared_ptr<Device> device) :
                   mWindow(window), mDevice(device) {
    RecreateSwapChain();
    CreateCommandBuffers();
}

Renderer::~Renderer() {
    FreeCommandBuffers();
}

void Renderer::CreateCommandBuffers() {
    mCommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mDevice->GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

    if (vkAllocateCommandBuffers(mDevice->GetDevice(), &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers");
    }
}

void Renderer::FreeCommandBuffers() {
    vkFreeCommandBuffers(
        mDevice->GetDevice(),
        mDevice->GetCommandPool(),
        static_cast<uint32_t>(mCommandBuffers.size()),
        mCommandBuffers.data());

    mCommandBuffers.clear();
}

void Renderer::RecreateSwapChain() {
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
        std::shared_ptr<SwapChain> oldSwapChain = std::move(mSwapChain);
        mSwapChain = std::make_unique<SwapChain>(mDevice, extent, oldSwapChain);

        if (!oldSwapChain->CompareSwapFormats(*mSwapChain.get())) {
            throw std::runtime_error("Swap chain image or depth format has changed");
        }
    }
}

VkCommandBuffer Renderer::BeginFrame() {
    gCoordinator.Assert(!mIsFrameStarted, "Can't call begin frame while already in progress");

    auto result = mSwapChain->AcquireNextImage(&mCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    mIsFrameStarted = true;
    auto commandBuffer = GetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer");
    }

    return commandBuffer;
}

void Renderer::EndFrame() {
    gCoordinator.Assert(mIsFrameStarted, "Can't call end frame while frame is not in progress");
    auto commandBuffer = GetCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer");
    }

    auto result = mSwapChain->SubmitCommandBuffers(&commandBuffer, &mCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindow->WasResized()) {
        mWindow->ResetResizedFlag();
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffers");
    }

    mIsFrameStarted = false;
    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    gCoordinator.Assert(mIsFrameStarted,
           "Can't call BeginSwapChainRenderPass while frame is not in progress");
    gCoordinator.Assert(commandBuffer == GetCurrentCommandBuffer(),
           "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = mSwapChain->GetFrameBuffer(mCurrentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    // clear color
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};

    // max depth
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(mSwapChain->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(mSwapChain->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, mSwapChain->GetSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    gCoordinator.Assert(mIsFrameStarted,
           "Can't call EndSwapChainRenderPass while frame is not in progress");
    gCoordinator.Assert(commandBuffer == GetCurrentCommandBuffer(),
           "Can't end render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
}
