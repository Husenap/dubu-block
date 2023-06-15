#pragma once

#include <glm/glm.hpp>

namespace dubu::block {
struct Plane {
  Plane() = default;
  Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    n = glm::normalize(glm::cross(b - a, c - b));
    d = glm::dot(n, a);
  }
  glm::vec3 n;
  float     d;
};
};  // namespace dubu::block