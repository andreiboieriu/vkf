#include <app.hpp>
#include <memory>
#include <stdexcept>

#include <simple_render_system.hpp>

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

    while (!mWindow->ShouldClose()) {
        mWindow->PollEvents();
        
        if (auto commandBuffer = mRenderer->BeginFrame()) {
            mRenderer->BeginSwapChainRenderPass(commandBuffer);

            // render game objects
            simple_render_system.RenderGameObjects(commandBuffer, mModel);

            mRenderer->EndSwapChainRenderPass(commandBuffer);
            mRenderer->EndFrame();
        }
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
}
