#version 330 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 text_color;



void main()
{
    float alpha = texture(tex, texCoords).r;
    FragColor = vec4(text_color, alpha);
}