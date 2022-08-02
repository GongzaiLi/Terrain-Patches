#version 330

layout (location = 0) in vec4 position;

// program Uniforms
uniform mat4 mvpMatrix;

void main()
{
   // gl_Position = mvpMatrix * position;
   gl_Position = position;
  
}
