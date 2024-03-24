#include <chrono>
#include <memory>
#include <stdexcept>

#include "app.hpp"
#include "simple_render_system.hpp"
#include "core/coordinator.hpp"
#include <resource_manager.hpp>

#include <buffer.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>

#include <systems/movement_system.hpp>
#include <systems/lsd_system.hpp>

Coordinator gCoordinator(LogLevel::DEBUG);
ResourceManager gResourceManager;

App::App() {
    mWindow = std::make_shared<Window>(WIDTH, HEIGHT, NAME);
    mDevice = std::make_shared<Device>(mWindow);
    mRenderer = std::make_shared<Renderer>(mWindow, mDevice);

    gResourceManager.Init(mDevice);
}

App::~App() {

}

void App::Run() {
    // load resources
    gResourceManager.LoadResources();

    // init systems
    gCoordinator.RegisterComponent<Transform>();
    gCoordinator.RegisterComponent<Renderable>();

    auto renderSystem = gCoordinator.RegisterSystem<SimpleRenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Transform>());
        signature.set(gCoordinator.GetComponentType<Renderable>());
        gCoordinator.SetSystemSignature<SimpleRenderSystem>(signature);
    }
    renderSystem->Init(mDevice, mRenderer->GetSwapChainRenderPass());

    auto movementSystem = gCoordinator.RegisterSystem<MovementSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<MovementSystem>(signature);
    }

    auto lsdSystem = gCoordinator.RegisterSystem<LsdSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Renderable>());
        gCoordinator.SetSystemSignature<LsdSystem>(signature);
    }

    Entity entity = gCoordinator.CreateEntity();

    Renderable renderable{gResourceManager.GetModel("square"),
                          gResourceManager.GetTexture("chicken"),
                          glm::vec3(1.0f),
                          1.0f};

    Transform transform{};
    transform.SetScale(glm::vec2(300.f));
    transform.TranslateOX(1280.f / 2.f);
    transform.TranslateOY(720.f / 2.f);

    gCoordinator.AddComponent(entity, transform);
    gCoordinator.AddComponent(entity, renderable);

    Entity entity2 = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(entity2, renderable);

    transform.TranslateOX(300.f);

    gCoordinator.AddComponent(entity2, transform);


    float dt = 0.0f;
    while (!mWindow->ShouldClose()) {
        auto startTime = std::chrono::high_resolution_clock::now();

        if (auto commandBuffer = mRenderer->BeginFrame()) {
            mRenderer->BeginSwapChainRenderPass(commandBuffer);

            // get frame index
            int frameIndex = mRenderer->GetFrameIndex();

            // render game objects
            lsdSystem->Update(dt);
            movementSystem->Update(dt);
            renderSystem->Render(commandBuffer, frameIndex);

            mRenderer->EndSwapChainRenderPass(commandBuffer);
            mRenderer->EndFrame();
        }

        mWindow->Update(dt);
        auto stopTime = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
        // gCoordinator.LogDebug("fps = ", 1.0f / dt);
    }

    vkDeviceWaitIdle(mDevice->GetDevice());
}
