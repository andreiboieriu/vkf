#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform {
    glm::vec3 position;
    float rotation;
    glm::vec2 scale;

    void TranslateOX(float distance) {
        position.x += distance;
        mUpdateModel = true;
    }

    void TranslateOY(float distance) {
        position.y += distance;
        mUpdateModel = true;
    }

    void SetScale(glm::vec2 scale) {
        this->scale = scale;
        mUpdateModel = true;
    }

    void SetDepth(float depth) {
        position.z = depth;
        mUpdateModel = true;
    }

    void Rotate(float rotation) {
        this->rotation += rotation;
        mUpdateModel = true;
    }

    glm::mat4 GetModelMatrix() {
        if (mUpdateModel) {
            UpdateModelMatrix();
            mUpdateModel = false;
        }

        return mModelMatrix;
    }

private:
    bool mUpdateModel = true;
    glm::mat4 mModelMatrix;

    void UpdateModelMatrix() {
        mModelMatrix = glm::mat4(1.f);
        mModelMatrix = glm::translate(mModelMatrix, position);
        mModelMatrix = glm::rotate(mModelMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        mModelMatrix = glm::scale(mModelMatrix, glm::vec3(scale, 0.f));
    }
};