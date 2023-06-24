#include "chunk_manager.hpp"

#include <glm/gtx/norm.hpp>
#include <imgui.h>

namespace dubu::block {

ChunkManager::ChunkManager(Atlas&                   atlas,
                           const BlockDescriptions& blockDescriptions,
                           const Seed&              seed)
    : mAtlas(atlas)
    , mBlockDescriptions(blockDescriptions)
    , mSeed(seed) {}

void ChunkManager::LoadChunk(const ChunkCoords& chunkCoords, ChunkLoadingPriority priority) {
  if (!queued.contains(chunkCoords)) {
    chunksToLoad.push_back({chunkCoords, priority});
    queued.insert(chunkCoords);
  }
}

void ChunkManager::Update(const glm::vec3& cameraPosition, float time) {
  if (glm::distance2(mPreviousCameraPosition, cameraPosition) > 20 * 20) {
    DUBU_LOG_DEBUG("Cleaning up chunks");
    std::erase_if(chunks, [this, &cameraPosition](const auto& p) {
      return ChunkDistanceFromCamera(p.first, cameraPosition) > 50 * 50;
    });
    mPreviousCameraPosition = cameraPosition;
    chunksToLoad.clear();
    queued.clear();
  }
  if (!chunksToLoad.empty()) {
    const auto removeFrom = std::remove_if(
        chunksToLoad.begin(), chunksToLoad.end(), [this, &cameraPosition](const auto& p) {
          return ChunkDistanceFromCamera(p.first, cameraPosition) > 50 * 50;
        });
    for (auto it = removeFrom; it != chunksToLoad.end(); ++it) {
      queued.extract(it->first);
    }
    chunksToLoad.erase(removeFrom, chunksToLoad.end());
  }

  if (!chunksToLoad.empty()) {
    std::sort(chunksToLoad.begin(),
              chunksToLoad.end(),
              [this, &cameraPosition](const auto& lhs, const auto& rhs) {
                return ChunkDistanceFromCamera(lhs.first, cameraPosition) +
                           static_cast<float>(lhs.second) * 10 * 10 >
                       ChunkDistanceFromCamera(rhs.first, cameraPosition) +
                           static_cast<float>(rhs.second) * 10 * 10;
              });

    const auto& [coords, priority] = chunksToLoad.back();
    chunksToLoad.pop_back();

    switch (priority) {
    case ChunkLoadingPriority::Generate:
      chunks.emplace(
          coords, std::make_unique<Chunk>(coords, *this, mAtlas, mBlockDescriptions, mSeed, time));
      break;
    default:
      chunks.at(coords)->Optimize();
      break;
    }
    queued.extract(coords);
  }
}

BlockId ChunkManager::GetBlockIdAt(glm::ivec3 coords) const {
  if (auto chunk = chunks.find({coords.x < 0 ? (-1 - ((-coords.x - 1) / Chunk::ChunkSize.x))
                                             : coords.x / Chunk::ChunkSize.x,
                                coords.z < 0 ? (-1 - ((-coords.z - 1) / Chunk::ChunkSize.z))
                                             : coords.z / Chunk::ChunkSize.z});
      chunk != chunks.end()) {
    return chunk->second->GetBlockIdAtWorldCoords(coords);
  }
  return BlockType::Empty;
}

void ChunkManager::Debug() {
  if (ImGui::Button("Clear")) {
    chunks.clear();
    chunksToLoad.clear();
    queued.clear();
  }
  ImGui::LabelText("Chunks Loaded", "%ld", chunks.size());
}

}  // namespace dubu::block
