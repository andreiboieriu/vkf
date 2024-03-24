#include <app.hpp>

#include "core/coordinator.hpp"

// std
#include <cstdlib>

extern Coordinator gCoordinator;

int main() {
    App app;

    try {
        app.Run();
    } catch (const std::exception& e) {
        gCoordinator.LogError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
