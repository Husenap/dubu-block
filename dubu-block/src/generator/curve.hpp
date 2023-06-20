#pragma once

#include <array>

#include "imgui/imgui_curve.hpp"

namespace dubu::block {

class Curve {
public:
  Curve(const char* label)
      : mLabel(label) {}

  void Draw() {
    ImGui::Curve(mLabel,
                 {ImGui::CalcItemWidth(), 200},
                 (int)pts.size(),
                 pts.data(),
                 &mSelection,
                 {0, 0},
                 {1, 1});
  }

  inline float Value(float alpha) const {
    return ImGui::CurveValueSmooth(alpha, (int)pts.size(), pts.data());
  }

private:
  const char*            mLabel;
  int                    mSelection = -1;
  std::array<ImVec2, 16> pts{{{ImGui::CurveTerminator, 0}}};
};

}  // namespace dubu::block