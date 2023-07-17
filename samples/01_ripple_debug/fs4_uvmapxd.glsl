#version 450
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUV;
layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D image;

// UVMAP 2D/3D 共通処理

void main()
{
  vec3 col = texture(image, vUV).rgb;

  FragColor = vec4(col, 1.0);
}
