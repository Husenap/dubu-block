#pragma once

#include <dubu_log/dubu_log.h>
#include <glad/gl.h>
#include <stb/stb_image.h>

#include "io/io.hpp"

namespace dubu::block {

class Gif {
public:
  ~Gif() {
    delete[] delays;
    glDeleteTextures(1, &texture);
  }

  void Load(std::string_view filepath) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    const auto rawData = read_file(filepath);

    auto data = stbi_load_gif_from_memory(rawData.data(),
                                          static_cast<int>(rawData.size()),
                                          &delays,
                                          &width,
                                          &height,
                                          &frames,
                                          &channels,
                                          4);

    DUBU_LOG_DEBUG("w:{}, h:{}, frames:{}, channels:{}", width, height, frames, channels);
    for (int i = 0; i < frames; ++i) {
      DUBU_LOG_DEBUG("frame {} delay: {}", i, delays[i]);
    }

    if (data) {
      glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, frames);
      glTexSubImage3D(
          GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, frames, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
      DUBU_LOG_FATAL("Failed to load texture!");
    }

    stbi_image_free(data);
  }

  void Bind(int location) {
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  }

  void Update(float deltaTime) {
    currentTime += deltaTime * 1000.f;
    while (currentTime > delays[currentFrame]) {
      currentTime -= delays[currentFrame];
      currentFrame = (currentFrame + 1) % frames;
    }
  }

  float GetFrame() { return currentFrame + currentTime / delays[currentFrame]; }

private:
  GLuint texture;
  float  currentTime;
  int*   delays;
  int    currentFrame = 0;
  int    frames;
};

}  // namespace dubu::block