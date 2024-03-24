#pragma once

#include <core/system.hpp>
#include <components/renderable.hpp>
#include <core/coordinator.hpp>

#include <random>

extern Coordinator gCoordinator;

class LsdSystem : public System {

public:
    void Update(float dt) {
        std::random_device rdev{};
        std::default_random_engine generator{rdev()};
        std::uniform_real_distribution<float> dist{0.0f, 1.0f};

        int i = 0;

        for (auto& entity : mEntities) {
            auto& renderable = gCoordinator.GetComponent<Renderable>(entity);

            spentTimes[i] += dt;

            if (spentTimes[i] >= transitionTime) {
                spentTimes[i] = 0.f;

                originalColors[i] = targetColors[i];
                targetColors[i] = glm::vec3(dist(generator), dist(generator), dist(generator));
            } else {
                float t = spentTimes[i] / transitionTime;

                renderable.color = originalColors[i] * (1.0f - t) + targetColors[i] * t;
            }


            i++;
        }
    }

private:
    const float transitionTime = .5f;
    std::vector<float> spentTimes{0.0f, 0.0f};
    std::vector<glm::vec3> originalColors{glm::vec3(1.0f), glm::vec3(1.0f)};
    std::vector<glm::vec3> targetColors{glm::vec3(0.f), glm::vec3(0.f)};
};