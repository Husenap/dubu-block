#pragma once

#include <array>

#include <glm/glm.hpp>

#include "game/atlas.hpp"
#include "gl/mesh.hpp"
#include "gl/shader_program.hpp"

namespace dubu::block {
class Chunk {
public:
  Chunk(Atlas& atlas);

  void Draw(const glm::mat4& mvp);
  void Debug();

private:
  void GenerateMesh();

  inline int CoordsToIndex(glm::ivec3 coords) const {
    assert(areCoordsBounded(coords));
    return coords.x + coords.y * ChunkSize.x + coords.z * ChunkSize.x * ChunkSize.y;
  }
  inline glm::ivec3 IndexToCoords(int index) const {
    assert(index >= 0 && index < blocks.size());
    return {index % ChunkSize.x,
            (index / ChunkSize.x) % ChunkSize.y,
            (index / (ChunkSize.x * ChunkSize.y))};
  }
  inline bool areCoordsBounded(glm::ivec3 coords) const {
    return coords.x >= 0 && coords.x < ChunkSize.x && coords.y >= 0 && coords.y < ChunkSize.y &&
           coords.z >= 0 && coords.z < ChunkSize.z;
  }

  struct FaceData {
    std::array<glm::vec3, 4>    vertices;
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
           glm::vec3{-0.5f, 0.5f, -0.5f},
           glm::vec3{-0.5f, 0.5f, 0.5f},
           glm::vec3{-0.5f, -0.5f, 0.5f},
           glm::vec3{-0.5f, -0.5f, -0.5f},
       },
         {0, 1, 2, 0, 2, 3}},
      // Right
      {{
           glm::vec3{0.5f, 0.5f, 0.5f},
           glm::vec3{0.5f, 0.5f, -0.5f},
           glm::vec3{0.5f, -0.5f, -0.5f},
           glm::vec3{0.5f, -0.5f, 0.5f},
       },
         {0, 1, 2, 0, 2, 3}},
      // Down
      {{
           glm::vec3{-0.5f, -0.5f, 0.5f},
           glm::vec3{0.5f, -0.5f, 0.5f},
           glm::vec3{0.5f, -0.5f, -0.5f},
           glm::vec3{-0.5f, -0.5f, -0.5f},
       },
         {0, 1, 2, 0, 2, 3}},
      // Up
      {{
           glm::vec3{-0.5f, 0.5f, -0.5f},
           glm::vec3{0.5f, 0.5f, -0.5f},
           glm::vec3{0.5f, 0.5f, 0.5f},
           glm::vec3{-0.5f, 0.5f, 0.5f},
       },
         {0, 1, 2, 0, 2, 3}},
      // Back
      {{
           glm::vec3{0.5f, 0.5f, -0.5f},
           glm::vec3{-0.5f, 0.5f, -0.5f},
           glm::vec3{-0.5f, -0.5f, -0.5f},
           glm::vec3{0.5f, -0.5f, -0.5f},
       },
         {0, 1, 2, 0, 2, 3}},
      // Front
      {{
           glm::vec3{-0.5f, 0.5f, 0.5f},
           glm::vec3{0.5f, 0.5f, 0.5f},
           glm::vec3{0.5f, -0.5f, 0.5f},
           glm::vec3{-0.5f, -0.5f, 0.5f},
       },
         {0, 1, 2, 0, 2, 3}},
  }};

  static constexpr glm::ivec3 ChunkSize{16, 384, 16};  //{16, 384, 16};

  std::array<uint8_t, ChunkSize.x * ChunkSize.y * ChunkSize.z> blocks;

  Mesh          mMesh;
  ShaderProgram mProgram;
  Atlas&        mAtlas;
};
}  // namespace dubu::block