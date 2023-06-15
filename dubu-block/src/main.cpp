#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <type_traits>

#include <dubu_event/dubu_event.h>
#include <dubu_log/dubu_log.h>
#include <dubu_opengl_app/dubu_opengl_app.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <stb/stb_image.h>

#include "game/atlas.hpp"
#include "game/block.hpp"
#include "game/chunk.hpp"
#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "io/io.hpp"
#include "threading/DoubleBuffer.hpp"

namespace dubu::block {

class App : public dubu::opengl_app::AppBase {
public:
  App()
      : dubu::opengl_app::AppBase({
            .appName = "Example App",
        }) {}
  virtual ~App() = default;

protected:
  virtual void Init() override {
    mResizeToken = mWindow->RegisterListener<dubu::window::EventResize>([&](const auto& e) {
      width  = e.width;
      height = e.height;
      DUBU_LOG_DEBUG("Window resized: ({}, {})", width, height);
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    atlas = std::make_unique<Atlas>(blockDescriptions);
  }

  virtual void Update() override {
    if (!chunkBuffer.GetWriteBuffer().empty()) {
      chunks.emplace(chunkBuffer.GetWriteBuffer().front(),
                     std::make_unique<Chunk>(*atlas, blockDescriptions));
      chunkBuffer.GetWriteBuffer().pop_front();
    }
    if (width <= 0 || height <= 0) return;

    static float previousTime = static_cast<float>(glfwGetTime());
    const float  time         = static_cast<float>(glfwGetTime());
    const float  deltaTime    = time - previousTime;
    previousTime              = time;

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 cameraLookAt =
        cameraPosition + glm::vec3{std::cosf(glm::radians(yaw)) * std::cosf(glm::radians(pitch)),
                                   std::sinf(glm::radians(pitch)),
                                   std::sinf(glm::radians(yaw)) * std::cosf(glm::radians(pitch))};
    const glm::mat4 view = glm::lookAt(cameraPosition, cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 500.f);
    const glm::mat4 viewProjection = projection * view;
    const glm::mat4 model          = glm::mat4(1.0f);

    int triangles   = 0;
    int chunksDrawn = 0;

    atlas->Bind(GL_TEXTURE0);
    for (int i = -RenderDistance; i < RenderDistance; ++i) {
      for (int j = -RenderDistance; j < RenderDistance; ++j) {
        const int x = std::roundf(cameraPosition.x / Chunk::ChunkSize.x) + i;
        const int z = std::roundf(cameraPosition.z / Chunk::ChunkSize.z) + j;

        const float dx = (x + 0.5f) - cameraPosition.x / (float)(Chunk::ChunkSize.x);
        const float dz = (z + 0.5f) - cameraPosition.z / (float)(Chunk::ChunkSize.z);
        if (dx * dx + dz * dz > RenderDistance * RenderDistance) continue;

        ChunkCoords chunkCoords{x, z};
        if (auto chunk = chunks.find(chunkCoords); chunk != chunks.end()) {
          const glm::mat4 mvp =
              viewProjection *
              glm::translate(model, glm::vec3(x * Chunk::ChunkSize.x, 0, z * Chunk::ChunkSize.z));
          triangles += chunk->second->Draw(mvp);
          ++chunksDrawn;
        } else {
          if (!seen.contains(chunkCoords)) {
            chunkBuffer.GetWriteBuffer().push_back(chunkCoords);
            seen.insert(chunkCoords);
          }
        }
      }
    }

    if (ImGui::Begin("Debug")) {
      static bool wireframe = false;
      if (ImGui::Checkbox("Wireframe", &wireframe)) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
      }
      ImGui::Separator();

      ImGui::DragFloat3("Camera Position", glm::value_ptr(cameraPosition));
      ImGui::DragFloat("Pitch", &pitch, 0.1f, -89.9f, 89.9f);
      ImGui::DragFloat("Yaw", &yaw, 0.1f);

      ImGui::Separator();

      ImGui::LabelText("Chunks Loaded", "%d", chunks.size());
      ImGui::LabelText("Chunks Drawn", "%d", chunksDrawn);
      ImGui::LabelText("Triangles Drawn", "%d", triangles);

      ImGui::Separator();
      static float bias = 0.f;
      ImGui::DragFloat("LOD Bias", &bias);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

      atlas->Debug();
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();
  }

private:
  ShaderProgram      mProgram;
  GLuint             mImage;
  dubu::event::Token mResizeToken;

  int width  = 1920;
  int height = 1080;

  constexpr static int                          RenderDistance = 20;
  std::map<ChunkCoords, std::unique_ptr<Chunk>> chunks;
  std::unique_ptr<Atlas>                        atlas;
  BlockDescriptions                             blockDescriptions;

  glm::vec3 cameraPosition{0.f, Chunk::ChunkSize.y, 0.f};
  float     yaw   = 0.f;
  float     pitch = -89.9f;

  DoubleBuffer<std::deque<ChunkCoords>> chunkBuffer;
  std::set<ChunkCoords>                 seen;
};
}  // namespace dubu::block

int main() {
  dubu::block::App app;

  app.Run();

  return 0;
}
