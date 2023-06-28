#pragma once

#include "stage.hpp"

namespace dubu::block {

class FeaturesStructuresStage : public PipelineStage {
public:
  FeaturesStructuresStage()
      : PipelineStage({}) {}

  virtual ~FeaturesStructuresStage() = default;

  void Transform(ChunkData& chunkData, Context) const override {
    DUBU_LOG_DEBUG("Features & Structures Stage");
    for (int x = 0; x < Chunk::ChunkSize.x; ++x) {
      for (int z = 0; z < Chunk::ChunkSize.z; ++z) {
        for (int y = Chunk::ChunkSize.y - 2; y > 127; --y) {
          if ((x + y + z) % 3) continue;
          if (chunkData.solidBlocks[Chunk::CoordsToIndex({x, y, z})] == BlockType::Empty) continue;
          const auto index = Chunk::CoordsToIndex({x, y + 1, z});
          if (chunkData.solidBlocks[index] != BlockType::Empty) continue;
          chunkData.solidBlocks[index] = BlockType::OakLog;
        }
      }
    }
  }
};

}  // namespace dubu::block