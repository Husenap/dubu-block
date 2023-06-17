#pragma once

#include <string_view>
#include <unordered_map>

#include <dubu_log/dubu_log.h>
#include <glm/glm.hpp>

namespace dubu::block {

using BlockId = uint8_t;

class BlockDescription {
public:
  struct CreateInfo {
    std::vector<std::string_view> texturePaths  = {};
    glm::vec3                     color         = {1, 1, 1};
    int                           topTexture    = 0;
    int                           sideTexture   = 0;
    int                           bottomTexture = 0;
    bool                          isOpaque      = true;
  };

  BlockDescription(const CreateInfo createInfo)
      : mCreateInfo(createInfo) {}

  int GetTextureIndexFromDirection(glm::vec3 direction) const {
    if (direction.y > 0.5f) return mCreateInfo.topTexture;
    if (direction.y < -0.5f) return mCreateInfo.bottomTexture;
    return mCreateInfo.sideTexture;
  }

  inline std::string_view GetTexturePath(int index) const {
    return mCreateInfo.texturePaths[index];
  }

  inline const glm::vec3& GetColor() const { return mCreateInfo.color; }

  inline bool IsOpaque() const { return mCreateInfo.isOpaque; }
  inline bool IsTransparent() const { return !mCreateInfo.isOpaque; }

private:
  friend class BlockDescriptions;
  const CreateInfo mCreateInfo;
};

enum BlockType : BlockId {
  Empty     = 0,
  Bedrock   = 1,
  Stone     = 2,
  Dirt      = 3,
  Grass     = 4,
  OakLeaves = 5,
};

class BlockDescriptions {
public:
  BlockDescriptions() {
    RegisterBlock(Bedrock, {{.texturePaths = {{"assets/textures/block/bedrock.png"}}}});
    RegisterBlock(Stone, {{.texturePaths = {{"assets/textures/block/stone.png"}}}});
    RegisterBlock(Dirt, {{.texturePaths = {{"assets/textures/block/dirt.png"}}}});
    RegisterBlock(Grass,
                  {{.texturePaths  = {{"assets/textures/block/grass_carried.png",
                                       "assets/textures/block/grass_side.tga",
                                       "assets/textures/block/dirt.png"}},
                    .topTexture    = 0,
                    .sideTexture   = 1,
                    .bottomTexture = 2,
                    .isOpaque      = false}});
    RegisterBlock(OakLeaves,
                  {{.texturePaths = {{"assets/textures/block/oak_leaves.png"}},
                    .color        = {0.1f, 1.f, 0.2f},
                    .isOpaque     = false}});
  }
  void RegisterBlock(BlockId id, BlockDescription description) {
    auto [it, inserted] = mBlockDescriptions.try_emplace(id, description);
    if (!inserted) {
      DUBU_LOG_FATAL(
          "Trying to add block with id {} but it already exists!\nExisting Textures:{}\nNew "
          "Textures:{}",
          id,
          it->second.mCreateInfo.texturePaths,
          description.mCreateInfo.texturePaths);
    }
  }
  const BlockDescription& GetBlockDescription(BlockId id) const {
    auto it = mBlockDescriptions.find(id);
    if (it == mBlockDescriptions.end()) {
      DUBU_LOG_ERROR("Tried to access unknown block id:{}", (int)id);
      return mErrorBlockDescription;
    }
    return it->second;
  }
  const BlockDescription& GetErrorBlockDescription() const { return mErrorBlockDescription; }

private:
  const BlockDescription mErrorBlockDescription{{
      .texturePaths = {{"assets/textures/block/error.png"}},
  }};

  std::unordered_map<BlockId, BlockDescription> mBlockDescriptions;
};

}  // namespace dubu::block