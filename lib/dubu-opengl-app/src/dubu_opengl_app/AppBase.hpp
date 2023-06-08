#pragma once

#include <memory>
#include <string>

#include <dubu_window/dubu_window.h>

namespace dubu::opengl_app {

class AppBase {
public:
  struct CreateInfo {
    int         width        = 1920;
    int         height       = 1080;
    std::string appName      = "dubu-opengl-app";
    int         swapInterval = 0;
  };

public:
  AppBase(const CreateInfo& createInfo);
  virtual ~AppBase() = default;

  void Run();

protected:
  virtual void Init()   = 0;
  virtual void Update() = 0;

  std::unique_ptr<dubu::window::GLFWWindow> mWindow;

private:
  void InitWindow();
  void InitImGui();

  void DrawDockSpace();

  CreateInfo mCreateInfo;

  dubu::event::Token mResizeToken;
  dubu::event::Token mKeyPressToken;
};

}  // namespace dubu::opengl_app