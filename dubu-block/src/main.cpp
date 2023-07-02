#include <memory>

#include <dubu_event/dubu_event.h>
#include <dubu_log/dubu_log.h>
#include <dubu_opengl_app/dubu_opengl_app.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "camera/freefly_camera.hpp"
#include "dubu_log/logger/Logger.h"
#include "game/atlas.hpp"
#include "game/block.hpp"
#include "game/chunk_manager.hpp"
#include "generator/seed.hpp"
#include "gl/debug_drawer.hpp"
#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "imgui/dock_space.hpp"
#include "input/input.hpp"
#include "io/io.hpp"
#include "linalg/frustum.hpp"
#include "linalg/ray.hpp"

namespace dubu::block {

class App : public dubu::opengl_app::AppBase, public dubu::event::EventSubscriber {
public:
  App()
      : dubu::opengl_app::AppBase({.appName = "dubu-block"}) {}
  virtual ~App() = default;

protected:
  virtual void Init() override {
    Subscribe<dubu::window::EventResize>(
        [&](const auto& e) {
          mWidth  = e.width;
          mHeight = e.height;
        },
        *mWindow);

    Input::Get().Init(*mWindow);
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
    static float previousTime = static_cast<float>(glfwGetTime());
    const float  time         = static_cast<float>(glfwGetTime());
    const float  deltaTime    = time - previousTime;
    previousTime              = time;

    mChunkManager->Update(camera.GetPosition(), time);

    Input::Update();
    camera.Update(deltaTime);

    if (mWidth <= 0 || mHeight <= 0) return;

    DrawDockSpace();

    glClearColor(mSkyColor.r, mSkyColor.g, mSkyColor.b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 projection =
        glm::perspective(glm::radians(60.0f),
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
      const auto& cameraPosition = camera.GetPosition();
      const int   x = static_cast<int>(std::roundf(cameraPosition.x / Chunk::ChunkSize.x)) + i;
      const int   z = static_cast<int>(std::roundf(cameraPosition.z / Chunk::ChunkSize.z)) + j;

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

        glUniform1f(mChunkProgram.GetUniformLocation("AGE"), time - chunk->GetCreationTime());

        triangles += chunk->Draw();
        ++chunksDrawn;

        if (!chunk->HasBeenOptimized() && d2 < mRenderDistance * mRenderDistance * 0.25f) {
          mChunkManager->LoadChunk(chunkCoords, ChunkManager::ChunkLoadingPriority::Optimize);
        }
      } else {
        mChunkManager->LoadChunk(chunkCoords, ChunkManager::ChunkLoadingPriority::Generate);
      }

      if (mDebugDrawChunkBorders) {
        mDebugDrawer->DrawAABB(
            {mChunkManager->ChunkCoordsToBlockCoords(chunkCoords),
             mChunkManager->ChunkCoordsToBlockCoords(chunkCoords) + glm::ivec3(Chunk::ChunkSize)},
            {1, 1, 0});
      }
    }

    // Raycast from camera and check if we hit anything
    RaycastHit raycastHit;
    auto       hitBlockType = BlockType::Empty;
    bool       didHit       = false;

    Raycast({.start = camera.GetPosition(), .direction = -camera.GetForward(), .length = 10.0f},
            [&](RaycastHit hit) {
              hitBlockType = mChunkManager->GetBlockTypeAt(hit.coords);
              if (hitBlockType != BlockType::Empty) {
                raycastHit = hit;
                didHit     = true;
              }
              return didHit;
            });

    mDebugDrawer->Draw(viewProjection);

    static bool placedOrRemovedBlock = false;
    if (didHit) {
      if (!placedOrRemovedBlock && Input::IsGamepadButtonDown(0, dubu::window::GamepadButtonA)) {
        mChunkManager->SetBlockTypeAt(raycastHit.coords + raycastHit.face, BlockType::Dirt);
        mChunkManager->UpdateSurroundingChunks(
            mChunkManager->BlockCoordsToChunkCoords(raycastHit.coords));
      } else if (!placedOrRemovedBlock &&
                 Input::IsGamepadButtonDown(0, dubu::window::GamepadButtonB)) {
        mChunkManager->SetBlockTypeAt(raycastHit.coords, BlockType::Empty);
        mChunkManager->UpdateSurroundingChunks(
            mChunkManager->BlockCoordsToChunkCoords(raycastHit.coords));
      }
      placedOrRemovedBlock = Input::IsGamepadButtonDown(0, dubu::window::GamepadButtonA) ||
                             Input::IsGamepadButtonDown(0, dubu::window::GamepadButtonB);

      mDebugDrawer->DrawAABB(
          {.min = raycastHit.coords, .max = raycastHit.coords + glm::ivec3(1, 1, 1)},
          {1.0f, 0.0f, 0.0f});

      mDebugDrawer->DrawLine(
          glm::vec3(raycastHit.coords) + glm::vec3(0.5f),
          glm::vec3(raycastHit.coords) + glm::vec3(0.5f) + glm::vec3(raycastHit.face),
          {0.0f, 1.0f, 0.0f});
    }

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

      camera.Debug();

      if (ImGui::CollapsingHeader("Chunks")) {
        mChunkManager->Debug();
        ImGui::LabelText("Chunks Drawn", "%d", chunksDrawn);
        ImGui::LabelText("Chunks Culled", "%d", chunksCulled);
        ImGui::LabelText("Triangles Drawn", "%d", triangles);
        ImGui::Checkbox("Draw chunk borders", &mDebugDrawChunkBorders);
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

      if (ImGui::CollapsingHeader("Raycasting")) {
        ImGui::LabelText("Hit block type", "%hhu", hitBlockType);
        ImGui::LabelText("Hit distance", "%f", raycastHit.distance);
        ImGui::LabelText("Hit face X", "%d", raycastHit.face.x);
        ImGui::LabelText("Hit face Y", "%d", raycastHit.face.y);
        ImGui::LabelText("Hit face Z", "%d", raycastHit.face.z);
      }
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();
  }

private:
  int mWidth  = 1920;
  int mHeight = 1080;

  int mRenderDistance = 10;

  ShaderProgram                    mChunkProgram;
  std::vector<std::pair<int, int>> mChunkIndexTable;

  std::unique_ptr<ChunkManager> mChunkManager;

  std::unique_ptr<Atlas> mAtlas;
  BlockDescriptions      mBlockDescriptions;

  glm::vec3 mSkyColor{0.45f, 0.76f, 1.0f};
  glm::vec2 mFogControl{2.f, 250000.f};

  std::unique_ptr<DebugDrawer> mDebugDrawer;

  Seed mSeed{1337};

  FreeflyCamera camera;

  bool mDebugDrawChunkBorders = false;
};
}  // namespace dubu::block

int main() {
  dubu::block::App app;

  app.Run();

  return 0;
}
