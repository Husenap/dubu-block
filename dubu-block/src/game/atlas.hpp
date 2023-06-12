#pragma once

#include <unordered_map>

#include <dubu_log/dubu_log.h>
#include <dubu_rect_pack/dubu_rect_pack.hpp>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <stb/stb_image.h>

namespace dubu::block {

class Atlas {
public:
  Atlas()
      : packer(Size, Size) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 6, GL_RGB8, Size, Size);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  ~Atlas() { glDeleteTextures(1, &texture); }

  std::pair<glm::vec2, glm::vec2> GetUVs(uint8_t id) {
    auto it = IdToUV.find(id);

    if (it == IdToUV.end()) {
      int width, height, channels;
      stbi_set_flip_vertically_on_load(true);

      const auto filepath =
          id == 1 ? "assets/textures/block/stone.png" : "assets/textures/block/dirt.png";
      const auto data = stbi_load(filepath, &width, &height, &channels, 3);
      if (!data) DUBU_LOG_FATAL("Failed to load texture: {}", filepath);

      DUBU_LOG_DEBUG("Loaded texture: {}x{}, {} channels", width, height, channels);

      const auto rect = packer.Pack({(unsigned int)width, (unsigned int)height});
      if (!rect) DUBU_LOG_FATAL("Failed to fit rect into atlas!");

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexSubImage2D(
          GL_TEXTURE_2D, 0, rect->x, rect->y, rect->w, rect->h, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(data);

      DUBU_LOG_DEBUG(
          "Fit Rect into Atlas at:({},{}) with size:({},{})", rect->x, rect->y, rect->w, rect->h);
      it = IdToUV
               .emplace(id,
                        std::make_pair(glm::vec2(rect->x / AtlasSize.x, rect->y / AtlasSize.y),
                                       glm::vec2(rect->w / AtlasSize.x, rect->h / AtlasSize.y)))
               .first;
    }

    return it->second;
  }

  void Bind(GLenum location) {
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void Debug() {
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), {256, 256});
  }

private:
  dubu::rect_pack::Packer packer;
  struct TextureEntry {};
  std::unordered_map<uint8_t, std::pair<glm::vec2, glm::vec2>> IdToUV;

  GLuint texture;

  static constexpr int Size = 128;
  glm::vec2            AtlasSize{Size, Size};
};

}  // namespace dubu::block