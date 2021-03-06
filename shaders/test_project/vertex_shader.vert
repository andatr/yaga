#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformObject {
  mat4 view;
  mat4 proj;
} uo;

layout(push_constant) uniform PushConsts {
  mat4 model;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUv;

void main() {
  gl_Position = uo.proj * uo.view * pc.model * vec4(inPosition, 1.0);
  outColor = inColor;
  outUv = inUv;
}