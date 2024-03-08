#pragma once

#include <window.hpp>
#include <device.hpp>
#include <model.hpp>
#include <renderer.hpp>

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
    void LoadModels();

    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<Renderer> mRenderer;

    std::shared_ptr<Model> mModel;
};