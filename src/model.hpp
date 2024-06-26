#pragma once

#include <memory>
#include <device.hpp>
#include <buffer.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

class Model {
public:
    struct Vertex {
        alignas(8) glm::vec2 position;
        alignas(16) glm::vec3 color;
        alignas(8) glm::vec2 texCoord;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
    };

    Model(std::shared_ptr<Device> device, const Builder& builder);
    ~Model();

    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);


private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);

    std::shared_ptr<Device> mDevice;

    std::unique_ptr<Buffer> mVertexBuffer;
    uint32_t mVertexCount;

    std::unique_ptr<Buffer> mIndexBuffer;
    uint32_t mIndexCount;
};
