#pragma once

#include <map>
#include <unordered_map>

#include <dubu_log/dubu_log.h>
#include <dubu_rect_pack/dubu_rect_pack.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <stb/stb_image.h>

#include "game/block.hpp"

namespace dubu::block {

class Atlas {
public:
  Atlas(const BlockDescriptions& blockDescriptions)
      : mPacker(Size, Size)
      , mBlockDescriptions(blockDescriptions) {
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 5, GL_RGBA8, Size, Size);
  }

  ~Atlas() { glDeleteTextures(1, &mTexture); }

  std::pair<glm::vec2, glm::vec2> GetUVs(BlockId id, glm::vec3 direction) {
    auto&     block        = mBlockDescriptions.GetBlockDescription(id);
    const int textureIndex = block.GetTextureIndexFromDirection(direction);

    const auto key = std::make_pair(id, textureIndex);

    auto it = mIdToUV.find(key);

    if (it == mIdToUV.end()) {
      int width, height, channels;
      stbi_set_flip_vertically_on_load(true);

      const auto filepath = block.GetTexturePath(textureIndex);

      const auto data = stbi_load(filepath.data(), &width, &height, &channels, 4);
      if (!data) {
        DUBU_LOG_FATAL("Failed to load texture: {}", filepath);
      }

      const auto rect = mPacker.Pack({(unsigned int)width, (unsigned int)height});
      if (!rect) {
        DUBU_LOG_FATAL("Failed to fit rect into atlas!");
      }

      glBindTexture(GL_TEXTURE_2D, mTexture);
      glTexSubImage2D(
          GL_TEXTURE_2D, 0, rect->x, rect->y, rect->w, rect->h, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(data);

      DUBU_LOG_DEBUG("Fit Rect into Atlas at:({},{}) with size:({},{}), texture:{}",
                     rect->x,
                     rect->y,
                     rect->w,
                     rect->h,
                     filepath);
      it = mIdToUV
               .emplace(key,
                        std::make_pair(glm::vec2(rect->x / AtlasSize.x, rect->y / AtlasSize.y),
                                       glm::vec2(rect->w / AtlasSize.x, rect->h / AtlasSize.y)))
               .first;
    }

    return it->second;
  }

  void Bind(GLenum location) {
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D, mTexture);
  }

  void Debug() {
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(mTexture)),
                 {256, 256},
                 {0, 1},
                 {1, 0},
                 {1, 1, 1, 1},
                 {0, 0, 0, 1});
  }

private:
  dubu::rect_pack::Packer                                            mPacker;
  std::map<std::pair<BlockId, int>, std::pair<glm::vec2, glm::vec2>> mIdToUV;

  GLuint mTexture;

  static constexpr int Size = 128;
  glm::vec2            AtlasSize{Size, Size};

  const BlockDescriptions& mBlockDescriptions;
};

}  // namespace dubu::block