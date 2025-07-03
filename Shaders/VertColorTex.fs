#version 330 core
out vec4 FragColor;
in vec3 myColor;
in vec2 myTex;
uniform sampler2D mytexture;
void main()
{
	FragColor = texture(mytexture, myTex) * vec4(myColor, 1.0);
}