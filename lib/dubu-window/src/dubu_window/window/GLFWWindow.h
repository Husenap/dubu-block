#pragma once

#include <GLFW/glfw3.h>

#include "IWindow.h"

namespace dubu::window {

class GLFWWindow : public IWindow {
public:
  enum class Api { NoApi, OpenGL };
  struct CreateInfo {
    int         width               = 800;
    int         height              = 600;
    std::string title               = "dubu-window";
    Api         api                 = Api::NoApi;
    int         contextVersionMajor = 4;
    int         contextVersionMinor = 6;
    int         opengl_profile      = GLFW_OPENGL_CORE_PROFILE;
  };

public:
  GLFWWindow(const CreateInfo& createInfo);
  GLFWWindow(int width, int height, std::string_view title);
  ~GLFWWindow();

  void PollEvents() override;
  void SwapBuffers() override;

  [[nodiscard]] bool ShouldClose() const override;
  [[nodiscard]] bool IsHovered() const override;
  [[nodiscard]] bool IsFocused() const override;

  void SetCursorMode(dubu::window::CursorMode cursorMode) override;

  [[nodiscard]] bool                        IsGamepadConnected(int gamepadIndex) override;
  [[nodiscard]] std::optional<GamepadState> GetGamepadState(int gamepadIndex) override;

  [[nodiscard]] GLFWwindow* GetGLFWHandle() const { return mWindow; }

private:
  static GLFWWindow* GetUserWindow(GLFWwindow* window);
  static void        WindowFramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void WindowDropCallback(GLFWwindow* window, int count, const char** paths);
  static void WindowContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY);
  static void WindowCharCallback(GLFWwindow* window, uint32_t codepoint);
  static void WindowCursorPosCallback(GLFWwindow* window, double posX, double posY);
  static void WindowCursorEnterCallback(GLFWwindow* window, int entered);
  static void WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void WindowScrollCallback(GLFWwindow* window, double offsetX, double offsetY);

  static int ConstructionCounter;

  GLFWwindow* mWindow;
  CursorMode  mCursorMode;
};

}  // namespace dubu::window