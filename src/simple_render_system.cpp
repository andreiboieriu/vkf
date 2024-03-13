#include <simple_render_system.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "core/coordinator.hpp"


extern Coordinator gCoordinator;


struct PushConstantData {
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(std::shared_ptr<Device> device, VkRenderPass renderPass) :
                                       mDevice(device) {
    CreatePipelineLayout();
    CreatePipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(mDevice->GetDevice(), mPipelineLayout, nullptr);
}

void SimpleRenderSystem::CreatePipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);


    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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

void SimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::shared_ptr<Model> model) {
    mPipeline->Bind(commandBuffer);
    model->Bind(commandBuffer);

    for (int j = 0; j < 4; j++) {
        PushConstantData push{};
        push.offset = glm::vec2(0.0f, -0.4f + 0.25f * j);
        push.color = glm::vec3(0.0f, 0.25f, 0.2f + 0.2f * j);

        vkCmdPushConstants(
            commandBuffer,
            mPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PushConstantData),
            &push);

        model->Draw(commandBuffer);
    }
}

