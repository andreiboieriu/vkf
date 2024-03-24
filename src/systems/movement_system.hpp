#pragma once

#include <core/system.hpp>
#include <components/transform.hpp>
#include <core/coordinator.hpp>

extern Coordinator gCoordinator;

class MovementSystem : public System {

public:
    void Update(float dt) {
        int i = 0;
        for (auto& entity : mEntities) {
            auto& transform = gCoordinator.GetComponent<Transform>(entity);

            switch (currentStates[i]) {
            case State::UP:
                transform.TranslateOY(dt * velocity);

                if (transform.position.y >= 720.f - transform.scale.y / 2.f) {
                    transform.position.y = 720.f - transform.scale.y / 2.f;
                    currentStates[i] = State::LEFT;
                }

                break;

            case State::DOWN:
                transform.TranslateOY(-dt * velocity);

                if (transform.position.y <= transform.scale.y / 2.f) {
                    transform.position.y = transform.scale.y / 2.f;
                    currentStates[i] = State::RIGHT;
                }

                break;

            case State::LEFT:
                transform.TranslateOX(-dt * velocity);

                if (transform.position.x <= transform.scale.x / 2.f) {
                    transform.position.x = transform.scale.x / 2.f;
                    currentStates[i] = State::DOWN;
                }
                break;

            case State::RIGHT:
                transform.TranslateOX(dt * velocity);

                if (transform.position.x >= 1280 - transform.scale.x / 2.f) {
                    transform.position.x = 1280 - transform.scale.x / 2.f;
                    currentStates[i] = State::UP;
                }
                break;
            }

            i++;
        }
    }

private:
    enum class State {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    std::vector<State> currentStates{State::UP, State::DOWN};

    const float velocity = 500.f;
};