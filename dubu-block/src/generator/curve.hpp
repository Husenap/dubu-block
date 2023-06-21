#pragma once

#include <array>

#include "imgui/imgui_curve.hpp"

namespace dubu::block {

class Curve {
public:
  Curve(const char*                  label,
        const std::array<ImVec2, 16> startValues = {{{ImGui::CurveTerminator, 0}}})
      : mLabel(label)
      , mPoints(startValues) {}

  void Draw() {
    ImGui::Curve(mLabel,
                 {ImGui::CalcItemWidth(), 200},
                 (int)mPoints.size(),
                 mPoints.data(),
                 &mSelection,
                 {0, 0},
                 {1, 1});
  }

  inline float Value(float alpha) const {
    return ImGui::CurveValueSmooth(alpha, (int)mPoints.size(), mPoints.data());
  }

private:
  const char*            mLabel;
  int                    mSelection = -1;
  std::array<ImVec2, 16> mPoints{{{ImGui::CurveTerminator, 0}}};
};

}  // namespace dubu::block