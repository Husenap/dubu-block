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
    std::vector<const char*> texturePaths  = {};
    glm::vec3                color         = {1, 1, 1};
    int                      topTexture    = 0;
    int                      sideTexture   = 0;
    int                      bottomTexture = 0;
    bool                     isOpaque      = true;
  };

  BlockDescription(const CreateInfo createInfo)
      : mCreateInfo(createInfo) {}

  int GetTextureIndexFromDirection(glm::vec3 direction) const {
    if (direction.y > 0.5f) return mCreateInfo.topTexture;
    if (direction.y < -0.5f) return mCreateInfo.bottomTexture;
    return mCreateInfo.sideTexture;
  }

  inline std::string_view GetTexturePath(int index) const {
    DUBU_LOG_DEBUG("Finding texture index {} in texture paths {}", index, mCreateInfo.texturePaths);
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
  OakLog    = 5,
  OakLeaves = 6,
};

class BlockDescriptions {
public:
  BlockDescriptions() {
    RegisterBlock(Bedrock, {{.texturePaths = {{"assets/textures/block/bedrock.png"}}}});
    RegisterBlock(Stone, {{.texturePaths = {{"assets/textures/block/stone.png"}}}});
    RegisterBlock(Dirt, {{.texturePaths = {{"assets/textures/block/dirt.png"}}}});
    RegisterBlock(Grass,
                  {{.texturePaths  = {{"assets/textures/block/grass_carried.png",
                                       "assets/textures/block/grass_side_carried.png",
                                       "assets/textures/block/dirt.png"}},
                    .topTexture    = 0,
                    .sideTexture   = 1,
                    .bottomTexture = 2}});
    RegisterBlock(OakLog,
                  {{.texturePaths  = {{
                        "assets/textures/block/log_oak.png",
                        "assets/textures/block/log_oak_top.png",
                    }},
                    .topTexture    = 1,
                    .sideTexture   = 0,
                    .bottomTexture = 1}});
    RegisterBlock(OakLeaves,
                  {{.texturePaths = {{"assets/textures/block/leaves_oak.tga"}},
                    .color        = {0.2f, 0.8f, 0.3f},
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