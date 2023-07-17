#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec2 vUV;

void main()
{
  vPosition = aPosition;
  vUV = aUV;

  gl_Position = vec4(aPosition, 1.0);
}
