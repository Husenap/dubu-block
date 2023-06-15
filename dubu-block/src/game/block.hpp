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
    std::vector<std::string_view> texturePaths;
    int                           topTexture    = 0;
    int                           sideTexture   = 0;
    int                           bottomTexture = 0;
    bool                          isSolid       = true;
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
  inline bool IsSolid() const { return mCreateInfo.isSolid; }
  inline bool IsTransparent() const { return !mCreateInfo.isSolid; }

private:
  friend class BlockDescriptions;
  const CreateInfo mCreateInfo;
};

enum BlockType : BlockId {
  Empty     = 0,
  Stone     = 1,
  Dirt      = 2,
  Barrel    = 3,
  OakLeaves = 4,
};

class BlockDescriptions {
public:
  BlockDescriptions() {
    RegisterBlock(Stone, {{.texturePaths = {{"assets/textures/block/stone.png"}}}});
    RegisterBlock(Dirt, {{.texturePaths = {{"assets/textures/block/dirt.png"}}}});
    RegisterBlock(Barrel,
                  {{.texturePaths  = {{"assets/textures/block/barrel_top.png",
                                       "assets/textures/block/barrel_side.png",
                                       "assets/textures/block/barrel_bottom.png"}},
                    .topTexture    = 0,
                    .sideTexture   = 1,
                    .bottomTexture = 2}});
    RegisterBlock(OakLeaves,
                  {{.texturePaths = {{"assets/textures/block/oak_leaves.png"}}, .isSolid = false}});
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
    if (it == mBlockDescriptions.end()) return mErrorBlockDescription;
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