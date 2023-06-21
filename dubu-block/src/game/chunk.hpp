#pragma once

#include <array>

#include <glm/glm.hpp>

#include "game/atlas.hpp"
#include "game/block.hpp"
#include "generator/seed.hpp"
#include "gl/mesh.hpp"

namespace dubu::block {
struct ChunkCoords {
  int            x;
  int            z;
  constexpr bool operator==(const ChunkCoords& rhs) const { return x == rhs.x && z == rhs.z; }
};
}  // namespace dubu::block

template <>
struct std::hash<dubu::block::ChunkCoords> {
  std::size_t operator()(const dubu::block::ChunkCoords& s) const noexcept {
    return std::hash<int64_t>{}(static_cast<int64_t>(s.x) << 32 | s.z);
  }
};

namespace dubu::block {

class ChunkManager;

class Chunk {
public:
  static constexpr glm::ivec3 ChunkSize{16, 384, 16};

  Chunk(const ChunkCoords        chunkCoords,
        const ChunkManager&      chunkManager,
        Atlas&                   atlas,
        const BlockDescriptions& blockDescriptions,
        const Seed&              seed);

  int Draw() const;

  void Optimize() {
    mHasBeenOptimized = true;
    GenerateMesh();
  }
  bool HasBeenOptimized() const { return mHasBeenOptimized; }

  BlockId GetBlockIdAtWorldCoords(glm::ivec3 coords) const;

private:
  void GenerateMesh();

  BlockId GetBlockIdAtLocalCoords(glm::ivec3 coords) const;

  inline std::size_t CoordsToIndex(glm::ivec3 coords) const {
    assert(AreCoordsBounded(coords));
    return coords.x + coords.y * ChunkSize.x + coords.z * ChunkSize.x * ChunkSize.y;
  }
  inline glm::ivec3 IndexToCoords(std::size_t index) const {
    assert(index >= 0 && index < blocks.size());
    return {index % ChunkSize.x,
            (index / ChunkSize.x) % ChunkSize.y,
            (index / (ChunkSize.x * ChunkSize.y))};
  }
  inline bool AreCoordsBounded(glm::ivec3 coords) const {
    return coords.x >= 0 && coords.x < ChunkSize.x && coords.y >= 0 && coords.y < ChunkSize.y &&
           coords.z >= 0 && coords.z < ChunkSize.z;
  }
  inline bool IsEmpty(glm::ivec3 coords) const {
    return GetBlockIdAtLocalCoords(coords) == BlockType::Empty;
  }

  struct FaceData {
    std::array<glm::vec3, 4>    vertices;
    std::array<glm::ivec3, 8>   aoNeighbours;
    std::array<unsigned int, 6> indices;
  };

  static constexpr std::array<glm::ivec3, 6> Directions{{
      {-1, 0, 0},  // Left
      {1, 0, 0},   // Right
      {0, -1, 0},  // Down
      {0, 1, 0},   // Up
      {0, 0, -1},  // Back
      {0, 0, 1},   // Front
  }};
  static constexpr std::array<FaceData, 6>   DirectionToFace{{
      // Left
      {{
           glm::vec3{0.f, 1.f, 0.f},
           glm::vec3{0.f, 1.f, 1.f},
           glm::vec3{0.f, 0.f, 1.f},
           glm::vec3{0.f, 0.f, 0.f},
       },
         {{{-1, 0, -1},
           {-1, 1, -1},
           {-1, 1, 0},
           {-1, 1, 1},
           {-1, 0, 1},
           {-1, -1, 1},
           {-1, -1, 0},
           {-1, -1, -1}}},
         {0, 1, 2, 0, 2, 3}},
      // Right
      {{
           glm::vec3{1.f, 1.f, 1.f},
           glm::vec3{1.f, 1.f, 0.f},
           glm::vec3{1.f, 0.f, 0.f},
           glm::vec3{1.f, 0.f, 1.f},
       },
         {{{1, 0, 1},
           {1, 1, 1},
           {1, 1, 0},
           {1, 1, -1},
           {1, 0, -1},
           {1, -1, -1},
           {1, -1, 0},
           {1, -1, 1}}},
         {0, 1, 2, 0, 2, 3}},
      // Down
      {{
           glm::vec3{0.f, 0.f, 1.f},
           glm::vec3{1.f, 0.f, 1.f},
           glm::vec3{1.f, 0.f, 0.f},
           glm::vec3{0.f, 0.f, 0.f},
       },
         {{{-1, -1, 0},
           {-1, -1, 1},
           {0, -1, 1},
           {1, -1, 1},
           {1, -1, 0},
           {1, -1, -1},
           {0, -1, -1},
           {-1, -1, -1}}},
         {0, 1, 2, 0, 2, 3}},
      // Up
      {{
           glm::vec3{0.f, 1.f, 0.f},
           glm::vec3{1.f, 1.f, 0.f},
           glm::vec3{1.f, 1.f, 1.f},
           glm::vec3{0.f, 1.f, 1.f},
       },
         {{{-1, 1, 0},
           {-1, 1, -1},
           {0, 1, -1},
           {1, 1, -1},
           {1, 1, 0},
           {1, 1, 1},
           {0, 1, 1},
           {-1, 1, 1}}},
         {0, 1, 2, 0, 2, 3}},
      // Back
      {{
           glm::vec3{1.f, 1.f, 0.f},
           glm::vec3{0.f, 1.f, 0.f},
           glm::vec3{0.f, 0.f, 0.f},
           glm::vec3{1.f, 0.f, 0.f},
       },
         {{{1, 0, -1},
           {1, 1, -1},
           {0, 1, -1},
           {-1, 1, -1},
           {-1, 0, -1},
           {-1, -1, -1},
           {0, -1, -1},
           {1, -1, -1}}},
         {0, 1, 2, 0, 2, 3}},
      // Front
      {{
           glm::vec3{0.f, 1.f, 1.f},
           glm::vec3{1.f, 1.f, 1.f},
           glm::vec3{1.f, 0.f, 1.f},
           glm::vec3{0.f, 0.f, 1.f},
       },
         {{{-1, 0, 1},
           {-1, 1, 1},
           {0, 1, 1},
           {1, 1, 1},
           {1, 0, 1},
           {1, -1, 1},
           {0, -1, 1},
           {-1, -1, 1}}},
         {0, 1, 2, 0, 2, 3}},
  }};

  std::array<BlockId, ChunkSize.x * ChunkSize.y * ChunkSize.z> blocks;

  const ChunkCoords mChunkCoords;
  const ChunkCoords mChunkBlockOffset;

  Mesh mMesh;

  const ChunkManager&      mChunkManager;
  Atlas&                   mAtlas;
  const BlockDescriptions& mBlockDescriptions;

  bool mHasBeenOptimized = false;
};
}  // namespace dubu::block