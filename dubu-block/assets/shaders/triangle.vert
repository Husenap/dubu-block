#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexture;

out vec2 TexCoord;
out float Texture;

uniform mat4 mvp;

void main(){
  gl_Position = mvp * vec4(aPos, 1.0);
  TexCoord = aTexCoord;
  Texture = aTexture;
}