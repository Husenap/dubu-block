#include "chunk.hpp"

#include <dubu_log/dubu_log.h>
#include <glm/gtc/random.hpp>
#include <imgui.h>

#include "io/io.hpp"
#include "util/timer.hpp"

namespace dubu::block {

float random(glm::vec2 st) {
  return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
}
float noise(glm::vec2 st) {
  const glm::vec2 i = floor(st);
  const glm::vec2 f = fract(st);

  // Four corners in 2D of a tile
  const float a = random(i);
  const float b = random(i + glm::vec2(1.0f, 0.0f));
  const float c = random(i + glm::vec2(0.0f, 1.0f));
  const float d = random(i + glm::vec2(1.0f, 1.0f));

  const glm::vec2 u = f * f * (3.0f - 2.0f * f);

  return glm::mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm(glm::vec2 st) {
  // Initial values
  float value     = 0.0;
  float amplitude = .5;
  //
  // Loop of octaves
  for (int i = 0; i < OCTAVES; i++) {
    value += amplitude * noise(st);
    st *= 2.f;
    amplitude *= .5f;
  }
  return value;
}

Chunk::Chunk(const ChunkCoords        chunkCoords,
             Atlas&                   atlas,
             const BlockDescriptions& blockDescriptions)
    : mMesh({.usage = GL_DYNAMIC_DRAW})
    , mAtlas(atlas)
    , mBlockDescriptions(blockDescriptions) {
  const glm::vec2 chunkBlockCoords = {chunkCoords.first * static_cast<float>(Chunk::ChunkSize.x),
                                      chunkCoords.second * static_cast<float>(Chunk::ChunkSize.z)};

  blocks.fill(BlockType::Empty);

  for (int x = 0; x < ChunkSize.x; ++x) {
    for (int z = 0; z < ChunkSize.z; ++z) {
      blocks[CoordsToIndex({x, 0, z})] = BlockType::Bedrock;

      const glm::vec2 blockCoords = {chunkBlockCoords.x + x, chunkBlockCoords.y + z};

      const int height = static_cast<int>(
          128 + std::max(fbm(blockCoords * 0.05f) * 8.0f,
                         std::powf(1.0f - std::powf(fbm(blockCoords * 0.04f + 100.f), 2.0f), 5.0f) *
                             20.0f));

      for (int y = 1; y <= height; ++y) {
        if (y < 64) {
          blocks[CoordsToIndex({x, y, z})] = BlockType::Stone;
        } else if (y < height) {
          blocks[CoordsToIndex({x, y, z})] = BlockType::Dirt;
        } else {
          blocks[CoordsToIndex({x, y, z})] = BlockType::Grass;
        }
      }
    }
  }

  GenerateMesh();
}

int Chunk::Draw() {
  return mMesh.Draw();
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

  static std::vector<Mesh::Vertex> vertices;
  static std::vector<unsigned int> indices;
  vertices.clear();
  indices.clear();

  for (int z = 0; z < ChunkSize.z; ++z) {
    for (int y = 0; y < ChunkSize.y; ++y) {
      for (int x = 0; x < ChunkSize.x; ++x) {
        const glm::ivec3 myCoord{x, y, z};
        const int        index = CoordsToIndex(myCoord);

        const auto blockId = blocks[index];

        if (blockId == BlockType::Empty) continue;

        for (int d = 0; d < Directions.size(); ++d) {
          const auto& dir        = Directions[d];
          const auto  otherCoord = myCoord + dir;

          if (AreCoordsBounded(otherCoord)) {
            const auto otherBlockId = blocks[CoordsToIndex(otherCoord)];
            if (otherBlockId != BlockType::Empty) {
              auto& otherBlockDescription = mBlockDescriptions.GetBlockDescription(otherBlockId);
              if (otherBlockDescription.IsOpaque()) continue;
            }
          }

          const auto& faceData = DirectionToFace[d];

          const glm::vec3 offsetPosition = myCoord;
          const auto [uvPos0, uvSize0]   = mAtlas.GetUVs(blockId, dir);
          const glm::vec3        color = mBlockDescriptions.GetBlockDescription(blockId).GetColor();
          static constexpr float ao    = 0.25f;

          vertices.push_back(
              {faceData.vertices[0] + offsetPosition,
               color * (1.0f + std::max(-2,
                                        (-3 + IsEmpty(myCoord + faceData.aoNeighbours[0][0]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[0][1]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[0][2]))) *
                                   ao),
               uvPos0 + uvSize0 * glm::vec2{0, 1}});
          vertices.push_back(
              {faceData.vertices[1] + offsetPosition,
               color * (1.0f + std::max(-2,
                                        (-3 + IsEmpty(myCoord + faceData.aoNeighbours[1][0]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[1][1]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[1][2]))) *
                                   ao),
               uvPos0 + uvSize0 * glm::vec2{1, 1}});
          vertices.push_back(
              {faceData.vertices[2] + offsetPosition,
               color * (1.0f + std::max(-2,
                                        (-3 + IsEmpty(myCoord + faceData.aoNeighbours[2][0]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[2][1]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[2][2]))) *
                                   ao),
               uvPos0 + uvSize0 * glm::vec2{1, 0}});
          vertices.push_back(
              {faceData.vertices[3] + offsetPosition,
               color * (1.0f + std::max(-2,
                                        (-3 + IsEmpty(myCoord + faceData.aoNeighbours[3][0]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[3][1]) +
                                         IsEmpty(myCoord + faceData.aoNeighbours[3][2]))) *
                                   ao),
               uvPos0 + uvSize0 * glm::vec2{0, 0}});

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
  mMesh.UpdateMesh(vertices, indices);
}

}  // namespace dubu::block