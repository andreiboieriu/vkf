#include <window.hpp>

// std
#include <stdexcept>

Window::Window(const uint32_t width, const uint32_t height, std::string name) :
               mWidth(width),
               mHeight(height),
               mName(name) {
    Init();
}

Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Window::Init() {
    glfwInit();

    // set window hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mWindow = glfwCreateWindow(mWidth, mHeight, mName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(mWindow, this);

    // set callbacks
    glfwSetFramebufferSizeCallback(mWindow, ResizeCallback);
}

void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, mWindow, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface");
    }
}

void Window::ResizeCallback(GLFWwindow *window, int width, int height) {
    auto engineWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    engineWindow->mResized = true;
    engineWindow->mWidth = width;
    engineWindow->mHeight = height;

}

