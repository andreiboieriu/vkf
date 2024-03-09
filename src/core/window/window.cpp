#include <core/window/window.hpp>
#include "core/coordinator.hpp"

// std
#include <stdexcept>


extern Coordinator gCoordinator;


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
    glfwSetKeyCallback(mWindow, KeyCallback);
}

void Window::Update([[maybe_unused]] float dt) {
    KeysBitset keys;

    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        keys[key] = glfwGetKey(mWindow, key);
    }

    gCoordinator.SendEvent(Event(Events::Input::Sync::Key::ID)
                .SetParam(Events::Input::Sync::Key::KEYS, keys));

    glfwPollEvents();
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

void Window::KeyCallback([[maybe_unused]] GLFWwindow* window,
                        int key, int scancode, int action, int mods)
{
    gCoordinator.LogDebug("Key pressed: ", static_cast<char>(key));
    gCoordinator.SendEvent(Event(Events::Input::Async::Key::ID)
                .SetParam(Events::Input::Async::Key::KEY, key)
                .SetParam(Events::Input::Async::Key::SCANCODE, scancode)
                .SetParam(Events::Input::Async::Key::ACTION, action)
                .SetParam(Events::Input::Async::Key::MODS, mods));
}
