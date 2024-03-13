#include <chrono>
#include <memory>
#include <stdexcept>

#include "app.hpp"
#include "simple_render_system.hpp"
#include "core/coordinator.hpp"


Coordinator gCoordinator(LogLevel::NORMAL);


App::App() {
    mWindow = std::make_shared<Window>(WIDTH, HEIGHT, NAME);
    mDevice = std::make_shared<Device>(mWindow);
    mRenderer = std::make_shared<Renderer>(mWindow, mDevice);

    LoadModels();
}

App::~App() {

}

void App::Run() {
    SimpleRenderSystem simple_render_system(mDevice, mRenderer->GetSwapChainRenderPass());

    float dt = 0.0f;
    while (!mWindow->ShouldClose()) {
        auto startTime = std::chrono::high_resolution_clock::now();

        if (auto commandBuffer = mRenderer->BeginFrame()) {
            mRenderer->BeginSwapChainRenderPass(commandBuffer);

            // render game objects
            simple_render_system.RenderGameObjects(commandBuffer, mModel);

            mRenderer->EndSwapChainRenderPass(commandBuffer);
            mRenderer->EndFrame();
        }

        mWindow->Update(dt);
        auto stopTime = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
        gCoordinator.LogDebug("fps = ", 1.0f / dt);
    }

    vkDeviceWaitIdle(mDevice->GetDevice());
}

void App::LoadModels() {
    std::vector<Model::Vertex> vertices {
        {glm::vec2(0.0f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec2(0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)},
        {glm::vec2(-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}
    };

    std::vector<uint32_t> indices {0, 1, 2};

    Model::Builder builder {vertices, indices};

    mModel = std::make_shared<Model>(mDevice, builder);

    auto tex = std::make_shared<Texture>(mDevice, "../assets/textures/chicken.jpg");
}
