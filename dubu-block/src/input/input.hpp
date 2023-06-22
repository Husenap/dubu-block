#pragma once

#include <array>
#include <optional>

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

  inline static void Update() { Get().InternalUpdate(); }

  inline static void LockCursor() {
    auto& input = Get();

    if (input.mWindow) input.mWindow->SetCursorMode(dubu::window::CursorMode::Locked);
    input.mCursorPreviousPosition = std::nullopt;
  }
  inline static void UnlockCursor() {
    auto& input = Get();

    if (input.mWindow) input.mWindow->SetCursorMode(dubu::window::CursorMode::Normal);
    input.mCursorPreviousPosition = std::nullopt;
  }

  inline static bool IsKeyDown(dubu::window::Key key) { return Get().mKeyStates[key]; }

  inline static float GamepadAxis(int gamepad, dubu::window::GamepadAxis axis) {
    return Get().mPreviousGamepadState[gamepad].axes[axis];
  }
  inline static bool IsGamepadButtonDown(int gamepad, dubu::window::GamepadButton button) {
    return Get().mPreviousGamepadState[gamepad].buttons[button] == dubu::window::Action::Press;
  }
  inline static bool IsGamepadConnected(int gamepad) {
    return Get().mWindow->IsGamepadConnected(gamepad);
  }

  inline static const glm::vec2& GetCursorDelta() { return Get().mCursorDelta; }

private:
  inline void InternalUpdate() {
    if (mCursorPreviousPosition) {
      mCursorDelta = mCursorPosition - *mCursorPreviousPosition;
    } else {
      mCursorDelta = {0, 0};
    }
    mCursorPreviousPosition = mCursorPosition;

    for (uint8_t gamepadIndex = 0; gamepadIndex < mPreviousGamepadState.size(); ++gamepadIndex) {
      if (auto gps = mWindow->GetGamepadState(gamepadIndex); gps) {
        mPreviousGamepadState[gamepadIndex] = *gps;
      }
    }
  }

  dubu::window::IWindow*   mWindow                 = nullptr;
  glm::vec2                mCursorDelta            = {};
  glm::vec2                mCursorPosition         = {};
  std::optional<glm::vec2> mCursorPreviousPosition = {};

  std::array<bool, 512> mKeyStates = {false};

  std::array<dubu::window::GamepadState, 16> mPreviousGamepadState = {};
};

}  // namespace dubu::block