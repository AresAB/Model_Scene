#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture1;

void main()
{
    FragColor = (Normal + 1) * 0.5;
}

