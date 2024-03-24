#include <simple_render_system.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "core/coordinator.hpp"
#include <swap_chain.hpp>

#include <components/transform.hpp>
#include <components/renderable.hpp>


extern Coordinator gCoordinator;

SimpleRenderSystem::SimpleRenderSystem() {

}

SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(mDevice->GetDevice(), mPipelineLayout, nullptr);
}

void SimpleRenderSystem::Init(std::shared_ptr<Device> device, VkRenderPass renderPass) {
    mDevice = device;

    CreateDescriptorSetLayouts();
    CreatePipelineLayout();
    CreatePipeline(renderPass);

    CreateUniformBuffers();
}

void SimpleRenderSystem::CreateDescriptorSetLayouts() {
    mDescriptorSetLayout = DescriptorSetLayout::Builder(mDevice)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build();
}

void SimpleRenderSystem::CreateUniformBuffers() {
    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto buffer = std::make_unique<Buffer>(
            mDevice,
            sizeof(SimpleRenderSystem::Ubo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        mUboBuffers.push_back(std::move(buffer));
        mUboBuffers[i]->Map();
    }
} 

void SimpleRenderSystem::CreatePipelineLayout() {
    VkPushConstantRange vertexPushConstantRange{};
    vertexPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vertexPushConstantRange.offset = 0;
    vertexPushConstantRange.size = sizeof(VertexPushData);

    VkPushConstantRange fragmentPushConstantRange{};
    fragmentPushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentPushConstantRange.offset = sizeof(VertexPushData);
    fragmentPushConstantRange.size = sizeof(FragmentPushData);

    std::vector<VkPushConstantRange> pushConstantRanges{vertexPushConstantRange, fragmentPushConstantRange};

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{mDescriptorSetLayout->GetDescriptorSetLayout()};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

    if (vkCreatePipelineLayout(mDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout");
    }
}

void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
    gCoordinator.Assert(mPipelineLayout != nullptr, "Cannot create pipeline before pipeline layout");

    Pipeline::ConfigInfo pipelineConfig{};
    Pipeline::DefaultConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = mPipelineLayout;

    mPipeline = std::make_unique<Pipeline>(mDevice,
                                           "../shaders/simple_shader.vert.spv",
                                           "../shaders/simple_shader.frag.spv",
                                           pipelineConfig);
}

void SimpleRenderSystem::Render(VkCommandBuffer commandBuffer, int frameIndex) {
    mPipeline->Bind(commandBuffer);

    for (auto& entity : mEntities) {
        auto& transform = gCoordinator.GetComponent<Transform>(entity);
        auto& renderable = gCoordinator.GetComponent<Renderable>(entity);

        auto bufferInfo = mUboBuffers[frameIndex]->DescriptorInfo();
        auto imageInfo = renderable.texture->DescriptorInfo();

        Ubo ubo{};
        ubo.projection = glm::ortho(0.0f, 1280.f, 720.0f, 0.0f, 0.0f, 1.0f);
        ubo.view = glm::mat4(1.f);

        mUboBuffers[frameIndex]->WriteToBuffer(&ubo);

        auto descriptorWrites = DescriptorWriter(*mDescriptorSetLayout)
            .WriteBuffer(0, &bufferInfo)
            .WriteImage(1, &imageInfo)
            .GetWrites();

        // attach ubo descriptor writes to command buffer
        mDevice->vkCmdPushDescriptorSetKHR(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            0,
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data()
        );

        VertexPushData vertexPush{};
        vertexPush.model = transform.GetModelMatrix();

        FragmentPushData fragmentPush{};
        fragmentPush.color = renderable.color;
        fragmentPush.opacity = renderable.opacity;

        vkCmdPushConstants(
            commandBuffer,
            mPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(VertexPushData),
            &vertexPush);

        vkCmdPushConstants(
            commandBuffer,
            mPipelineLayout,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            sizeof(VertexPushData),
            sizeof(FragmentPushData),
            &fragmentPush);

        renderable.model->Bind(commandBuffer);
        renderable.model->Draw(commandBuffer);
    }
}
