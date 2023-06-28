#pragma once

#include <vector>

#include <concurrentqueue.h>

#include "stage.hpp"

namespace dubu::block {

class Pipeline {
public:
  Pipeline()  = default;
  ~Pipeline() = default;

  template <typename T, typename... Args>
  std::enable_if_t<std::is_base_of_v<PipelineStage, T>> AddPipelineStage(Args&&... args) {
    mStages.emplace_back(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
  }

  void LoadChunk(ChunkCoords chunkCoords) {
    if (auto it = chunks.find(chunkCoords); it != chunks.end()) return;

    PipelineStage::ChunkData chunkData;
    chunkData.solidBlocks.fill(BlockType::Empty);

    while (chunkData.stage < mStages.size()) {
      mStages[chunkData.stage]->Transform(chunkData, {});
      ++chunkData.stage;
    }

    chunks[chunkCoords] = chunkData;
  }

  PipelineStage::ChunkData GetChunk(ChunkCoords chunkCoords) { return chunks[chunkCoords]; }

private:
  std::vector<std::unique_ptr<PipelineStage>>               mStages;
  std::unordered_map<ChunkCoords, PipelineStage::ChunkData> chunks;

  moodycamel::ConcurrentQueue<ChunkCoords> queuedChunks;
  moodycamel::ConcurrentQueue<ChunkCoords> finishedChunks;
};

}  // namespace dubu::block