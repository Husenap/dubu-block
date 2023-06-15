#version 460

out vec4 FragColor;

uniform sampler2D atlas;

in vec2 UV;

void main(){
  vec4 texel = texture(atlas, UV);

  if(texel.a < 0.5){
    discard;
  }

  FragColor = vec4(texel.rgb, 1.0);
}