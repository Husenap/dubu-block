#include "chunk.hpp"

#include <algorithm>

#include <dubu_log/dubu_log.h>

#include "chunk_manager.hpp"
#include "game/block.hpp"
#include "io/io.hpp"
#include "util/timer.hpp"

namespace dubu::block {

Chunk::Chunk(const ChunkCoords        chunkCoords,
             ChunkManager&            chunkManager,
             Atlas&                   atlas,
             const BlockDescriptions& blockDescriptions,
             const Seed&              seed,
             float                    creationTime)
    : mChunkCoords(chunkCoords)
    , mChunkBlockOffset({chunkCoords.x * Chunk::ChunkSize.x, chunkCoords.z * Chunk::ChunkSize.z})
    , mMesh({.usage = GL_DYNAMIC_DRAW})
    , mChunkManager(chunkManager)
    , mAtlas(atlas)
    , mBlockDescriptions(blockDescriptions)
    , mCreationTime(creationTime) {
  blocks.fill(BlockType::Empty);

  for (int x = 0; x < ChunkSize.x; ++x) {
    for (int z = 0; z < ChunkSize.z; ++z) {
      blocks[CoordsToIndex({x, 0, z})] = BlockType::Bedrock;

      const glm::vec2 blockCoords{mChunkBlockOffset.x + x, mChunkBlockOffset.z + z};

      const int height =
          std::clamp((int)(100 + seed.Continentalness(blockCoords) * 64), 0, ChunkSize.y - 1);

      for (int y = 1; y <= height; ++y) {
        blocks[CoordsToIndex({x, y, z})] = BlockType::Stone;
      }
      for (int y = height; y <= 127; ++y) {
        blocks[CoordsToIndex({x, y, z})] = BlockType::Water;
      }
    }
  }

  GenerateMesh();
}

int Chunk::Draw() const {
  return mMesh.Draw();
}

void Chunk::GenerateMesh() {
  Timer timer("Chunk::GenerateMesh");

  static std::vector<Mesh::Vertex> vertices;
  static std::vector<unsigned int> indices;
  vertices.clear();
  indices.clear();

  for (std::size_t index = 0; index < blocks.size(); ++index) {
    const auto blockType = blocks[index];

    if (blockType == BlockType::Empty) continue;

    const auto myCoord = IndexToCoords(index);

    for (std::size_t d = 0; d < Directions.size(); ++d) {
      const auto& dir        = Directions[d];
      const auto  otherCoord = myCoord + dir;

      const auto otherBlockType = GetBlockTypeAtLocalCoords(otherCoord);
      if (otherBlockType != BlockType::Empty) {
        auto& otherBlockDescription = mBlockDescriptions.GetBlockDescription(otherBlockType);
        if (otherBlockDescription.IsOpaque()) continue;
      }

      const auto& faceData = DirectionToFace[d];

      const glm::vec3 offsetPosition = myCoord;
      const auto [uvPos0, uvSize0]   = mAtlas.GetUVs(blockType, dir);
      const glm::vec3 color          = mBlockDescriptions.GetBlockDescription(blockType).GetColor();

      static constexpr float aoStrength = 0.2f;

      float ao0 = 1.0f;
      float ao1 = 1.0f;
      float ao2 = 1.0f;
      float ao3 = 1.0f;

      if (otherBlockType != BlockType::Empty) {
        ao0 = ao1 = ao2 = ao3 = 1.0f - 3.0f * aoStrength;
      } else {
        const bool n0 = IsEmpty(myCoord + faceData.aoNeighbours[0]);
        const bool n1 = IsEmpty(myCoord + faceData.aoNeighbours[1]);
        const bool n2 = IsEmpty(myCoord + faceData.aoNeighbours[2]);
        const bool n3 = IsEmpty(myCoord + faceData.aoNeighbours[3]);
        const bool n4 = IsEmpty(myCoord + faceData.aoNeighbours[4]);
        const bool n5 = IsEmpty(myCoord + faceData.aoNeighbours[5]);
        const bool n6 = IsEmpty(myCoord + faceData.aoNeighbours[6]);
        const bool n7 = IsEmpty(myCoord + faceData.aoNeighbours[7]);
        ao0 += (n0 + n1 + n2 - 3) * aoStrength;
        ao1 += (n2 + n3 + n4 - 3) * aoStrength;
        ao2 += (n4 + n5 + n6 - 3) * aoStrength;
        ao3 += (n6 + n7 + n0 - 3) * aoStrength;
      }

      vertices.push_back({.position = faceData.vertices[0] + offsetPosition,
                          .color    = color,
                          .uv0      = uvPos0 + uvSize0 * glm::vec2{0, 1},
                          .ao       = ao0});
      vertices.push_back({.position = faceData.vertices[1] + offsetPosition,
                          .color    = color,
                          .uv0      = uvPos0 + uvSize0 * glm::vec2{1, 1},
                          .ao       = ao1});
      vertices.push_back({.position = faceData.vertices[2] + offsetPosition,
                          .color    = color,
                          .uv0      = uvPos0 + uvSize0 * glm::vec2{1, 0},
                          .ao       = ao2});
      vertices.push_back({.position = faceData.vertices[3] + offsetPosition,
                          .color    = color,
                          .uv0      = uvPos0 + uvSize0 * glm::vec2{0, 0},
                          .ao       = ao3});

      const unsigned int startIndex = static_cast<unsigned int>(vertices.size()) - 4;
      indices.push_back(startIndex + faceData.indices[0]);
      indices.push_back(startIndex + faceData.indices[1]);
      indices.push_back(startIndex + faceData.indices[2]);
      indices.push_back(startIndex + faceData.indices[3]);
      indices.push_back(startIndex + faceData.indices[4]);
      indices.push_back(startIndex + faceData.indices[5]);
    }
  }
  mMesh.UpdateMesh(vertices, indices);
}

BlockType Chunk::GetBlockTypeAtWorldCoords(glm::ivec3 coords) const {
  return GetBlockTypeAtLocalCoords(
      {coords.x - mChunkBlockOffset.x, coords.y, coords.z - mChunkBlockOffset.z});
}

BlockType Chunk::GetBlockTypeAtLocalCoords(glm::ivec3 coords) const {
  if (AreCoordsBounded(coords)) {
    return blocks[CoordsToIndex(coords)];
  }
  if (coords.y < 0 || coords.y >= ChunkSize.y) return BlockType::Empty;
  return mChunkManager.GetBlockTypeAt(
      {coords.x + mChunkBlockOffset.x, coords.y, coords.z + mChunkBlockOffset.z});
}

void Chunk::SetBlockTypeAtWorldCoords(glm::ivec3 coords, BlockType type) {
  SetBlockTypeAtLocalCoords(
      {coords.x - mChunkBlockOffset.x, coords.y, coords.z - mChunkBlockOffset.z}, type);
}

void Chunk::SetBlockTypeAtLocalCoords(glm::ivec3 coords, BlockType type) {
  if (AreCoordsBounded(coords)) {
    blocks[CoordsToIndex(coords)] = type;
  }
}

}  // namespace dubu::block
