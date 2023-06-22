#version 330 core

out vec4 FragColor;

uniform sampler2D atlas;

in vec3 color;
in vec2 uv0;
in float ao;

in vec4 fogColor;

void main(){
  vec4 texel = texture(atlas, uv0);

  if(texel.a < 0.5){
    discard;
  }

  vec3 diffuse = texel.rgb;

  diffuse *= color;

  diffuse *= ao;

  diffuse = mix(diffuse, fogColor.rgb, fogColor.a);

  FragColor = vec4(diffuse, 1.0);
}