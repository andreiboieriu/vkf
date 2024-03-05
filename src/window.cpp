#include <window.hpp>

// std
#include <stdexcept>

Window::Window(const uint32_t width, const uint32_t height, std::string name) :
               mWidth(width),
               mHeight(height),
               mName(name) {
    init();
}

Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Window::init() {
    glfwInit();

    // set window hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(mWidth, mHeight, mName.c_str(), nullptr, nullptr);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, mWindow, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface");
    }
}
