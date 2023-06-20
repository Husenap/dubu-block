#pragma once

#include <FastNoiseLite.h>
#include <glm/glm.hpp>

#include "generator/curve.hpp"

namespace dubu::block {

class Seed {
public:
  Seed(int seed) {
    continentalnessNoise.SetSeed(seed);
    continentalnessNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    continentalnessNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    continentalnessNoise.SetFractalOctaves(4);
  }

  float Continentalness(const glm::vec2& p) const {
    return continentalnessCurve.Value(continentalnessNoise.GetNoise(p.x, p.y) * 0.5f + 0.5f);
  }

  void Debug() { continentalnessCurve.Draw(); }

private:
  FastNoiseLite continentalnessNoise;
  Curve         continentalnessCurve{"Continentalness"};
};

}  // namespace dubu::block