#version 330 core

layout (location = 0) in vec4 verts;
out vec2 fragCol;

void main()
{
gl_Position = vec4(verts.xy, 0, 1);
fragCol = verts.zw;
}