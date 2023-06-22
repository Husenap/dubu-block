#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "input/input.hpp"

namespace dubu::block {

class FreeflyCamera : public dubu::event::EventSubscriber {
public:
  FreeflyCamera() {
    Subscribe<dubu::window::EventMouseButtonPress>(
        [this](const auto& e) {
          if (e.button == dubu::window::MouseButtonRight) {
            Input::LockCursor();
            mActive = true;
          }
        },
        Input::Get());
    Subscribe<dubu::window::EventMouseButtonRelease>(
        [this](const auto& e) {
          if (e.button == dubu::window::MouseButtonRight) {
            Input::UnlockCursor();
            mActive = false;
          }
        },
        Input::Get());

    UpdateViewMatrix();
  }

  void Update(float deltaTime) {
    UpdateKeyboardAndMouse(deltaTime);
    UpdateGamepad(deltaTime);
  }

  const glm::mat4& GetViewMatrix() const { return mView; }

  const glm::vec3  GetRight() const { return {mView[0][0], mView[1][0], mView[2][0]}; }
  const glm::vec3  GetUp() const { return {mView[0][1], mView[1][1], mView[2][1]}; }
  const glm::vec3  GetForward() const { return {mView[0][2], mView[1][2], mView[2][2]}; }
  const glm::vec3& GetPosition() const { return mPosition; }

private:
  void UpdateKeyboardAndMouse(float deltaTime) {
    if (!mActive) return;

    const auto delta = Input::GetCursorDelta();

    mPitch = std::clamp(mPitch - delta.y * MouseSensitivity, -90.0f, 90.0f);
    mYaw += delta.x * MouseSensitivity;
    while (mYaw < 0.0f) mYaw += 360.0f;
    while (mYaw > 360.0f) mYaw -= 360.0f;

    const auto forward = GetForward();
    const auto right   = GetRight();
    const auto up      = GetUp();

    float speedMultiplier = 1.0f;
    if (Input::IsKeyDown(dubu::window::KeyLeftShift)) speedMultiplier = 10.0f;
    if (Input::IsKeyDown(dubu::window::KeyLeftControl)) speedMultiplier = 0.1f;

    const float movementSpeed = deltaTime * BaseMovementSpeed * speedMultiplier;

    if (Input::IsKeyDown(dubu::window::KeyW)) {
      mPosition -= forward * movementSpeed;
    }
    if (Input::IsKeyDown(dubu::window::KeyS)) {
      mPosition += forward * movementSpeed;
    }
    if (Input::IsKeyDown(dubu::window::KeyD)) {
      mPosition += right * movementSpeed;
    }
    if (Input::IsKeyDown(dubu::window::KeyA)) {
      mPosition -= right * movementSpeed;
    }
    if (Input::IsKeyDown(dubu::window::KeyE)) {
      mPosition += up * movementSpeed;
    }
    if (Input::IsKeyDown(dubu::window::KeyQ)) {
      mPosition -= up * movementSpeed;
    }

    UpdateViewMatrix();
  }
  void UpdateGamepad(float deltaTime) {
    if (!Input::IsGamepadConnected(0)) return;

    const glm::vec2 rs{Input::GamepadAxis(0, dubu::window::GamepadAxisRightX),
                       Input::GamepadAxis(0, dubu::window::GamepadAxisRightY)};

    if (glm::length2(rs) > 0.1f * 0.1f) {
      mPitch = std::clamp(mPitch + rs.y * GamepadSensitivity * deltaTime, -90.0f, 90.0f);
      mYaw += rs.x * GamepadSensitivity * deltaTime;
      while (mYaw < 0.0f) mYaw += 360.0f;
      while (mYaw > 360.0f) mYaw -= 360.0f;
    }

    float speedMultiplier = std::lerp(
        std::lerp(1.0f, 10.0f, Input::GamepadAxis(0, dubu::window::GamepadAxisRightTrigger)),
        0.1f,
        Input::GamepadAxis(0, dubu::window::GamepadAxisLeftTrigger));
    const float movementSpeed = deltaTime * BaseMovementSpeed * speedMultiplier;

    const glm::vec2 ls{Input::GamepadAxis(0, dubu::window::GamepadAxisLeftX),
                       Input::GamepadAxis(0, dubu::window::GamepadAxisLeftY)};

    if (glm::length2(ls) > 0.1f * 0.1f) {
      mPosition += ls.x * movementSpeed * GetRight();
      mPosition += ls.y * movementSpeed * -GetForward();
    }

    const auto up = GetUp();
    if (Input::IsGamepadButtonDown(0, dubu::window::GamepadButtonRightBumper)) {
      mPosition += up * movementSpeed;
    }
    if (Input::IsGamepadButtonDown(0, dubu::window::GamepadButtonLeftBumper)) {
      mPosition -= up * movementSpeed;
    }

    UpdateViewMatrix();
  }
  void UpdateViewMatrix() {
    mView = glm::translate(
        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(mPitch), glm::vec3(-1, 0, 0)),
                    glm::radians(mYaw),
                    glm::vec3(0, 1, 0)),
        -mPosition);
  }

  bool      mActive = false;
  glm::vec3 mPosition{-8, 167, 25};
  float     mPitch = 0.0f;
  float     mYaw   = 0.0f;
  glm::mat4 mView;

  static constexpr float MouseSensitivity   = 0.1f;
  static constexpr float GamepadSensitivity = 100.0f;
  static constexpr float BaseMovementSpeed  = 5.612f;
};

}  // namespace dubu::block