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

void main()
{
  const vec2 aspectVec = vec2(uo.screen.z, 1.0);
  const vec2 thickness = vec2(inPrev.w, inNext.w);
  mat4 projViewModel = uo.proj * uo.view * pc.model;

  vec4 projected     = projViewModel * vec4(inPosition.xyz, 1.0);
  vec4 nextProjected = projViewModel * vec4(inNext.xyz,     1.0);
  vec4 prevProjected = projViewModel * vec4(inPrev.xyz,     1.0);

  vec2 screen     = projected.xy     / projected.w     * aspectVec;
  vec2 nextScreen = nextProjected.xy / nextProjected.w * aspectVec;
  vec2 prevScreen = prevProjected.xy / prevProjected.w * aspectVec;

  vec2 offset;
  vec2 dirNext = normalize(nextScreen - screen);
  vec2 dirPrev = normalize(screen - prevScreen);
  if (inPosition.w > 1.0) {
    offset = (-dirNext * thickness.x + vec2(-dirNext.y, dirNext.x) * thickness.y) / aspectVec;
  }
  else {
    vec2 tangent = normalize(dirPrev + dirNext);
    vec2 normal  = vec2(-tangent.y, tangent.x);
    vec2 perp    = vec2(-dirNext.y, dirNext.x);
    float len = thickness.x / dot(normal, perp);
    offset = normal * len / aspectVec;
  }

  gl_Position = projected + vec4(offset, 0.0f, 1.0);
  outColor = inColor;
  outTexture = inTexture;
}