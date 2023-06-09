#version 460

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
  vec3 col = mix(
    pow(texture(texture1, TexCoord).rgb, vec3(2.2)),
    pow(texture(texture2, TexCoord).rgb, vec3(2.2)),
    0.2);
  FragColor = vec4(pow(col, vec3(0.4545)), 1.0);
}