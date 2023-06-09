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

#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "io/io.hpp"

namespace dubu::block {

// clang-format off
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture1.Load("assets/textures/hotline-hanyu.png");
    texture2.Load("assets/textures/awesomeface.png");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    VertexShader vertexShader(dubu::block::read_file("assets/shaders/triangle.vert"));
    if (auto err = vertexShader.GetError()) DUBU_LOG_ERROR("shader error: {}", *err);

    FragmentShader fragmentShader(dubu::block::read_file("assets/shaders/triangle.frag"));
    if (auto err = fragmentShader.GetError()) DUBU_LOG_ERROR("shader error: {}", *err);

    mProgram.Link(vertexShader, fragmentShader);
    if (auto err = mProgram.GetError()) DUBU_LOG_ERROR("shader program error: {}", *err);
    mProgram.Use();
    glUniform1i(mProgram.GetUniformLocation("texture1"), 0);
    glUniform1i(mProgram.GetUniformLocation("texture2"), 1);
  }

  virtual void Update() override {
    if (width <= 0 || height <= 0) return;

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model           = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view           = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.f);

    glm::mat4 mvp = projection * view * model;

    glm::mat4 trans = glm::mat4(1.0f);
    trans           = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans           = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0, 0, 1));
    trans           = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

    mProgram.Use();
    texture1.Bind(GL_TEXTURE0);
    texture2.Bind(GL_TEXTURE1);

    glUniformMatrix4fv(mProgram.GetUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (ImGui::Begin("Debug")) {
      static bool wireframe    = false;
      static bool depthTesting = true;
      if (ImGui::Checkbox("Wireframe", &wireframe)) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
      }
      if (ImGui::Checkbox("Depth Testing", &depthTesting)) {
        (depthTesting ? glEnable : glDisable)(GL_DEPTH_TEST);
      }
    }
    ImGui::End();
  }

private:
  ShaderProgram      mProgram;
  GLuint             mImage;
  dubu::event::Token mKeyPressToken, mResizeToken;

  int width  = 1920;
  int height = 1080;

  GLuint  vbo, vao, ebo;
  Texture texture1, texture2;
};
}  // namespace dubu::block

int main() {
  dubu::block::App app;

  app.Run();

  return 0;
}
