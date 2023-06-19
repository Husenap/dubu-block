#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

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
#include "game/chunk_manager.hpp"
#include "gl/debug_drawer.hpp"
#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "io/io.hpp"
#include "linalg/frustum.hpp"

namespace dubu::block {

class App : public dubu::opengl_app::AppBase {
public:
  App()
      : dubu::opengl_app::AppBase({
            .appName = "dubu-block",
        }) {}
  virtual ~App() = default;

protected:
  virtual void Init() override {
    mResizeToken = mWindow->RegisterListener<dubu::window::EventResize>([&](const auto& e) {
      width  = e.width;
      height = e.height;
      DUBU_LOG_DEBUG("Window resized: ({}, {})", width, height);
    });

    mDebugDrawer = std::make_unique<DebugDrawer>();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    VertexShader   vertexShader(dubu::block::ReadFile("assets/shaders/chunk.vert"));
    FragmentShader fragmentShader(dubu::block::ReadFile("assets/shaders/chunk.frag"));
    chunkProgram.Link(vertexShader, fragmentShader);
    if (const auto err = chunkProgram.GetError()) {
      DUBU_LOG_ERROR("shader program error: {}", *err);
    }

    atlas = std::make_unique<Atlas>(blockDescriptions);

    mChunkManager = std::make_unique<ChunkManager>(*atlas, blockDescriptions);

    CalculateChunkIndexTable();
  }

  void CalculateChunkIndexTable() {
    chunkIndexTable.clear();

    chunkIndexTable.push_back({0, 0});
    for (int d = 1; d <= mRenderDistance; ++d) {
      for (int x = -d; x <= d; ++x) {
        int z = d;
        chunkIndexTable.push_back({x, z});
      }
      for (int z = d; z >= -d; --z) {
        int x = d;
        chunkIndexTable.push_back({x, z});
      }
      for (int x = d; x >= -d; --x) {
        int z = -d;
        chunkIndexTable.push_back({x, z});
      }
      for (int z = -d; z < d; ++z) {
        int x = -d;
        chunkIndexTable.push_back({x, z});
      }
    }
  }

  virtual void Update() override {
    mChunkManager->Update(cameraPosition);

    if (width <= 0 || height <= 0) return;

    DrawDockSpace();

    static float previousTime = static_cast<float>(glfwGetTime());
    const float  time         = static_cast<float>(glfwGetTime());
    const float  deltaTime    = time - previousTime;
    previousTime              = time;

    cameraPosition.z += deltaTime * 5.612f;

    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 cameraLookAt =
        cameraPosition + glm::vec3{-std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
                                   std::sin(glm::radians(pitch)),
                                   std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch))};
    const glm::mat4 view = glm::lookAt(cameraPosition, cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f),
                         static_cast<float>(width) / height,
                         0.1f,
                         static_cast<float>((mRenderDistance + 1) * Chunk::ChunkSize.z));
    const glm::mat4 viewProjection = projection * view;
    const glm::mat4 model          = glm::mat4(1.0f);

    int triangles    = 0;
    int chunksDrawn  = 0;
    int chunksCulled = 0;

    Frustum frustum(glm::inverse(viewProjection));

    atlas->Bind(GL_TEXTURE0);
    for (const auto [i, j] : chunkIndexTable) {
      const int x = static_cast<int>(std::roundf(cameraPosition.x / Chunk::ChunkSize.x)) + i;
      const int z = static_cast<int>(std::roundf(cameraPosition.z / Chunk::ChunkSize.z)) + j;

      const float dx = (x + 0.5f) - cameraPosition.x / (float)(Chunk::ChunkSize.x);
      const float dz = (z + 0.5f) - cameraPosition.z / (float)(Chunk::ChunkSize.z);
      const float d2 = dx * dx + dz * dz;
      if (d2 > mRenderDistance * mRenderDistance) continue;

      const AABB aabb{
          {x * Chunk::ChunkSize.x, 0.0f, z * Chunk::ChunkSize.z},
          {(x + 1) * Chunk::ChunkSize.x, Chunk::ChunkSize.y, (z + 1) * Chunk::ChunkSize.z}};

      if (frustum.IsOutside(aabb)) {
        ++chunksCulled;
        continue;
      }

      const ChunkCoords chunkCoords{x, z};
      if (auto chunk = mChunkManager->FindChunk(chunkCoords); chunk) {
        const auto chunkModel =
            glm::translate(model, glm::vec3(x * Chunk::ChunkSize.x, 0, z * Chunk::ChunkSize.z));
        const glm::mat4 mvp = viewProjection * chunkModel;

        chunkProgram.Use();
        glUniformMatrix4fv(
            chunkProgram.GetUniformLocation("MODELVIEWPROJ"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(
            chunkProgram.GetUniformLocation("MODEL"), 1, GL_FALSE, glm::value_ptr(chunkModel));
        glUniformMatrix4fv(chunkProgram.GetUniformLocation("MODELVIEW"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(view * chunkModel));
        glUniformMatrix4fv(
            chunkProgram.GetUniformLocation("PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(chunkProgram.GetUniformLocation("SKYCOLOR"), 1, glm::value_ptr(skyColor));
        glUniform1f(chunkProgram.GetUniformLocation("RENDER_DISTANCE"),
                    static_cast<float>(mRenderDistance * Chunk::ChunkSize.z));
        glUniform2fv(chunkProgram.GetUniformLocation("FOG_CONTROL"), 1, glm::value_ptr(fogControl));

        triangles += chunk->Draw();
        ++chunksDrawn;

        if (!chunk->HasBeenOptimized() && d2 < mRenderDistance * mRenderDistance * 0.25f) {
          mChunkManager->LoadChunk(chunkCoords, ChunkManager::ChunkLoadingPriority::Optimize);
        }
      } else {
        mChunkManager->LoadChunk(chunkCoords, ChunkManager::ChunkLoadingPriority::Generate);
      }
    }

    mDebugDrawer->Draw(viewProjection);

    if (ImGui::Begin("Debug")) {
      static bool wireframe = false;
      if (ImGui::Checkbox("Wireframe", &wireframe)) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
      }
      ImGui::Separator();
      ImGui::ColorEdit3("Sky Color", glm::value_ptr(skyColor));
      ImGui::DragFloat2("Fog Control", glm::value_ptr(fogControl));
      if (ImGui::DragInt("Render Distance", &mRenderDistance, 1, 5, 35)) CalculateChunkIndexTable();
      ImGui::Separator();

      ImGui::DragFloat3("Camera Position", glm::value_ptr(cameraPosition));
      ImGui::DragFloat("Pitch", &pitch, 0.1f, -89.9f, 89.9f);
      ImGui::DragFloat("Yaw", &yaw, 0.1f);

      ImGui::Separator();

      frustum.Debug();

      ImGui::Separator();

      mChunkManager->Debug();
      ImGui::LabelText("Chunks Drawn", "%d", chunksDrawn);
      ImGui::LabelText("Chunks Culled", "%d", chunksCulled);
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
  void DrawDockSpace() {
    static constexpr ImGuiWindowFlags dockSpaceWindowFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0, 0, 0, 0});
    ImGui::Begin("#DOCK_SPACE", nullptr, dockSpaceWindowFlags);

    ImGui::DockSpace(
        ImGui::GetID("DOCK_SPACE_WINDOW"), {0.f, 0.f}, ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);
  }

  dubu::event::Token mResizeToken;

  int width  = 1920;
  int height = 1080;

  int mRenderDistance = 10;

  ShaderProgram                    chunkProgram;
  std::vector<std::pair<int, int>> chunkIndexTable;

  std::unique_ptr<ChunkManager> mChunkManager;

  std::unique_ptr<Atlas> atlas;
  BlockDescriptions      blockDescriptions;

  glm::vec3 cameraPosition{-8, 167, 25};
  float     pitch = -22;
  float     yaw   = -2.7f;

  glm::vec3 skyColor{0.45f, 0.76f, 1.0f};
  glm::vec2 fogControl{2.f, 250000.f};

  std::unique_ptr<DebugDrawer> mDebugDrawer;
};
}  // namespace dubu::block

int main() {
  dubu::block::App app;

  app.Run();

  return 0;
}
