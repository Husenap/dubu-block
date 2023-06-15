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
        0, 3, GL_FLOAT, GL_FALSE, sizeof(mVertices[0]), (GLvoid*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(mVertices[0]), (GLvoid*)offsetof(Vertex, uv));
    Unbind();
  }

  void SetVertices(const std::vector<glm::vec3>& vertices) {
    mVertices.resize(vertices.size());
    for (int i = 0; i < mVertices.size(); ++i) mVertices[i].position = vertices[i];
  }
  void SetUVs(const std::vector<glm::vec2>& uvs) {
    mVertices.resize(uvs.size());
    for (int i = 0; i < mVertices.size(); ++i) mVertices[i].uv = uvs[i];
  }
  void SetIndices(const std::vector<GLuint>& indices) {
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(indices[0]),
                 indices.data(),
                 mCreateInfo.usage);
    indexCount = static_cast<GLsizei>(indices.size());
    Unbind();
  }

  void UpdateMesh() {
    Bind();
    glBufferData(GL_ARRAY_BUFFER,
                 mVertices.size() * sizeof(mVertices[0]),
                 mVertices.data(),
                 mCreateInfo.usage);
    Unbind();
  }

  void Bind() { glBindVertexArray(vao); }
  void Unbind() { glBindVertexArray(0); }

  int Draw() {
    Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    Unbind();
    return indexCount / 3;
  }

private:
  const CreateInfo mCreateInfo;
  GLuint           vao, vbo, ebo;

  struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
  };
  std::vector<Vertex> mVertices;
  GLsizei             indexCount;
};
}  // namespace dubu::block