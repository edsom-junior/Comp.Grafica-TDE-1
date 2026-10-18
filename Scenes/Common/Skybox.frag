#version 330 core

in vec3 TexCoord;
out vec4 FragColor;

uniform samplerCube SkyboxTextures;

void main()
{
	FragColor = texture(SkyboxTextures, TexCoord);
}