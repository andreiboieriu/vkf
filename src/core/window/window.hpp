#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <bitset>
#include <string>


using KeysBitset = std::bitset<GLFW_KEY_LAST + 1>;


class Window {
public:
    Window(const uint32_t width, const uint32_t height, std::string name);
    ~Window();

    void Update(float dt);
        
    bool ShouldClose() {
        return glfwWindowShouldClose(mWindow);
    }

    bool WasResized() {
        return mResized;
    }

    void ResetResizedFlag() {
        mResized = false;
    }

    void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    VkExtent2D GetExtent() {
        return {mWidth, mHeight};
    }

private:
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void KeyCallback(GLFWwindow *window,
                            int key, int scancode, int action, int mods);
    void Init();

    GLFWwindow *mWindow;
    uint32_t mWidth;
    uint32_t mHeight;
    bool mResized = false;
    std::string mName;
};
