#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inUv;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = inColor; //texture(texSampler, inUv);
}
