#pragma once

#include <array>

#include <dubu_log/dubu_log.h>

#include "game/chunk.hpp"
#include "gl/mesh.hpp"

namespace dubu::block {

class PipelineStage {
public:
  struct ChunkData {
    std::array<BlockType, Chunk::BlockCount> solidBlocks = {};
    std::vector<Mesh::Vertex>                vertices    = {};
    std::vector<unsigned int>                indices     = {};
    std::size_t                              stage       = {};
  };

protected:
  struct CreateInfo {
    bool needsNeighbours = false;
  };
  struct Context {};

public:
  PipelineStage(const CreateInfo& createInfo)
      : mCreateInfo(createInfo) {}
  virtual ~PipelineStage() = default;

  virtual void Transform(ChunkData& chunkData, Context context) const = 0;

private:
  const CreateInfo mCreateInfo = {};
};

}  // namespace dubu::block