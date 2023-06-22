#pragma once

#include <optional>

#include <dubu_log/dubu_log.h>
#include <dubu_window/dubu_window.h>
#include <imgui.h>

#include "util/singleton.hpp"

namespace dubu::block {

class Input : public Singleton<Input>,
              public dubu::event::EventSubscriber,
              public dubu::event::EventEmitter {
public:
  virtual ~Input() = default;

  void Init(dubu::window::IWindow& window) {
    mWindow = &window;

    Subscribe<dubu::window::EventKeyPress>(
        [this](const auto& e) {
          if (ImGui::GetIO().WantCaptureKeyboard) return;
          if (e.key == dubu::window::KeyUnknown) return;
          mKeyStates[e.key] = true;
        },
        window);
    Subscribe<dubu::window::EventKeyRelease>(
        [this](const auto& e) {
          if (ImGui::GetIO().WantCaptureKeyboard) return;
          if (e.key == dubu::window::KeyUnknown) return;
          mKeyStates[e.key] = false;
        },
        window);

    Subscribe<dubu::window::EventMouseButtonPress>(
        [this](const auto& e) {
          if (ImGui::GetIO().WantCaptureMouse) return;
          Emit(e);
        },
        window);
    Subscribe<dubu::window::EventMouseButtonRelease>(
        [this](const auto& e) {
          if (ImGui::GetIO().WantCaptureMouse) return;
          Emit(e);
        },
        window);

    Subscribe<dubu::window::EventCursorPos>(
        [this](const auto& e) {
          if (ImGui::GetIO().WantCaptureMouse) return;
          mCursorPosition = {e.posX, e.posY};
        },
        window);

    Subscribe<dubu::window::EventScroll>(
        [](const auto&) {
          if (ImGui::GetIO().WantCaptureMouse) return;
        },
        window);
  }

  static void Update() {
    auto& input = Get();

    if (input.mCursorPreviousPosition) {
      input.mCursorDelta = input.mCursorPosition - *input.mCursorPreviousPosition;
    } else {
      input.mCursorDelta = {0, 0};
    }
    input.mCursorPreviousPosition = input.mCursorPosition;
  }

  static void LockCursor() {
    auto& input = Get();

    if (input.mWindow) input.mWindow->SetCursorMode(dubu::window::CursorMode::Locked);
    input.mCursorPreviousPosition = std::nullopt;
  }
  static void UnlockCursor() {
    auto& input = Get();

    if (input.mWindow) input.mWindow->SetCursorMode(dubu::window::CursorMode::Normal);
    input.mCursorPreviousPosition = std::nullopt;
  }

  static bool IsKeyDown(dubu::window::Key key) { return Get().mKeyStates[key]; }

  static const glm::vec2& GetCursorDelta() { return Get().mCursorDelta; }

private:
  dubu::window::IWindow*   mWindow = nullptr;
  glm::vec2                mCursorDelta;
  glm::vec2                mCursorPosition;
  std::optional<glm::vec2> mCursorPreviousPosition;

  bool mKeyStates[512]{false};
};

}  // namespace dubu::block