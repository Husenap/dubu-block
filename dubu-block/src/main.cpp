#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>

#include <dubu_event/dubu_event.h>
#include <dubu_log/dubu_log.h>
#include <dubu_opengl_app/dubu_opengl_app.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "game/atlas.hpp"
#include "game/block.hpp"
#include "game/chunk.hpp"
#include "game/chunk_manager.hpp"
#include "generator/seed.hpp"
#include "gl/debug_drawer.hpp"
#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "imgui/dock_space.hpp"
#include "io/io.hpp"
#include "linalg/frustum.hpp"

namespace dubu::block {

class App : public dubu::opengl_app::AppBase {
public:
  App()
      : dubu::opengl_app::AppBase({.appName = "dubu-block"}) {}
  virtual ~App() = default;

protected:
  virtual void Init() override {
    mResizeToken = mWindow->RegisterListener<dubu::window::EventResize>([&](const auto& e) {
      mWidth  = e.width;
      mHeight = e.height;
      DUBU_LOG_DEBUG("Window resized: ({}, {})", mWidth, mHeight);
    });

    mDebugDrawer = std::make_unique<DebugDrawer>();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    VertexShader   vertexShader(dubu::block::ReadFile("assets/shaders/chunk.vert"));
    FragmentShader fragmentShader(dubu::block::ReadFile("assets/shaders/chunk.frag"));
    mChunkProgram.Link(vertexShader, fragmentShader);
    if (const auto err = mChunkProgram.GetError()) {
      DUBU_LOG_ERROR("shader program error: {}", *err);
    }

    mAtlas = std::make_unique<Atlas>(mBlockDescriptions);

    mChunkManager = std::make_unique<ChunkManager>(*mAtlas, mBlockDescriptions, mSeed);

    CalculateChunkIndexTable();
  }

  void CalculateChunkIndexTable() {
    mChunkIndexTable.clear();

    mChunkIndexTable.push_back({0, 0});
    for (int d = 1; d <= mRenderDistance; ++d) {
      for (int x = -d; x <= d; ++x) {
        int z = d;
        mChunkIndexTable.push_back({x, z});
      }
      for (int z = d; z >= -d; --z) {
        int x = d;
        mChunkIndexTable.push_back({x, z});
      }
      for (int x = d; x >= -d; --x) {
        int z = -d;
        mChunkIndexTable.push_back({x, z});
      }
      for (int z = -d; z < d; ++z) {
        int x = -d;
        mChunkIndexTable.push_back({x, z});
      }
    }
  }

  virtual void Update() override {
    mChunkManager->Update(mCameraPosition);

    if (mWidth <= 0 || mHeight <= 0) return;

    DrawDockSpace();

    static float previousTime = static_cast<float>(glfwGetTime());
    const float  time         = static_cast<float>(glfwGetTime());
    // const float  deltaTime    = time - previousTime;
    previousTime = time;

    // mCameraPosition.z += deltaTime * 5.612f;

    glClearColor(mSkyColor.r, mSkyColor.g, mSkyColor.b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = glm::translate(
        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(mPitch), glm::vec3(-1, 0, 0)),
                    glm::radians(mYaw),
                    glm::vec3(0, 1, 0)),
        -mCameraPosition);
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f),
                         static_cast<float>(mWidth) / mHeight,
                         0.1f,
                         static_cast<float>((mRenderDistance + 1) * Chunk::ChunkSize.z));
    const glm::mat4 viewProjection = projection * view;
    const glm::mat4 model          = glm::mat4(1.0f);

    int triangles    = 0;
    int chunksDrawn  = 0;
    int chunksCulled = 0;

    Frustum frustum(glm::inverse(viewProjection));

    mAtlas->Bind(GL_TEXTURE0);
    for (const auto& [i, j] : mChunkIndexTable) {
      const int x = static_cast<int>(std::roundf(mCameraPosition.x / Chunk::ChunkSize.x)) + i;
      const int z = static_cast<int>(std::roundf(mCameraPosition.z / Chunk::ChunkSize.z)) + j;

      const float dx = (x + 0.5f) - mCameraPosition.x / (float)(Chunk::ChunkSize.x);
      const float dz = (z + 0.5f) - mCameraPosition.z / (float)(Chunk::ChunkSize.z);
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

        mChunkProgram.Use();
        glUniformMatrix4fv(
            mChunkProgram.GetUniformLocation("MODELVIEWPROJ"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(
            mChunkProgram.GetUniformLocation("MODEL"), 1, GL_FALSE, glm::value_ptr(chunkModel));
        glUniformMatrix4fv(mChunkProgram.GetUniformLocation("MODELVIEW"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(view * chunkModel));
        glUniformMatrix4fv(
            mChunkProgram.GetUniformLocation("PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(mChunkProgram.GetUniformLocation("SKYCOLOR"), 1, glm::value_ptr(mSkyColor));
        glUniform1f(mChunkProgram.GetUniformLocation("RENDER_DISTANCE"),
                    static_cast<float>(mRenderDistance * Chunk::ChunkSize.z));
        glUniform2fv(
            mChunkProgram.GetUniformLocation("FOG_CONTROL"), 1, glm::value_ptr(mFogControl));

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
      if (ImGui::CollapsingHeader("Render Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        static bool wireframe = false;
        if (ImGui::Checkbox("Wireframe", &wireframe)) {
          glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        ImGui::ColorEdit3("Sky Color", glm::value_ptr(mSkyColor));
        ImGui::DragFloat2("Fog Control", glm::value_ptr(mFogControl));
        if (ImGui::DragInt("Render Distance", &mRenderDistance, 1, 5, 35))
          CalculateChunkIndexTable();
      }

      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Camera Position", glm::value_ptr(mCameraPosition));
        ImGui::DragFloat("Pitch", &mPitch, 0.1f, -90.0f, 90.0f);
        if (ImGui::DragFloat("Yaw", &mYaw, 0.1f)) {
          while (mYaw < 0) mYaw += 360;
          while (mYaw > 360) mYaw -= 360;
        }
      }

      if (ImGui::CollapsingHeader("Chunks")) {
        mChunkManager->Debug();
        ImGui::LabelText("Chunks Drawn", "%d", chunksDrawn);
        ImGui::LabelText("Chunks Culled", "%d", chunksCulled);
        ImGui::LabelText("Triangles Drawn", "%d", triangles);
      }

      if (ImGui::CollapsingHeader("Textures")) {
        static float bias = 0.f;
        ImGui::DragFloat("LOD Bias", &bias);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

        mAtlas->Debug();
      }

      if (ImGui::CollapsingHeader("Generator", ImGuiTreeNodeFlags_DefaultOpen)) {
        mSeed.Debug();
      }
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();
  }

private:
  dubu::event::Token mResizeToken;

  int mWidth  = 1920;
  int mHeight = 1080;

  int mRenderDistance = 10;

  ShaderProgram                    mChunkProgram;
  std::vector<std::pair<int, int>> mChunkIndexTable;

  std::unique_ptr<ChunkManager> mChunkManager;

  std::unique_ptr<Atlas> mAtlas;
  BlockDescriptions      mBlockDescriptions;

  glm::vec3 mCameraPosition{-8, 167, 25};
  float     mPitch = -22;
  float     mYaw   = -2.7f;

  glm::vec3 mSkyColor{0.45f, 0.76f, 1.0f};
  glm::vec2 mFogControl{2.f, 250000.f};

  std::unique_ptr<DebugDrawer> mDebugDrawer;

  Seed mSeed{1337};
};
}  // namespace dubu::block

int main() {
  dubu::block::App app;

  app.Run();

  return 0;
}
