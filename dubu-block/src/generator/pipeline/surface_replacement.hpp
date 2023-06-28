#pragma once

#include "stage.hpp"

namespace dubu::block {

class SurfaceReplacementStage : public PipelineStage {
public:
  SurfaceReplacementStage()
      : PipelineStage({}) {}

  virtual ~SurfaceReplacementStage() = default;

  void Transform(ChunkData& chunkData, Context) const override {
    DUBU_LOG_DEBUG("Surface Replacement Stage");
    for (int x = 0; x < Chunk::ChunkSize.x; ++x) {
      for (int z = 0; z < Chunk::ChunkSize.z; ++z) {
        int toReplace = 3;
        for (int y = Chunk::ChunkSize.y - 1; y > 127; --y) {
          const auto index = Chunk::CoordsToIndex({x, y, z});
          if (chunkData.solidBlocks[index] == BlockType::Empty) {
            toReplace = 3;
          } else if (toReplace > 0) {
            chunkData.solidBlocks[index] = toReplace == 3 ? BlockType::Grass : BlockType::Dirt;
            --toReplace;
          }
        }
      }
    }
  }
};

}  // namespace dubu::block