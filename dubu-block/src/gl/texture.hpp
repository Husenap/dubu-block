#pragma once

#include <dubu_log/dubu_log.h>
#include <glad/gl.h>
#include <stb/stb_image.h>

namespace dubu::block {

class Texture {
public:
  ~Texture() { glDeleteTextures(1, &texture); }

  void Load(const char* filepath) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      DUBU_LOG_FATAL("Failed to load texture!");
    }
    stbi_image_free(data);
  }

  void Bind(int location) {
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

private:
  GLuint texture;
};

}  // namespace dubu::block