#pragma once

#include "stage.hpp"

namespace dubu::block {

class MeshGenerationStage : public PipelineStage {
public:
  MeshGenerationStage()
      : PipelineStage({.needsNeighbours = true}) {}

  virtual ~MeshGenerationStage() = default;

  void Transform(ChunkData& chunkData, Context) const override {
    DUBU_LOG_DEBUG("Mesh Generation Stage");
    for (std::size_t index = 0; index < Chunk::BlockCount; ++index) {
      const auto blockType = chunkData.solidBlocks[index];

      if (blockType == BlockType::Empty) continue;

      const auto myCoord = Chunk::IndexToCoords(index);

      for (std::size_t d = 0; d < Directions.size(); ++d) {
        const auto& dir        = Directions[d];
        const auto  otherCoord = myCoord + dir;
        if (!Chunk::AreCoordsBounded(otherCoord) ||
            chunkData.solidBlocks[Chunk::CoordsToIndex(otherCoord)] == BlockType::Empty)
          continue;

        const auto& faceData = DirectionToFace[d];

        const glm::vec3 offsetPosition = myCoord;

        chunkData.vertices.push_back({.position = faceData.vertices[0] + offsetPosition,
                                      .color    = {1, 1, 1},
                                      .uv0      = {0, 0},
                                      .ao       = 1.0f});
        chunkData.vertices.push_back({.position = faceData.vertices[1] + offsetPosition,
                                      .color    = {1, 1, 1},
                                      .uv0      = {0, 0},
                                      .ao       = 1.0f});
        chunkData.vertices.push_back({.position = faceData.vertices[2] + offsetPosition,
                                      .color    = {1, 1, 1},
                                      .uv0      = {0, 0},
                                      .ao       = 1.0f});
        chunkData.vertices.push_back({.position = faceData.vertices[3] + offsetPosition,
                                      .color    = {1, 1, 1},
                                      .uv0      = {0, 0},
                                      .ao       = 1.0f});

        const unsigned int startIndex = static_cast<unsigned int>(chunkData.vertices.size()) - 4;
        chunkData.indices.push_back(startIndex + faceData.indices[0]);
        chunkData.indices.push_back(startIndex + faceData.indices[1]);
        chunkData.indices.push_back(startIndex + faceData.indices[2]);
        chunkData.indices.push_back(startIndex + faceData.indices[3]);
        chunkData.indices.push_back(startIndex + faceData.indices[4]);
        chunkData.indices.push_back(startIndex + faceData.indices[5]);
      }
    }
  }

private:
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
};

}  // namespace dubu::block