#version 460

out vec4 FragColor;

uniform sampler2D atlas;

in vec2 UV;

void main(){
  vec3 color = texture(atlas, UV).rgb;
  FragColor = vec4(color, 1.0);
}