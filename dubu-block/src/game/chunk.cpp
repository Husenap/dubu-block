#include "chunk.hpp"

#include <dubu_log/dubu_log.h>
#include <glm/gtc/random.hpp>
#include <imgui.h>

#include "chunk_manager.hpp"
#include "io/io.hpp"
#include "perlin_noise.hpp"
#include "util/timer.hpp"

namespace dubu::block {

Chunk::Chunk(const ChunkCoords        chunkCoords,
             const ChunkManager&      chunkManager,
             Atlas&                   atlas,
             const BlockDescriptions& blockDescriptions)
    : mChunkCoords(chunkCoords)
    , mChunkBlockOffset({chunkCoords.x * Chunk::ChunkSize.x, chunkCoords.z * Chunk::ChunkSize.z})
    , mMesh({.usage = GL_DYNAMIC_DRAW})
    , mChunkManager(chunkManager)
    , mAtlas(atlas)
    , mBlockDescriptions(blockDescriptions) {
  blocks.fill(BlockType::Empty);

  for (int x = 0; x < ChunkSize.x; ++x) {
    for (int z = 0; z < ChunkSize.z; ++z) {
      blocks[CoordsToIndex({x, 0, z})] = BlockType::Bedrock;

      const glm::vec2 blockCoords{mChunkBlockOffset.x + x, mChunkBlockOffset.z + z};

      const int height = static_cast<int>(
          128 + std::max(noise::fbm(blockCoords * 0.05f) * 8.0f,
                         std::powf(1.0f - std::powf(noise::fbm(blockCoords * 0.04f + 100.f), 2.0f),
                                   5.0f) *
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

int Chunk::Draw() const {
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

          const auto otherBlockId = GetBlockIdAtLocalCoords(otherCoord);
          if (otherBlockId != BlockType::Empty) {
            auto& otherBlockDescription = mBlockDescriptions.GetBlockDescription(otherBlockId);
            if (otherBlockDescription.IsOpaque()) continue;
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

BlockId Chunk::GetBlockIdAtWorldCoords(glm::ivec3 coords) const {
  return GetBlockIdAtLocalCoords(
      {coords.x - mChunkBlockOffset.x, coords.y, coords.z - mChunkBlockOffset.z});
}

BlockId Chunk::GetBlockIdAtLocalCoords(glm::ivec3 coords) const {
  if (AreCoordsBounded(coords)) {
    return blocks[CoordsToIndex(coords)];
  }
  if (coords.y < 0 || coords.y > ChunkSize.y) return BlockType::Empty;
  return mChunkManager.GetBlockIdAt(
      {coords.x + mChunkBlockOffset.x, coords.y, coords.z + mChunkBlockOffset.z});
}

}  // namespace dubu::block