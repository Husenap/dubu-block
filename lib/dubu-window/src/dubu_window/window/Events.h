#pragma once

#include <filesystem>

#include "Definitions.h"

namespace dubu::window {

struct EventResize {
  int32_t width;
  int32_t height;
};

struct EventContentScale {
  float scaleX;
  float scaleY;
};

struct EventKey {
  Key     key;
  int32_t scancode;
  Action  action;
  Mods    mods;
};
struct EventKeyPress {
  Key     key;
  int32_t scancode;
  Mods    mods;
};
struct EventKeyRelease {
  Key     key;
  int32_t scancode;
  Mods    mods;
};
struct EventKeyRepeat {
  Key     key;
  int32_t scancode;
  Mods    mods;
};

struct EventChar {
  uint32_t codepoint;
};

struct EventCursorPos {
  double posX;
  double posY;
};
struct EventCursorEnter {};
struct EventCursorLeave {};

struct EventMouseButton {
  MouseButton button;
  Action      action;
  Mods        mods;
};
struct EventMouseButtonPress {
  MouseButton button;
  Mods        mods;
};
struct EventMouseButtonRelease {
  MouseButton button;
  Mods        mods;
};

struct EventScroll {
  double offsetX;
  double offsetY;
};

struct EventDroppedFile {
  std::filesystem::path file;
};

}  // namespace dubu::window