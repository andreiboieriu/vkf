#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window {

public:
    Window(const uint32_t width, const uint32_t height, std::string name);
    ~Window();

    bool shouldClose() {
        return glfwWindowShouldClose(mWindow);
    }

    void pollEvents() {
        glfwPollEvents();
    }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    VkExtent2D getExtent() {
        return {mWidth, mHeight};
    }

private:
    void init();

    GLFWwindow *mWindow;
    uint32_t mWidth;
    uint32_t mHeight;
    std::string mName;
};