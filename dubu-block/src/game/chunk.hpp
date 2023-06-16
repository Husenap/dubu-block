#pragma once

#include <array>

#include <glm/glm.hpp>

#include "game/atlas.hpp"
#include "game/block.hpp"
#include "gl/mesh.hpp"

namespace dubu::block {
using ChunkCoords = std::pair<int, int>;
}

template <>
struct std::hash<dubu::block::ChunkCoords> {
  std::size_t operator()(const dubu::block::ChunkCoords& s) const noexcept {
    return std::hash<long long>{}(static_cast<long long>(s.first) << 32 | s.second);
  }
};

namespace dubu::block {

class Chunk {
public:
  static constexpr glm::ivec3 ChunkSize{16, 384, 16};

  Chunk(const ChunkCoords chunkCoords, Atlas& atlas, const BlockDescriptions& blockDescriptions);

  int  Draw();
  void Debug();

private:
  void GenerateMesh();

  inline int CoordsToIndex(glm::ivec3 coords) const {
    assert(AreCoordsBounded(coords));
    return coords.x + coords.y * ChunkSize.x + coords.z * ChunkSize.x * ChunkSize.y;
  }
  inline glm::ivec3 IndexToCoords(int index) const {
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
    return !AreCoordsBounded(coords) || blocks[CoordsToIndex(coords)] == BlockType::Empty;
  }

  struct FaceData {
    std::array<glm::vec3, 4>                 vertices;
    std::array<std::array<glm::ivec3, 3>, 4> aoNeighbours;
    std::array<unsigned int, 6>              indices;
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
         std::array<std::array<glm::ivec3, 3>, 4>{{
           {{{-1, 1, 0}, {-1, 1, -1}, {-1, 0, -1}}},
           {{{-1, 1, 0}, {-1, 1, 1}, {-1, 0, 1}}},
           {{{-1, -1, 0}, {-1, -1, 1}, {-1, 0, 1}}},
           {{{-1, -1, 0}, {-1, -1, -1}, {-1, 0, -1}}},
       }},
         {0, 1, 2, 0, 2, 3}},
      // Right
      {{
           glm::vec3{1.f, 1.f, 1.f},
           glm::vec3{1.f, 1.f, 0.f},
           glm::vec3{1.f, 0.f, 0.f},
           glm::vec3{1.f, 0.f, 1.f},
       },
         std::array<std::array<glm::ivec3, 3>, 4>{{
           {{{1, 1, 0}, {1, 1, 1}, {1, 0, 1}}},
           {{{1, 1, 0}, {1, 1, -1}, {1, 0, -1}}},
           {{{1, -1, 0}, {1, -1, -1}, {1, 0, -1}}},
           {{{1, -1, 0}, {1, -1, 1}, {1, 0, 1}}},
       }},
         {0, 1, 2, 0, 2, 3}},
      // Down
      {{
           glm::vec3{0.f, 0.f, 1.f},
           glm::vec3{1.f, 0.f, 1.f},
           glm::vec3{1.f, 0.f, 0.f},
           glm::vec3{0.f, 0.f, 0.f},
       },
         std::array<std::array<glm::ivec3, 3>, 4>{{
           {{{-1, -1, 0}, {-1, -1, 1}, {0, -1, 1}}},
           {{{1, -1, 0}, {1, -1, 1}, {0, -1, 1}}},
           {{{1, -1, 0}, {1, -1, -1}, {0, -1, -1}}},
           {{{-1, -1, 0}, {-1, -1, -1}, {0, -1, -1}}},
       }},
         {0, 1, 2, 0, 2, 3}},
      // Up
      {{
           glm::vec3{0.f, 1.f, 0.f},
           glm::vec3{1.f, 1.f, 0.f},
           glm::vec3{1.f, 1.f, 1.f},
           glm::vec3{0.f, 1.f, 1.f},
       },
         std::array<std::array<glm::ivec3, 3>, 4>{{
           {{{-1, 1, 0}, {-1, 1, -1}, {0, 1, -1}}},
           {{{1, 1, 0}, {1, 1, -1}, {0, 1, -1}}},
           {{{1, 1, 0}, {1, 1, 1}, {0, 1, 1}}},
           {{{-1, 1, 0}, {-1, 1, 1}, {0, 1, 1}}},
       }},
         {0, 1, 2, 0, 2, 3}},
      // Back
      {{
           glm::vec3{1.f, 1.f, 0.f},
           glm::vec3{0.f, 1.f, 0.f},
           glm::vec3{0.f, 0.f, 0.f},
           glm::vec3{1.f, 0.f, 0.f},
       },
         std::array<std::array<glm::ivec3, 3>, 4>{{
           {{{0, 1, -1}, {1, 1, -1}, {1, 0, -1}}},
           {{{0, 1, -1}, {-1, 1, -1}, {-1, 0, -1}}},
           {{{-1, 0, -1}, {-1, -1, -1}, {0, -1, -1}}},
           {{{1, 0, -1}, {1, -1, -1}, {0, -1, -1}}},
       }},
         {0, 1, 2, 0, 2, 3}},
      // Front
      {{
           glm::vec3{0.f, 1.f, 1.f},
           glm::vec3{1.f, 1.f, 1.f},
           glm::vec3{1.f, 0.f, 1.f},
           glm::vec3{0.f, 0.f, 1.f},
       },
         std::array<std::array<glm::ivec3, 3>, 4>{{
           {{{0, 1, 1}, {-1, 1, 1}, {-1, 0, 1}}},
           {{{0, 1, 1}, {1, 1, 1}, {1, 0, 1}}},
           {{{1, 0, 1}, {1, -1, 1}, {0, -1, 1}}},
           {{{-1, 0, 1}, {-1, -1, 1}, {0, -1, 1}}},
       }},
         {0, 1, 2, 0, 2, 3}},
  }};

  std::array<BlockId, ChunkSize.x * ChunkSize.y * ChunkSize.z> blocks;
  const BlockDescriptions&                                     mBlockDescriptions;

  Mesh   mMesh;
  Atlas& mAtlas;
};
}  // namespace dubu::block