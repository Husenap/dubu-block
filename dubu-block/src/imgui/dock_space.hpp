#pragma once

#include <imgui.h>

namespace dubu::block {

void DrawDockSpace() {
  static constexpr ImGuiWindowFlags dockSpaceWindowFlags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;

  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
  ImGui::PushStyleColor(ImGuiCol_WindowBg, {0, 0, 0, 0});
  ImGui::Begin("#DOCK_SPACE", nullptr, dockSpaceWindowFlags);

  ImGui::DockSpace(
      ImGui::GetID("DOCK_SPACE_WINDOW"), {0.f, 0.f}, ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();
  ImGui::PopStyleColor(1);
  ImGui::PopStyleVar(3);
}

}  // namespace dubu::block