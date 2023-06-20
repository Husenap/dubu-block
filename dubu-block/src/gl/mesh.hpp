#pragma once

#include <vector>

#include <dubu_log/dubu_log.h>
#include <glad/glad.h>

namespace dubu::block {
class Mesh {
public:
  struct CreateInfo {
    GLenum usage = GL_STATIC_DRAW;
  };
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 uv0;
    float     ao;
  };

  Mesh(const CreateInfo createInfo)
      : mCreateInfo(createInfo) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    Bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv0));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, ao));
    Unbind();
  }

  ~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
  }

  void UpdateMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
    Bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(indices[0]),
                 indices.data(),
                 mCreateInfo.usage);
    indexCount = static_cast<GLsizei>(indices.size());

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), mCreateInfo.usage);
    Unbind();
  }

  int Draw() const {
    Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    Unbind();
    return indexCount / 3;
  }

private:
  void Bind() const { glBindVertexArray(vao); }
  void Unbind() const { glBindVertexArray(0); }

  const CreateInfo mCreateInfo;
  GLuint           vao, vbo, ebo;

  GLsizei indexCount;
};
}  // namespace dubu::block