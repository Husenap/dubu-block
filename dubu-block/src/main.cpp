#include <iostream>

#include <dubu_window/dubu_window.h>

int main() {
  dubu::window::GLFWWindow window(400, 400, "dubu block");
  auto token = window.RegisterListener<dubu::window::EventResize>([](const auto& e) {
    std::cout << "Window "
              << " resized: " << e.width << ", " << e.height << ")" << std::endl;
  });

  while (!window.ShouldClose()) {
    window.PollEvents();
  }

  return 0;
}