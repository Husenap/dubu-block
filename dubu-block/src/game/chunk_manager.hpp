#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "chunk.hpp"

namespace dubu::block {

class ChunkManager {
public:
  enum class ChunkLoadingPriority { Update, Generate, Optimize };

  ChunkManager(Atlas& atlas, const BlockDescriptions& blockDescriptions);

  void LoadChunk(const ChunkCoords& chunkCoords, ChunkLoadingPriority priority);

  void Update(const glm::vec3& cameraPosition);

  const Chunk* FindChunk(const ChunkCoords& chunkCoords) const {
    if (auto chunk = chunks.find(chunkCoords); chunk != chunks.end()) return chunk->second.get();
    return nullptr;
  }

  BlockId GetBlockIdAt(glm::ivec3 coords) const;

  void Debug();

private:
  inline float ChunkDistanceFromCamera(const ChunkCoords& coords,
                                       const glm::vec3&   cameraPosition) const {
    const float dx = (coords.x + 0.5f) - cameraPosition.x / (float)(Chunk::ChunkSize.x);
    const float dz = (coords.z + 0.5f) - cameraPosition.z / (float)(Chunk::ChunkSize.z);
    const float d  = dx * dx + dz * dz;
    return d;
  }

  std::unordered_map<ChunkCoords, std::unique_ptr<Chunk>>   chunks;
  std::vector<std::pair<ChunkCoords, ChunkLoadingPriority>> chunksToLoad;
  std::unordered_set<ChunkCoords>                           queued;

  Atlas&                   mAtlas;
  const BlockDescriptions& mBlockDescriptions;
};

}  // namespace dubu::block