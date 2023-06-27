#pragma once

#include "stage.hpp"

namespace dubu::block {

class WaterFillingStage : public PipelineStage {
public:
  WaterFillingStage()
      : PipelineStage({}) {}

  virtual ~WaterFillingStage() = default;

  void Transform(ChunkData& chunkData, Context) const override {
    DUBU_LOG_DEBUG("Water Filling Stage");
    for (int x = 0; x < Chunk::ChunkSize.x; ++x) {
      for (int z = 0; z < Chunk::ChunkSize.z; ++z) {
        for (int y = 127; y >= 0; --y) {
          const auto index = Chunk::CoordsToIndex({x, y, z});
          if (chunkData.solidBlocks[index] != BlockType::Empty) break;
          chunkData.solidBlocks[index] = BlockType::Water;
        }
      }
    }
  }
};

}  // namespace dubu::block