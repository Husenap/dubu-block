#include "AppBase.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <dubu_log/dubu_log.h>
#include <glad/glad.h>

namespace dubu::opengl_app {

AppBase::AppBase(const CreateInfo& createInfo)
    : mCreateInfo(createInfo) {
  dubu::log::Register<dubu::log::ConsoleLogger>();
}

void AppBase::Run() {
  InitWindow();

  InitImGui();

  Init();

  ImGuiIO& io = ImGui::GetIO();

  while (!mWindow->ShouldClose()) {
    mWindow->PollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Update();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    mWindow->SwapBuffers();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void AppBase::InitWindow() {
  mWindow =
      std::move(std::make_unique<dubu::window::GLFWWindow>(dubu::window::GLFWWindow::CreateInfo{
          .width  = mCreateInfo.width,
          .height = mCreateInfo.height,
          .title  = mCreateInfo.appName,
          .api    = dubu::window::GLFWWindow::Api::OpenGL,
      }));

  glfwMakeContextCurrent(mWindow->GetGLFWHandle());
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  glViewport(0, 0, mCreateInfo.width, mCreateInfo.height);
  glfwSwapInterval(mCreateInfo.swapInterval);

  mResizeToken = mWindow->RegisterListener<dubu::window::EventResize>(
      [](const auto& e) { glViewport(0, 0, e.width, e.height); });
  mKeyPressToken = mWindow->RegisterListener<dubu::window::EventKeyPress>([&](const auto& e) {
    if (e.key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(mWindow->GetGLFWHandle(), GLFW_TRUE);
    }
  });
}

void AppBase::InitImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui_ImplGlfw_InitForOpenGL(mWindow->GetGLFWHandle(), true);
  ImGui_ImplOpenGL3_Init("#version 330");

  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  colors[ImGuiCol_WindowBg]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_ChildBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_PopupBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_Border]                = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
  colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_FrameBg]               = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
  colors[ImGuiCol_FrameBgActive]         = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
  colors[ImGuiCol_TitleBg]               = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
  colors[ImGuiCol_TitleBgActive]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
  colors[ImGuiCol_MenuBarBg]             = ImVec4(0.33f, 0.33f, 0.33f, 0.75f);
  colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
  colors[ImGuiCol_CheckMark]             = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
  colors[ImGuiCol_SliderGrab]            = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
  colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
  colors[ImGuiCol_Button]                = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
  colors[ImGuiCol_ButtonHovered]         = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
  colors[ImGuiCol_ButtonActive]          = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
  colors[ImGuiCol_Header]                = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
  colors[ImGuiCol_HeaderHovered]         = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
  colors[ImGuiCol_HeaderActive]          = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
  colors[ImGuiCol_Separator]             = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
  colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.70f, 0.67f, 0.60f, 0.29f);
  colors[ImGuiCol_SeparatorActive]       = ImVec4(0.70f, 0.67f, 0.60f, 0.67f);
  colors[ImGuiCol_ResizeGrip]            = ImVec4(0.99f, 0.78f, 0.61f, 1.00f);
  colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.78f, 0.62f, 0.48f, 1.00f);
  colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.86f, 0.68f, 0.53f, 1.00f);
  colors[ImGuiCol_Tab]                   = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_TabHovered]            = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  colors[ImGuiCol_TabActive]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
  colors[ImGuiCol_TabUnfocused]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
  colors[ImGuiCol_DockingPreview]        = ImVec4(1.00f, 0.37f, 0.64f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.75f, 0.28f, 0.47f, 1.00f);
  colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.72f, 0.30f, 1.00f);
  colors[ImGuiCol_PlotHistogram]         = ImVec4(0.68f, 0.84f, 0.51f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.48f, 0.59f, 0.42f, 1.00f);
  colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
  colors[ImGuiCol_DragDropTarget]        = ImVec4(0.51f, 0.83f, 0.98f, 1.00f);
  colors[ImGuiCol_NavHighlight]          = ImVec4(0.38f, 0.62f, 0.73f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

  style.PopupRounding = 3.f;

  style.WindowPadding    = ImVec2(2.f, 2.f);
  style.FramePadding     = ImVec2(4.f, 4.f);
  style.ItemSpacing      = ImVec2(3.f, 3.f);
  style.ItemInnerSpacing = ImVec2(3.f, 3.f);

  style.ScrollbarSize = 18.f;

  style.WindowBorderSize = 1.f;
  style.ChildBorderSize  = 1.f;
  style.PopupBorderSize  = 1.f;
  style.FrameBorderSize  = 0.f;

  style.WindowRounding    = 2.f;
  style.ChildRounding     = 2.f;
  style.FrameRounding     = 2.f;
  style.ScrollbarRounding = 2.f;
  style.GrabRounding      = 3.f;

  style.TabBorderSize = 0.f;
  style.TabRounding   = 3.f;

  style.WindowMenuButtonPosition = ImGuiDir_Right;
}

}  // namespace dubu::opengl_app
