#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window {

public:
    Window(const uint32_t width, const uint32_t height, std::string name);
    ~Window();

    bool ShouldClose() {
        return glfwWindowShouldClose(mWindow);
    }

    bool WasResized() {
        return mResized;
    }

    void ResetResizedFlag() {
        mResized = false;
    }

    void PollEvents() {
        glfwPollEvents();
    }

    void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    VkExtent2D GetExtent() {
        return {mWidth, mHeight};
    }

private:
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    void Init();

    GLFWwindow *mWindow;
    uint32_t mWidth;
    uint32_t mHeight;
    bool mResized = false;
    std::string mName;
};
