#include <resource_manager.hpp>

#include <algorithm>
#include <stdexcept>

void ResourceManager::LoadResources() {
    LoadTexture("chicken", "../assets/textures/chicken.jpg");

    // square model
    std::vector<Model::Vertex> vertices {
        {glm::vec2(-0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},   // -0.5 -0.5                0.5 -0.5
        {glm::vec2(0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},    
        {glm::vec2(0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec2(-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)}    // -0.5 0.5                 0.5 0.5
    };

    std::vector<uint32_t> indices {0, 1, 2, 2, 3, 0};

    Model::Builder builder{vertices, indices};

    LoadModel("square", builder);
}

void ResourceManager::LoadTexture(const std::string& name, const std::string& filePath) {
    // check if a texture with the given name has already been loaded
    if (std::find(mTextureNames.begin(), mTextureNames.end(), name) != mTextureNames.end()) {
        throw std::runtime_error("Texture named " + name + " has already been loaded");
    }

    mTextureNames.push_back(name);
    mTextureMap[name] = std::make_unique<Texture>(mDevice, filePath);
}

Texture *ResourceManager::GetTexture(const std::string& name) {
    if (std::find(mTextureNames.begin(), mTextureNames.end(), name) == mTextureNames.end()) {
        throw std::runtime_error("No texture named " + name + " is loaded");
    }

    return mTextureMap[name].get();
}

void ResourceManager::LoadModel(const std::string& name, const Model::Builder& builder) {
    // check if a model with the given name has already been loaded
    if (std::find(mModelNames.begin(), mModelNames.end(), name) != mModelNames.end()) {
        throw std::runtime_error("Texture named " + name + " has already been loaded");
    }

    mModelNames.push_back(name);
    mModelMap[name] = std::make_unique<Model>(mDevice, builder);
}

Model *ResourceManager::GetModel(const std::string& name) {
    if (std::find(mModelNames.begin(), mModelNames.end(), name) == mModelNames.end()) {
        throw std::runtime_error("No texture named " + name + " is loaded");
    }

    return mModelMap[name].get();
}
