#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D scr_tex;

void main()
{
    FragColor = texture(scr_tex, TexCoord).xyz;
}