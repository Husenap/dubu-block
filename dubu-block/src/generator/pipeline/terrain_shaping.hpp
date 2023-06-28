#pragma once

#include "stage.hpp"

namespace dubu::block {

class TerrainShapingStage : public PipelineStage {
public:
  TerrainShapingStage()
      : PipelineStage({}) {}

  virtual ~TerrainShapingStage() = default;

  void Transform(ChunkData& chunkData, Context) const override {
    DUBU_LOG_DEBUG("Terrain Shaping Stage");
    for (int x = 0; x < Chunk::ChunkSize.x; ++x) {
      for (int z = 0; z < Chunk::ChunkSize.z; ++z) {
        for (int y = 0; y < Chunk::ChunkSize.y; ++y) {
          chunkData.solidBlocks[Chunk::CoordsToIndex({x, y, z})] =
              y < 127 ? BlockType::Stone : BlockType::Empty;
        }
      }
    }
  }
};

}  // namespace dubu::block