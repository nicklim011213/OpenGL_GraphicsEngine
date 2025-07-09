#version 330 core
out vec4 FragColor;

in vec3 myColor;
in vec2 myTex;
in vec3 myNormal;
in vec3 myFragPos;

uniform sampler2D mytexture;
uniform vec3 lightcolor;
uniform vec3 lightPos;
uniform vec3 viewpos;

void main()
{
	float ambientStrength = 0.1;
	vec3 ambientLight = ambientStrength * lightcolor;
	
	vec3 norm = normalize(myNormal);
	vec3 lightDir = normalize(lightPos - myFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightcolor;
	
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewpos - myFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1024);
	vec3 specular = specularStrength * spec * lightcolor;  
	
	vec3 result = (ambientLight + diffuse + specular) * myColor;
	
	FragColor = texture(mytexture, myTex) * vec4(result, 1.0);
}