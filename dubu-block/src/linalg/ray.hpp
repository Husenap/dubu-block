#pragma once

#include <cmath>
#include <functional>
#include <memory>

#include <glm/glm.hpp>

#include "game/block.hpp"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"

namespace {
inline int minAxis(glm::vec3 v) {
  if (v.x < v.y) {
    if (v.x < v.z) return 0;
  } else if (v.y < v.z) {
    return 1;
  }
  return 2;
}
}  // namespace

namespace dubu::block {

struct Ray {
  glm::vec3 start;
  glm::vec3 direction;
  float     length;
};

struct RaycastHit {
  glm::ivec3 coords;
  glm::ivec3 face;
  float      distance;
};

/*
 * Casts a ray using the DDA algorithm.
 * `callback` is called with the hit info at every block boundary.
 */
inline void Raycast(Ray ray, std::function<bool(RaycastHit)> callback) {
  glm::vec3 stepSize = {
      glm::length(
          glm::vec3(1, ray.direction.y / ray.direction.x, ray.direction.z / ray.direction.x)),
      glm::length(
          glm::vec3(1, ray.direction.x / ray.direction.y, ray.direction.z / ray.direction.y)),
      glm::length(
          glm::vec3(1, ray.direction.x / ray.direction.z, ray.direction.y / ray.direction.z)),
  };

  glm::ivec3 blockCoords = glm::floor(ray.start);

  glm::ivec3 step;
  glm::vec3  axisLengths;
  for (int axis = 0; axis < 3; axis++) {
    if (ray.direction[axis] < 0) {
      step[axis]        = -1;
      axisLengths[axis] = (ray.start[axis] - float(blockCoords[axis])) * stepSize[axis];
    } else {
      axisLengths[axis] = (float(blockCoords[axis] + 1) - ray.start[axis]) * stepSize[axis];
      step[axis]        = 1;
    }
  }

  float distance = 0.0f;
  for (;;) {
    int axis = minAxis(axisLengths);
    distance = axisLengths[axis];

    if (distance > ray.length) break;

    blockCoords[axis] += step[axis];
    axisLengths[axis] += stepSize[axis];

    auto face  = glm::vec3(0.0f);
    face[axis] = -step[axis];

    if (callback({
            .coords   = blockCoords,
            .face     = face,
            .distance = distance,
        })) {
      break;
    }
  }
}
}  // namespace dubu::block
