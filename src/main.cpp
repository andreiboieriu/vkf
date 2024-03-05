#include <app.hpp>

// std
#include <iostream>
#include <cstdlib>

int main() {
  App app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}