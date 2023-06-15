#pragma once

#include <glm/glm.hpp>

#include "linalg/plane.hpp"

namespace dubu::block {

struct AABB {
  glm::vec3 min;
  glm::vec3 max;

  bool IsOutside(const Plane& plane) const {
    float minD, maxD;
    if (plane.n.x > 0.0f) {
      minD = plane.n.x * min.x;
      maxD = plane.n.x * max.x;
    } else {
      minD = plane.n.x * max.x;
      maxD = plane.n.x * min.x;
    }
    if (plane.n.y > 0.0f) {
      minD += plane.n.y * min.y;
      maxD += plane.n.y * max.y;
    } else {
      minD += plane.n.y * max.y;
      maxD += plane.n.y * min.y;
    }
    if (plane.n.z > 0.0f) {
      minD += plane.n.z * min.z;
      maxD += plane.n.z * max.z;
    } else {
      minD += plane.n.z * max.z;
      maxD += plane.n.z * min.z;
    }

    return minD >= plane.d;
  }
};

}  // namespace dubu::block