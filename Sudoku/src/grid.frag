#version 330 core
in vec3 fragCol;


void main()
{
gl_FragColor = vec4(fragCol, 1);
}