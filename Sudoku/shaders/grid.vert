#version 330 core
layout (location = 0) in vec2 pos;
out vec3 fragCol;

uniform vec3 col;
uniform mat4 projection;

void main()
{
gl_Position = projection * vec4(pos, 0, 1);
fragCol = col;
}