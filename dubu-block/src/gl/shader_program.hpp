#pragma once

#include <optional>
#include <string>

#include <glad/gl.h>

namespace dubu::block {

class ShaderProgram {
public:
  ShaderProgram() {}
  ~ShaderProgram() { glDeleteProgram(mProgram); }

  template <typename... Shaders>
  void Link(Shaders&&... shaders) {
    mProgram = glCreateProgram();
    (AttachShader(shaders), ...);
    glLinkProgram(mProgram);
  }

  std::optional<std::string> GetError() {
    int success(-1);
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (!success) {
      char buffer[4096];
      glGetProgramInfoLog(mProgram, 4096, NULL, buffer);
      return std::string(buffer);
    }
    return std::nullopt;
  }

  void Use() const { glUseProgram(mProgram); }

  GLint GetAttributeLocation(std::string_view name) {
    return glGetAttribLocation(mProgram, name.data());
  }
  GLint GetUniformLocation(std::string_view name) {
    return glGetUniformLocation(mProgram, name.data());
  }

  GLuint mProgram;

private:
  template <typename T>
  void AttachShader(T&& shader) {
    glAttachShader(mProgram, shader.mShader);
  }
};

}  // namespace dubu::block