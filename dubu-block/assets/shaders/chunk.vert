#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV0;
layout(location = 3) in float aAO;

uniform mat4  MODELVIEWPROJ;
uniform mat4  MODEL;
uniform mat4  MODELVIEW;
uniform mat4  PROJ;
uniform vec3  SKYCOLOR;
uniform float RENDER_DISTANCE;
uniform vec2  FOG_CONTROL;
uniform float AGE;

out vec3  color;
out vec2  uv0;
out float ao;

out vec4 fogColor;

void main() {
  color = aColor;
  uv0   = aUV0;
  ao    = aAO;

  vec4  worldPos    = MODELVIEW * vec4(aPos, 1.0);
  float cameraDepth = length(worldPos.xyz);
  fogColor.rgb      = SKYCOLOR;
  fogColor.a        = mix(
      1.0 - exp(-(1 / FOG_CONTROL.y) * pow(cameraDepth, 2.0)) *
                smoothstep(RENDER_DISTANCE, RENDER_DISTANCE - 16.0 * FOG_CONTROL.x, cameraDepth),
      1.0,
      exp(-5.6 * AGE));

  gl_Position = PROJ * vec4(worldPos.xyz, 1.0);
}