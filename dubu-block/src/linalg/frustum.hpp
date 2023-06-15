#pragma once

#include <array>

#include <glm/glm.hpp>
#include <imgui.h>

#include "linalg/aabb.hpp"
#include "linalg/plane.hpp"

namespace dubu::block {

class Frustum {
public:
  Frustum(glm::mat4 inverseViewProjection) {
    glm::vec4 nearBottomLeft  = inverseViewProjection * glm::vec4{-1, -1, 1, 1};
    glm::vec4 nearBottomRight = inverseViewProjection * glm::vec4{1, -1, 1, 1};
    glm::vec4 nearTopLeft     = inverseViewProjection * glm::vec4{-1, 1, 1, 1};
    glm::vec4 nearTopRight    = inverseViewProjection * glm::vec4{1, 1, 1, 1};
    glm::vec4 farBottomLeft   = inverseViewProjection * glm::vec4{-1, -1, -1, 1};
    glm::vec4 farBottomRight  = inverseViewProjection * glm::vec4{1, -1, -1, 1};
    glm::vec4 farTopLeft      = inverseViewProjection * glm::vec4{-1, 1, -1, 1};
    glm::vec4 farTopRight     = inverseViewProjection * glm::vec4{1, 1, -1, 1};

    nearBottomLeft /= nearBottomLeft.w;
    nearBottomRight /= nearBottomRight.w;
    nearTopLeft /= nearTopLeft.w;
    nearTopRight /= nearTopRight.w;
    farBottomLeft /= farBottomLeft.w;
    farBottomRight /= farBottomRight.w;
    farTopLeft /= farTopLeft.w;
    farTopRight /= farTopRight.w;

    planes[0] = Plane(farTopLeft, nearTopLeft, nearBottomLeft);          // Left
    planes[1] = Plane(nearTopRight, farTopRight, farBottomRight);        // Right
    planes[2] = Plane(farTopLeft, farTopRight, nearTopRight);            // Top
    planes[3] = Plane(nearBottomLeft, nearBottomRight, farBottomRight);  // Bottom
    planes[4] = Plane(nearTopLeft, nearTopRight, nearBottomRight);       // Near
    planes[5] = Plane(farTopRight, farTopLeft, farBottomLeft);           // Far
  }

  bool IsOutside(const AABB& aabb) const {
    for (auto& plane : planes) {
      if (aabb.IsOutside(plane)) return true;
    }

    return false;
  }

  void Debug() {
    ImGui::InputFloat3("Left Plane Normal", &planes[0].n.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Right Plane Normal", &planes[1].n.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Top Plane Normal", &planes[2].n.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Bottom Plane Normal", &planes[3].n.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Near Plane Normal", &planes[4].n.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Far Plane Normal", &planes[5].n.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
  }

private:
  std::array<Plane, 6> planes;
};
}  // namespace dubu::block