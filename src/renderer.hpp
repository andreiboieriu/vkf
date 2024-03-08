#pragma once

#include <window.hpp>
#include <device.hpp>
#include <swap_chain.hpp>
#include <model.hpp>

// std
#include <memory>
#include <vector>
#include <cassert>

class Renderer {

public:
    Renderer(std::shared_ptr<Window> window, std::shared_ptr<Device> device);
    ~Renderer();

    bool IsFrameInProgress() const {
        return mIsFrameStarted;
    }

    VkCommandBuffer GetCurrentCommandBuffer() const {
        assert(mIsFrameStarted && "Can't get command buffer if frame is not in progress");
        return mCommandBuffers[mCurrentFrameIndex];
    }

    VkRenderPass GetSwapChainRenderPass() const {
        return mSwapChain->GetRenderPass();
    }

    int GetFrameIndex() const {
        assert(mIsFrameStarted && "Can't get frame index if frame is not in progress");
        return mCurrentFrameIndex;
    }

    VkCommandBuffer BeginFrame();
    void EndFrame();
    void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void CreateCommandBuffers();
    void FreeCommandBuffers();
    void RecreateSwapChain();

    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Device> mDevice;
    std::unique_ptr<SwapChain> mSwapChain;
    std::vector<VkCommandBuffer> mCommandBuffers;

    uint32_t mCurrentImageIndex;
    int mCurrentFrameIndex;
    bool mIsFrameStarted = false;
};
