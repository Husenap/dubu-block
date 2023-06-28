#include "GLFWWindow.h"

#include <iostream>
#include <string>
#include <string_view>

namespace dubu::window {

static void GLFWErrorCallback(int error, const char* msg) {
  std::string s;
  s = " [" + std::to_string(error) + "] " + msg + '\n';
  std::cerr << s << std::endl;
}

int GLFWWindow::ConstructionCounter = 0;

GLFWWindow::GLFWWindow(int width, int height, std::string_view title)
    : GLFWWindow(CreateInfo{.width = width, .height = height, .title = title.data()}) {}

GLFWWindow::GLFWWindow(const CreateInfo& createInfo) {
  if (ConstructionCounter <= 0) {
    glfwInit();
  }
  glfwSetErrorCallback(GLFWErrorCallback);
  ++ConstructionCounter;

  switch (createInfo.api) {
  case Api::NoApi:
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    break;
  case Api::OpenGL:
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif
    break;
  }

  mWindow = glfwCreateWindow(
      createInfo.width, createInfo.height, createInfo.title.c_str(), nullptr, nullptr);

  glfwSetWindowUserPointer(mWindow, this);

  glfwSetFramebufferSizeCallback(mWindow, WindowFramebufferSizeCallback);
  glfwSetKeyCallback(mWindow, WindowKeyCallback);
  glfwSetDropCallback(mWindow, WindowDropCallback);
  glfwSetWindowContentScaleCallback(mWindow, WindowContentScaleCallback);
  glfwSetCharCallback(mWindow, WindowCharCallback);
  glfwSetCursorPosCallback(mWindow, WindowCursorPosCallback);
  glfwSetCursorEnterCallback(mWindow, WindowCursorEnterCallback);
  glfwSetMouseButtonCallback(mWindow, WindowMouseButtonCallback);
  glfwSetScrollCallback(mWindow, WindowScrollCallback);
}

GLFWWindow::~GLFWWindow() {
  if (mWindow) {
    glfwDestroyWindow(mWindow);
  }

  --ConstructionCounter;
  if (ConstructionCounter <= 0) {
    glfwTerminate();
  }
}

void GLFWWindow::PollEvents() {
  glfwPollEvents();
}

void GLFWWindow::SwapBuffers() {
  glfwSwapBuffers(mWindow);
}

bool GLFWWindow::ShouldClose() const {
  return glfwWindowShouldClose(mWindow);
}

bool GLFWWindow::IsHovered() const {
  return glfwGetWindowAttrib(mWindow, GLFW_HOVERED);
}

bool GLFWWindow::IsFocused() const {
  return glfwGetWindowAttrib(mWindow, GLFW_FOCUSED);
}

void GLFWWindow::SetCursorMode(dubu::window::CursorMode cursorMode) {
  switch (cursorMode) {
  case CursorMode::Normal:
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case CursorMode::Hidden:
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    break;
  case CursorMode::Locked:
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
  }
}

bool GLFWWindow::IsGamepadConnected(int gamepadIndex) {
  return glfwJoystickIsGamepad(gamepadIndex);
}

std::optional<GamepadState> GLFWWindow::GetGamepadState(int gamepadIndex) {
  if (!IsGamepadConnected(gamepadIndex)) {
    return std::nullopt;
  }

  GLFWgamepadstate state;

  if (glfwGetGamepadState(gamepadIndex, &state) == GLFW_FALSE) {
    return std::nullopt;
  }

  GamepadState gps;

  for (uint8_t i = 0; i <= GamepadButtonLast; ++i) {
    gps.buttons[i] = state.buttons[i] == GLFW_PRESS ? Action::Press : Action::Release;
  }
  gps.axes[GamepadAxisLeftX]        = state.axes[GamepadAxisLeftX];
  gps.axes[GamepadAxisLeftY]        = -state.axes[GamepadAxisLeftY];
  gps.axes[GamepadAxisRightX]       = state.axes[GamepadAxisRightX];
  gps.axes[GamepadAxisRightY]       = -state.axes[GamepadAxisRightY];
  gps.axes[GamepadAxisLeftTrigger]  = state.axes[GamepadAxisLeftTrigger] * 0.5f + 0.5f;
  gps.axes[GamepadAxisRightTrigger] = state.axes[GamepadAxisRightTrigger] * 0.5f + 0.5f;

  return gps;
}

GLFWWindow* GLFWWindow::GetUserWindow(GLFWwindow* window) {
  return reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
}

void GLFWWindow::WindowFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    userWindow->Emit(EventResize{.width = width, .height = height});
  }
}

void GLFWWindow::WindowKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    switch (action) {
    case GLFW_PRESS:
      userWindow->Emit(EventKey{.key      = static_cast<Key>(key),
                                .scancode = scancode,
                                .action   = Action::Press,
                                .mods     = static_cast<Mods>(mods)});
      userWindow->Emit(EventKeyPress{
          .key = static_cast<Key>(key), .scancode = scancode, .mods = static_cast<Mods>(mods)});
      break;
    case GLFW_RELEASE:
      userWindow->Emit(EventKey{.key      = static_cast<Key>(key),
                                .scancode = scancode,
                                .action   = Action::Release,
                                .mods     = static_cast<Mods>(mods)});
      userWindow->Emit(EventKeyRelease{
          .key = static_cast<Key>(key), .scancode = scancode, .mods = static_cast<Mods>(mods)});
      break;
    case GLFW_REPEAT:
      userWindow->Emit(EventKey{.key      = static_cast<Key>(key),
                                .scancode = scancode,
                                .action   = Action::Repeat,
                                .mods     = static_cast<Mods>(mods)});
      userWindow->Emit(EventKeyRepeat{
          .key = static_cast<Key>(key), .scancode = scancode, .mods = static_cast<Mods>(mods)});
      break;
    default:
      break;
    }
  }
}

void GLFWWindow::WindowDropCallback(GLFWwindow* window, int count, const char** paths) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    for (int i = 0; i < count; ++i) {
      userWindow->Emit(EventDroppedFile{.file = paths[i]});
    }
  }
}

void GLFWWindow::WindowContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    userWindow->Emit(EventContentScale{.scaleX = scaleX, .scaleY = scaleY});
  }
}

void GLFWWindow::WindowCharCallback(GLFWwindow* window, uint32_t codepoint) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    userWindow->Emit(EventChar{.codepoint = codepoint});
  }
}

void GLFWWindow::WindowCursorPosCallback(GLFWwindow* window, double posX, double posY) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    userWindow->Emit(EventCursorPos{.posX = posX, .posY = posY});
  }
}

void GLFWWindow::WindowCursorEnterCallback(GLFWwindow* window, int entered) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    if (entered) {
      userWindow->Emit<EventCursorEnter>();
    } else {
      userWindow->Emit<EventCursorLeave>();
    }
  }
}

void GLFWWindow::WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    switch (action) {
    case GLFW_PRESS:
      userWindow->Emit(EventMouseButton{.button = static_cast<MouseButton>(button),
                                        .action = Action::Press,
                                        .mods   = static_cast<Mods>(mods)});
      userWindow->Emit(EventMouseButtonPress{.button = static_cast<MouseButton>(button),
                                             .mods   = static_cast<Mods>(mods)});
      break;
    case GLFW_RELEASE:
      userWindow->Emit(EventMouseButton{.button = static_cast<MouseButton>(button),
                                        .action = Action::Release,
                                        .mods   = static_cast<Mods>(mods)});
      userWindow->Emit(EventMouseButtonRelease{.button = static_cast<MouseButton>(button),
                                               .mods   = static_cast<Mods>(mods)});
      break;
    default:
      break;
    }
  }
}

void GLFWWindow::WindowScrollCallback(GLFWwindow* window, double offsetX, double offsetY) {
  if (auto userWindow = GetUserWindow(window); userWindow) {
    userWindow->Emit(EventScroll{.offsetX = offsetX, .offsetY = offsetY});
  }
}

}  // namespace dubu::window