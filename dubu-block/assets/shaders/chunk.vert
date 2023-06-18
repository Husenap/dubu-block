#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV0;

uniform mat4 MODELVIEWPROJ;
uniform mat4 MODEL;
uniform mat4 MODELVIEW;
uniform mat4 PROJ;
uniform vec3 SKYCOLOR;
uniform float RENDER_DISTANCE;
uniform vec2 FOG_CONTROL;

out vec3 color;
out vec2 uv0;

out vec4 fogColor;

void main(){
  gl_Position = MODELVIEWPROJ * vec4(aPos, 1.0);
  color = aColor;
  uv0 = aUV0;

  vec4 worldPos = MODELVIEW * vec4(aPos,1.0);
  float cameraDepth = length(worldPos.xyz);
  float len = cameraDepth / RENDER_DISTANCE;
  fogColor.rgb = SKYCOLOR;
  fogColor.a = 1.0
              - exp(-(1/FOG_CONTROL.y) * pow(cameraDepth, 2.0))
              * smoothstep(RENDER_DISTANCE, RENDER_DISTANCE - 16.0 * FOG_CONTROL.x, cameraDepth);
}