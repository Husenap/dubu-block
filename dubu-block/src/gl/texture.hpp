#pragma once

#include <dubu_log/dubu_log.h>
#include <glad/gl.h>
#include <stb/stb_image.h>

namespace dubu::block {

class Texture {
public:
  ~Texture() { glDeleteTextures(1, &texture); }

  void Load(std::string_view filepath) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.data(), &width, &height, &channels, 4);

    if (!data) DUBU_LOG_FATAL("Failed to load texture: {}", filepath);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
  }

  void Bind(GLenum location) {
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

private:
  GLuint texture;
};

}  // namespace dubu::block