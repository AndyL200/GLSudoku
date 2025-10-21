#version 330 core
layout (location = 0) in vec2 pos;
out vec3 fragCol;

uniform vec3 col;

void main()
{
gl_Position = vec4(pos, 0, 1);
fragCol = col;
}