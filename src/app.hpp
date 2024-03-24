#pragma once

#include <device.hpp>
#include <model.hpp>
#include <renderer.hpp>
#include "core/window/window.hpp"
#include <texture.hpp>
#include <glm/glm.hpp>

#include <descriptors.hpp>

// std
#include <memory>
#include <vector>

class App {

public:
    static constexpr uint32_t WIDTH = 1280;
    static constexpr uint32_t HEIGHT = 720;
    static constexpr std::string NAME = "Vulkan";

    App();
    ~App();

    void Run();

private:

    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<Renderer> mRenderer;
    std::shared_ptr<Model> mModel;
    std::shared_ptr<Texture> mTex;

    glm::vec3 color = {0.0f, 0.0f, 0.0f};
    int sign = 1;
};
