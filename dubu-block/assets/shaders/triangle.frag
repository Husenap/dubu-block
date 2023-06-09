#version 460

in vec2 TexCoord;
in float Texture;

out vec4 FragColor;

uniform sampler2DArray texture1;
uniform sampler2DArray texture2;

uniform float frame1;
uniform float frame2;

void main(){
  vec3 col = mix(
    pow(texture(texture1, vec3(TexCoord, floor(frame1))).rgb, vec3(1.0)),
    pow(texture(texture2, vec3(TexCoord, floor(frame2))).rgb, vec3(1.0)),
    Texture);
  FragColor = vec4(pow(col, vec3(1.0)), 1.0);
}