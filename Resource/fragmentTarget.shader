#version 410 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D image;
uniform vec3 spriteColor;

void main()
{
	FragColor = vec4(spriteColor, 1.0);
}