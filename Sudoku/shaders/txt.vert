#version 330 core

layout (location = 0) in vec4 verts;
out vec2 texCoords;


uniform mat4 projection;

void main()
{
gl_Position = projection * vec4(verts.xy, 0, 1);
texCoords = verts.zw;
}