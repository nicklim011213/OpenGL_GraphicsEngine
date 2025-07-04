#version 330 core
out vec4 FragColor;
in vec3 myColor;
in vec2 myTex;
uniform sampler2D mytexture;
uniform vec3 lightcolor;
void main()
{
	FragColor = texture(mytexture, myTex) * vec4(myColor * lightcolor, 1.0);
}