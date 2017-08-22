#version 430 core

out vec4 fColor;
in vec2 texCoord;

uniform sampler2D theTexture;

void main()
{
	vec4 color = texture(theTexture, texCoord);
    fColor = color;
}