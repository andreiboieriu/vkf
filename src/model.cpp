#include <model.hpp>
#include <cstring>

#include "core/coordinator.hpp"

extern Coordinator gCoordinator;

Model::Model(std::shared_ptr<Device> device, const Builder& builder) :
             mDevice(device) {
    CreateVertexBuffer(builder.vertices);
    CreateIndexBuffer(builder.indices);

    // device pointer no longer needed
    mDevice = nullptr;
}

Model::~Model() {

}

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Model::Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Model::Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Model::Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Model::Vertex, texCoord);

    return attributeDescriptions;
}

void Model::Bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {mVertexBuffer->GetBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    // index size can be optimized
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Model::Draw(VkCommandBuffer commandBuffer) {
    vkCmdDrawIndexed(commandBuffer, mIndexCount, 1, 0, 0, 0);
}

void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    mVertexCount = static_cast<uint32_t>(vertices.size());
    gCoordinator.Assert(mVertexCount >= 3, "Vertex count must be at least 3");

    VkDeviceSize bufferSize = sizeof(vertices[0]) * mVertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    // create staging buffer
    Buffer stagingBuffer(
        mDevice,
        vertexSize,
        mVertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    // copy data to staging buffer
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void *)vertices.data());

    // create vertex buffer
    mVertexBuffer = std::make_unique<Buffer>(
        mDevice,
        vertexSize,
        mVertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    // copy data from staging to vertex buffer
    mDevice->CopyBuffer(stagingBuffer.GetBuffer(), mVertexBuffer->GetBuffer(), bufferSize);
}

void Model::CreateIndexBuffer(const std::vector<uint32_t>& indices) {
    mIndexCount = static_cast<uint32_t>(indices.size());

    VkDeviceSize bufferSize = sizeof(indices[0]) * mIndexCount;
    uint32_t indexSize = sizeof(indices[0]);

    // create staging buffer
    Buffer stagingBuffer(
        mDevice,
        indexSize,
        mIndexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    // copy data to staging buffer
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void *)indices.data());

    // create index buffer
    mIndexBuffer = std::make_unique<Buffer>(
        mDevice,
        indexSize,
        mIndexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    // copy data from staging to index buffer
    mDevice->CopyBuffer(stagingBuffer.GetBuffer(), mIndexBuffer->GetBuffer(), bufferSize);
}
