#version 460 core

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D sTexture;

void main() {
  outColor = fragColor * texture(sTexture, fragUV);
}