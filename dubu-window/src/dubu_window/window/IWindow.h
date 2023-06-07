#pragma once

#include <dubu_event/dubu_event.h>

#include "Events.h"


namespace dubu::window {

enum class CursorMode {
  Normal,
  Hidden,
  Locked,
};

class IWindow : public dubu::event::EventEmitter {
public:
  IWindow();
  virtual ~IWindow();

  virtual void PollEvents()  = 0;
  virtual void SwapBuffers() = 0;

  [[nodiscard]] virtual bool ShouldClose() const = 0;
  [[nodiscard]] virtual bool IsHovered() const   = 0;
  [[nodiscard]] virtual bool IsFocused() const   = 0;

  virtual void SetCursorMode(dubu::window::CursorMode cursorMode) = 0;

  void SimulateEventKey(const EventKey& e);
  void SimulateEventChar(const EventChar& e);
  void SimulateEventCursorPos(const EventCursorPos& e);
  void SimulateEventMouseButton(const EventMouseButton& e);

  [[nodiscard]] virtual bool                        IsGamepadConnected(int gamepadIndex);
  [[nodiscard]] virtual std::optional<GamepadState> GetGamepadState(int gamepadIndex);

protected:
private:
};

}  // namespace dubu::window