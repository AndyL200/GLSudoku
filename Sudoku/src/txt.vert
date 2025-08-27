#version 330 core

layout (location = 0) in vec4 verts;
out vec2 texCoords;

void main()
{
gl_Position = vec4(verts.xy, 0, 1);
texCoords = verts.zw;
}