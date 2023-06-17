#pragma once

#include <glm/glm.hpp>

namespace dubu::block::noise {

float random(glm::vec2 st) {
  return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
}
float noise(glm::vec2 st) {
  const glm::vec2 i = floor(st);
  const glm::vec2 f = fract(st);

  // Four corners in 2D of a tile
  const float a = random(i);
  const float b = random(i + glm::vec2(1.0f, 0.0f));
  const float c = random(i + glm::vec2(0.0f, 1.0f));
  const float d = random(i + glm::vec2(1.0f, 1.0f));

  const glm::vec2 u = f * f * (3.0f - 2.0f * f);

  return glm::mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
}

static constexpr int OCTAVES = 6;

float fbm(glm::vec2 st) {
  // Initial values
  float value     = 0.0;
  float amplitude = .5;

  // Loop of octaves
  for (int i = 0; i < OCTAVES; i++) {
    value += amplitude * noise(st);
    st *= 2.f;
    amplitude *= .5f;
  }
  return value;
}

}  // namespace dubu::block::noise
