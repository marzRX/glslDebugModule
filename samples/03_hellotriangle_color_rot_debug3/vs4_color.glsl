#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 0) out vec3 vColor;

//uniform mat4 uMVPMatrix;
uniform mat4 uMVPMatrix = mat4(1.0, 0.0, 0.0, 0.0, 
  0.0, 1.0, 0.0, 0.0,
  0.0, 0.0, 1.0, 0.0,
  0.0, 0.0, 0.0, 1.0);

void main()
{
  vColor = aColor;

  gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
//  gl_Position = vec4(aPosition, 1.0);
}