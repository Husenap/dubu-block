#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>

#include <dubu_event/dubu_event.h>
#include <dubu_log/dubu_log.h>
#include <dubu_opengl_app/dubu_opengl_app.hpp>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <stb/stb_image.h>

#include "game/atlas.hpp"
#include "game/chunk.hpp"
#include "gl/gif.hpp"
#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "io/io.hpp"

namespace dubu::block {

// clang-format off
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
                                          
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
                                          
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
                                          
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
                                          
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                                          
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
};
// clang-format on
constexpr GLuint indices[]{0, 1, 3, 3, 1, 2};

class App : public dubu::opengl_app::AppBase {
public:
  App()
      : dubu::opengl_app::AppBase({
            .appName = "Example App",
        }) {}
  virtual ~App() = default;

protected:
  virtual void Init() override {
    mKeyPressToken = mWindow->RegisterListener<dubu::window::EventKeyPress>(
        [&](const auto& e) { DUBU_LOG_DEBUG("key event: {}", e.key); });
    mResizeToken = mWindow->RegisterListener<dubu::window::EventResize>([&](const auto& e) {
      width  = e.width;
      height = e.height;
      DUBU_LOG_DEBUG("Window resized: ({}, {})", width, height);
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    atlas = std::make_unique<Atlas>();
    chunk = std::make_unique<Chunk>(*atlas);
    // atlas.Load("assets/textures/awesomeface.png");

    /*
    gif1.Load("assets/textures/cute-bear.gif");
    gif2.Load("assets/textures/bears.gif");

    texture1.Load("assets/textures/hotline-hanyu.png");
    texture2.Load("assets/textures/awesomeface.png");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    VertexShader   vertexShader(dubu::block::read_file("assets/shaders/triangle.vert"));
    FragmentShader fragmentShader(dubu::block::read_file("assets/shaders/triangle.frag"));
    mProgram.Link(vertexShader, fragmentShader);
    if (const auto err = mProgram.GetError()) DUBU_LOG_ERROR("shader program error: {}", *err);
    mProgram.Use();
    glUniform1i(mProgram.GetUniformLocation("texture1"), 0);
    glUniform1i(mProgram.GetUniformLocation("texture2"), 1);
    */
  }

  virtual void Update() override {
    if (width <= 0 || height <= 0) return;

    static float previousTime = static_cast<float>(glfwGetTime());
    const float  time         = static_cast<float>(glfwGetTime());
    const float  deltaTime    = time - previousTime;
    previousTime              = time;

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 model = glm::mat4(1.0f);

    const glm::mat4 view = glm::lookAt(cameraPosition, cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 500.f);

    const glm::mat4 mvp = projection * view * model;

    atlas->Bind(GL_TEXTURE0);
    chunk->Draw(mvp);

    /*
    mProgram.Use();
    gif1.Bind(GL_TEXTURE0);
    gif1.Update(deltaTime);
    gif2.Bind(GL_TEXTURE1);
    gif2.Update(deltaTime);
    glUniform1f(mProgram.GetUniformLocation("frame1"), gif1.GetFrame());
    glUniform1f(mProgram.GetUniformLocation("frame2"), gif2.GetFrame());

    glUniformMatrix4fv(mProgram.GetUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    */

    if (ImGui::Begin("Debug")) {
      static bool wireframe    = false;
      static bool depthTesting = true;
      if (ImGui::Checkbox("Wireframe", &wireframe)) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
      }
      if (ImGui::Checkbox("Depth Testing", &depthTesting)) {
        (depthTesting ? glEnable : glDisable)(GL_DEPTH_TEST);
      }
      ImGui::DragFloat3("Camera Position", glm::value_ptr(cameraPosition));
      ImGui::DragFloat3("Camera Look At", glm::value_ptr(cameraLookAt));

      ImGui::Separator();
      static float bias = 0.f;
      ImGui::DragFloat("LOD Bias", &bias);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

      atlas->Debug();
      // chunk->Debug();
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();
  }

private:
  ShaderProgram      mProgram;
  GLuint             mImage;
  dubu::event::Token mKeyPressToken, mResizeToken;

  int width  = 1920;
  int height = 1080;

  GLuint  vbo, vao, ebo;
  Texture texture1, texture2;
  Gif     gif1, gif2;

  std::unique_ptr<Chunk> chunk;
  std::unique_ptr<Atlas> atlas;
  glm::vec3              cameraPosition{6, 4, 6}, cameraLookAt{0, 0.5f, 0};
};
}  // namespace dubu::block

int main() {
  dubu::block::App app;

  app.Run();

  return 0;
}
