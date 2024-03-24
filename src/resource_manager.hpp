#pragma once

#include <texture.hpp>
#include <model.hpp>
#include <unordered_map>
#include <device.hpp>

class ResourceManager {

public:
    void Init(std::shared_ptr<Device> device) {
        mDevice = device;
    }

    void LoadResources();

    // Uses default image view and samplers
    void LoadTexture(const std::string& name, const std::string& filePath);
    Texture *GetTexture(const std::string& name);

    void LoadModel(const std::string& name, const Model::Builder& builder);
    Model *GetModel(const std::string& name);

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextureMap;
    std::vector<std::string> mTextureNames;

    std::unordered_map<std::string, std::unique_ptr<Model>> mModelMap;
    std::vector<std::string> mModelNames;

    std::shared_ptr<Device> mDevice;
};
