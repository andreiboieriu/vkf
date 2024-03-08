#pragma once

#include <memory>
#include <device.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

class Model {
public:
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    Model(std::shared_ptr<Device>& device, const std::vector<Vertex>& vertices);
    ~Model();

    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);


private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);

    std::shared_ptr<Device> mDevice;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    uint32_t mVertexCount;
};
