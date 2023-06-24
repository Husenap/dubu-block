#pragma once

#include <string_view>
#include <unordered_map>

#include <dubu_log/dubu_log.h>
#include <glm/glm.hpp>

namespace dubu::block {

class BlockDescription {
public:
  struct CreateInfo {
    std::vector<const char*> texturePaths  = {};
    glm::vec3                color         = {1, 1, 1};
    uint8_t                  topTexture    = 0;
    uint8_t                  sideTexture   = 0;
    uint8_t                  bottomTexture = 0;
    bool                     isOpaque      = true;
  };

  BlockDescription(const CreateInfo createInfo)
      : mCreateInfo(createInfo) {}

  uint8_t GetTextureIndexFromDirection(glm::vec3 direction) const {
    if (direction.y > 0.5f) return mCreateInfo.topTexture;
    if (direction.y < -0.5f) return mCreateInfo.bottomTexture;
    return mCreateInfo.sideTexture;
  }

  inline std::string_view GetTexturePath(uint8_t index) const {
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

enum class BlockType : uint8_t {
  Empty     = 0,
  Bedrock   = 1,
  Stone     = 2,
  Dirt      = 3,
  Grass     = 4,
  OakLog    = 5,
  OakLeaves = 6,
  Water     = 7,
};

class BlockDescriptions {
public:
  BlockDescriptions() {
    RegisterBlock(BlockType::Bedrock, {{.texturePaths = {{"assets/textures/block/bedrock.png"}}}});
    RegisterBlock(BlockType::Stone, {{.texturePaths = {{"assets/textures/block/stone.png"}}}});
    RegisterBlock(BlockType::Dirt, {{.texturePaths = {{"assets/textures/block/dirt.png"}}}});
    RegisterBlock(BlockType::Grass,
                  {{.texturePaths  = {{"assets/textures/block/grass_carried.png",
                                       "assets/textures/block/grass_side_carried.png",
                                       "assets/textures/block/dirt.png"}},
                    .topTexture    = 0,
                    .sideTexture   = 1,
                    .bottomTexture = 2}});
    RegisterBlock(BlockType::OakLog,
                  {{.texturePaths  = {{
                        "assets/textures/block/log_oak.png",
                        "assets/textures/block/log_oak_top.png",
                    }},
                    .topTexture    = 1,
                    .sideTexture   = 0,
                    .bottomTexture = 1}});
    RegisterBlock(BlockType::OakLeaves,
                  {{.texturePaths = {{"assets/textures/block/leaves_oak.tga"}},
                    .color        = {0.2f, 0.8f, 0.3f},
                    .isOpaque     = false}});
    RegisterBlock(BlockType::Water,
                  {{.texturePaths = {{"assets/textures/block/water_placeholder.png"}}}});
  }
  void RegisterBlock(BlockType id, BlockDescription description) {
    auto [it, inserted] = mBlockDescriptions.try_emplace(id, description);
    if (!inserted) {
      DUBU_LOG_FATAL(
          "Trying to add block with id {} but it already exists!\nExisting Textures:{}\nNew "
          "Textures:{}",
          (int)id,
          it->second.mCreateInfo.texturePaths,
          description.mCreateInfo.texturePaths);
    }
  }
  const BlockDescription& GetBlockDescription(BlockType id) const {
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

  std::unordered_map<BlockType, BlockDescription> mBlockDescriptions;
};

}  // namespace dubu::block