#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformObject {
  mat4 view;
  mat4 proj;
  vec3 screen;
} uo;

layout(push_constant) uniform PushConsts {
  mat4 model;
} pc;

layout(location = 0) in vec4  inPosition;
layout(location = 1) in vec4  inNext;
layout(location = 2) in vec4  inPrev;
layout(location = 3) in vec2  inTexture;
layout(location = 4) in vec4  inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexture;

void main() {
  gl_Position = uo.proj * uo.view * pc.model * inPosition;
  outColor = inColor;
  outTexture = inTexture;
}