#include "chunk.hpp"

#include <dubu_log/dubu_log.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "gl/shader.hpp"
#include "io/io.hpp"
#include "util/timer.hpp"

namespace dubu::block {

Chunk::Chunk(Atlas& atlas)
    : mMesh({.usage = GL_DYNAMIC_DRAW})
    , mAtlas(atlas) {
  for (int i = 0; i < blocks.size(); ++i) {
    if (IndexToCoords(i).y < 64) {
      blocks[i] = rand() % 2;
    } else if (IndexToCoords(i).y < 128) {
      blocks[i] = (rand() % 2) * 2;
    } else {
      blocks[i] = 0;
    }
  }

  GenerateMesh();

  VertexShader   vertexShader(dubu::block::read_file("assets/shaders/chunk.vert"));
  FragmentShader fragmentShader(dubu::block::read_file("assets/shaders/chunk.frag"));
  mProgram.Link(vertexShader, fragmentShader);
  if (const auto err = mProgram.GetError()) DUBU_LOG_ERROR("shader program error: {}", *err);
}

void Chunk::Draw(const glm::mat4& mvp) {
  mProgram.Use();
  glUniformMatrix4fv(mProgram.GetUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
  mMesh.Draw();
}

void Chunk::Debug() {
  bool edited = false;
  for (int y = ChunkSize.y - 1; y >= 0; --y) {
    ImGui::Text("Layer: %d", y);
    ImGui::PushID(y);
    if (ImGui::BeginTable("ChunkTable", ChunkSize.x)) {
      for (int z = 0; z < ChunkSize.z; ++z) {
        ImGui::PushID(z);
        ImGui::TableNextRow();
        for (int x = 0; x < ChunkSize.x; ++x) {
          ImGui::TableSetColumnIndex(x);
          ImGui::PushID(x);
          edited |= ImGui::Checkbox("", (bool*)&blocks[CoordsToIndex({x, y, z})]);
          ImGui::PopID();
        }
        ImGui::PopID();
      }
      ImGui::EndTable();
    }
    ImGui::PopID();
  }
  if (edited) GenerateMesh();
}

void Chunk::GenerateMesh() {
  Timer timer("Chunk::GenerateMesh");

  std::vector<glm::vec3>    vertices;
  std::vector<glm::vec2>    uvs;
  std::vector<unsigned int> indices;
  for (int z = 0; z < ChunkSize.z; ++z) {
    for (int y = 0; y < ChunkSize.y; ++y) {
      for (int x = 0; x < ChunkSize.x; ++x) {
        const glm::ivec3 myCoord{x, y, z};
        const int        index = CoordsToIndex(myCoord);

        const auto blockId = blocks[index];

        if (blockId == 0) continue;

        for (int d = 0; d < Directions.size(); ++d) {
          const auto otherCoord = myCoord + Directions[d];

          if (areCoordsBounded(otherCoord) && blocks[CoordsToIndex(otherCoord)] != 0) continue;

          const auto& faceData = DirectionToFace[d];

          const glm::vec3 offsetPosition = myCoord;
          vertices.push_back(faceData.vertices[0] + offsetPosition);
          vertices.push_back(faceData.vertices[1] + offsetPosition);
          vertices.push_back(faceData.vertices[2] + offsetPosition);
          vertices.push_back(faceData.vertices[3] + offsetPosition);

          const auto [uvPos, uvSize] = mAtlas.GetUVs(blockId);
          uvs.push_back(uvPos + uvSize * glm::vec2{0, 1});
          uvs.push_back(uvPos + uvSize * glm::vec2{1, 1});
          uvs.push_back(uvPos + uvSize * glm::vec2{1, 0});
          uvs.push_back(uvPos + uvSize * glm::vec2{0, 0});

          const unsigned int startIndex = static_cast<unsigned int>(vertices.size()) - 4;
          indices.push_back(startIndex + faceData.indices[0]);
          indices.push_back(startIndex + faceData.indices[1]);
          indices.push_back(startIndex + faceData.indices[2]);
          indices.push_back(startIndex + faceData.indices[3]);
          indices.push_back(startIndex + faceData.indices[4]);
          indices.push_back(startIndex + faceData.indices[5]);
        }
      }
    }
  }
  assert(vertices.size() == uvs.size());
  mMesh.SetVertices(vertices);
  mMesh.SetUVs(uvs);
  mMesh.UpdateMesh();
  mMesh.SetIndices(indices);
  DUBU_LOG_DEBUG("NumTriangles in chunk: {}", indices.size() / 3);
}

}  // namespace dubu::block