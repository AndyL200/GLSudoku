#version 330 core

in vec2 texCoords;
uniform sampler2D tex;
uniform vec4 color;

out vec4 FragColor;

void main()
{
FragColor = vec4(1,1,1,texture(tex, texCoords).r) * color;
}