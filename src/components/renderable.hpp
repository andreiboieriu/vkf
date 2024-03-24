#pragma once

#include <memory>
#include <texture.hpp>
#include <model.hpp>

struct Renderable {
    Model *model;
    Texture *texture;
    glm::vec3 color;
    float opacity;
};