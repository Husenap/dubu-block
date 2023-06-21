#pragma once

#include <FastNoiseLite.h>
#include <glm/glm.hpp>

#include "generator/curve.hpp"

namespace dubu::block {

class Seed {
public:
  Seed(int seed) {
    SetSeed(seed);

    continentalnessNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    continentalnessNoise.SetFrequency(0.004f);
    continentalnessNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    continentalnessNoise.SetFractalOctaves(6);

    erosionNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    erosionNoise.SetFrequency(0.02f);
    erosionNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    erosionNoise.SetFractalOctaves(5);

    peaksAndValleysNoise.SetFrequency(0.012f);
    peaksAndValleysNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    peaksAndValleysNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    peaksAndValleysNoise.SetFractalOctaves(1);

    peaksAndValleysDomainWarp.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2Reduced);
    peaksAndValleysDomainWarp.SetDomainWarpAmp(65.0f);
    peaksAndValleysDomainWarp.SetFrequency(0.015f);
    peaksAndValleysDomainWarp.SetFractalType(FastNoiseLite::FractalType_DomainWarpIndependent);
    peaksAndValleysDomainWarp.SetFractalOctaves(3);
  }

  float Continentalness(const glm::vec2& p) const {
    return continentalnessCurve.Value(continentalnessNoise.GetNoise(p.x, p.y) * 0.5f + 0.5f);
  }

  float Erosion(const glm::vec2& p) const {
    return erosionCurve.Value(erosionNoise.GetNoise(p.x, p.y) * 0.5f + 0.5f);
  }

  float PeaksAndValleys(glm::vec2 p) const {
    peaksAndValleysDomainWarp.DomainWarp(p.x, p.y);
    return peaksAndValleysCurve.Value((-peaksAndValleysNoise.GetNoise(p.x, p.y)) * 0.5f + 0.5f);
  }

  void SetSeed(int seed) {
    mSeed = seed;
    continentalnessNoise.SetSeed(mSeed);
    erosionNoise.SetSeed(mSeed);
    peaksAndValleysNoise.SetSeed(mSeed);
    peaksAndValleysDomainWarp.SetSeed(mSeed);
  }

  void Debug() {
    if (ImGui::DragInt("Seed", &mSeed)) SetSeed(mSeed);
    continentalnessCurve.Draw();
    erosionCurve.Draw();
    peaksAndValleysCurve.Draw();
  }

private:
  FastNoiseLite continentalnessNoise;
  Curve         continentalnessCurve{"Continentalness",
                                     {{{0, 0},
                                       {0.27f, 0.12f},
                                       {0.44f, 0.47f},
                                       {0.58f, 0.54f},
                                       {0.72f, 0.91f},
                                       {1, 0.92f},
                                       {ImGui::CurveTerminator, 0}}}};

  FastNoiseLite erosionNoise;
  Curve         erosionCurve{"Erosion"};

  FastNoiseLite peaksAndValleysNoise;  // invert this value
  FastNoiseLite peaksAndValleysDomainWarp;
  Curve         peaksAndValleysCurve{"Peaks & Valleys"};

  int mSeed = 1337;
};

}  // namespace dubu::block