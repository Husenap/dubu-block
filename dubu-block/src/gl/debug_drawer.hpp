#pragma once

#include <vector>

#include <dubu_log/dubu_log.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl/shader.hpp"
#include "gl/shader_program.hpp"
#include "glm/fwd.hpp"
#include "io/io.hpp"
#include "linalg/aabb.hpp"

namespace dubu::block {

class DebugDrawer {
public:
  DebugDrawer() {
    glGenVertexArrays(1, &mLineVAO);
    glGenBuffers(1, &mLineVBO);

    Bind();
    glBindBuffer(GL_ARRAY_BUFFER, mLineVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (GLvoid*)offsetof(LineVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (GLvoid*)offsetof(LineVertex, color));
    Unbind();

    VertexShader   lineVertexShader(ReadFile("assets/shaders/debug/line.vert"));
    FragmentShader lineFragmentShader(ReadFile("assets/shaders/debug/line.frag"));
    mLineProgram.Link(lineVertexShader, lineFragmentShader);
    if (const auto err = mLineProgram.GetError()) {
      DUBU_LOG_ERROR("shader program error: {}", *err);
    }
  }

  void Draw(const glm::mat4& viewProjection) {
    if (ourLineVertices.empty()) return;
    Bind();
    mLineProgram.Use();
    glUniformMatrix4fv(
        mLineProgram.GetUniformLocation("vp"), 1, GL_FALSE, glm::value_ptr(viewProjection));

    glBindBuffer(GL_ARRAY_BUFFER, mLineVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 ourLineVertices.size() * sizeof(ourLineVertices[0]),
                 ourLineVertices.data(),
                 GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(ourLineVertices.size()));
    ourLineVertices.clear();
    Unbind();
  }

  static void DrawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color = {1, 1, 1}) {
    ourLineVertices.push_back({a, color});
    ourLineVertices.push_back({b, color});
  }
  static void DrawAABB(const AABB& aabb, glm::vec3 color = {1, 1, 1}) {
    DrawLine({aabb.min.x, aabb.min.y, aabb.min.z}, {aabb.max.x, aabb.min.y, aabb.min.z}, color);
    DrawLine({aabb.min.x, aabb.min.y, aabb.min.z}, {aabb.min.x, aabb.min.y, aabb.max.z}, color);
    DrawLine({aabb.min.x, aabb.min.y, aabb.max.z}, {aabb.max.x, aabb.min.y, aabb.max.z}, color);
    DrawLine({aabb.max.x, aabb.min.y, aabb.min.z}, {aabb.max.x, aabb.min.y, aabb.max.z}, color);

    DrawLine({aabb.min.x, aabb.min.y, aabb.min.z}, {aabb.min.x, aabb.max.y, aabb.min.z}, color);
    DrawLine({aabb.max.x, aabb.min.y, aabb.min.z}, {aabb.max.x, aabb.max.y, aabb.min.z}, color);
    DrawLine({aabb.max.x, aabb.min.y, aabb.max.z}, {aabb.max.x, aabb.max.y, aabb.max.z}, color);
    DrawLine({aabb.min.x, aabb.min.y, aabb.max.z}, {aabb.min.x, aabb.max.y, aabb.max.z}, color);

    DrawLine({aabb.min.x, aabb.max.y, aabb.min.z}, {aabb.max.x, aabb.max.y, aabb.min.z}, color);
    DrawLine({aabb.max.x, aabb.max.y, aabb.min.z}, {aabb.max.x, aabb.max.y, aabb.max.z}, color);
    DrawLine({aabb.max.x, aabb.max.y, aabb.max.z}, {aabb.min.x, aabb.max.y, aabb.max.z}, color);
    DrawLine({aabb.min.x, aabb.max.y, aabb.max.z}, {aabb.min.x, aabb.max.y, aabb.min.z}, color);
  }

private:
  void Bind() { glBindVertexArray(mLineVAO); }
  void Unbind() { glBindVertexArray(0); }

  struct LineVertex {
    glm::vec3 position;
    glm::vec3 color;
  };
  static inline std::vector<LineVertex> ourLineVertices;

  GLuint        mLineVAO;
  GLuint        mLineVBO;
  ShaderProgram mLineProgram;
};

}  // namespace dubu::block
