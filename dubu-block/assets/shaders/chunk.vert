#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 mvp;

out vec2 UV;

void main(){
  gl_Position = mvp * vec4(aPos, 1.0);
  UV = aUV;
}