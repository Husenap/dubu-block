#pragma once

#include <glad/gl.h>

namespace dubu::block {

template <GLenum SHADER_TYPE>
class Shader {
public:
  Shader(std::vector<unsigned char> shaderCode) {
    mShader = glCreateShader(SHADER_TYPE);
    shaderCode.push_back(0);
    auto c_str = reinterpret_cast<const char*>(shaderCode.data());
    glShaderSource(mShader, 1, &c_str, NULL);
    glCompileShader(mShader);
  }
  ~Shader() { glDeleteShader(mShader); }

  std::optional<std::string> GetError() {
    int success(-1);
    glGetShaderiv(mShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char buffer[4096];
      glGetShaderInfoLog(mShader, 4096, nullptr, buffer);
      return std::string(buffer);
    }
    return std::nullopt;
  }

  GLuint mShader;
};

using VertexShader   = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

}  // namespace dubu::block