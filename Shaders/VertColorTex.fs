#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

out vec4 FragColor;

in vec2 myTex;
in vec3 myNormal;
in vec3 myFragPos;

uniform sampler2D mytexture;
uniform vec3 lightPos;
uniform vec3 viewpos;

uniform Material mat;
uniform Light light; 

void main()
{
	// Ambient	
	vec3 ambientLight = light.ambient * mat.ambient;
	
	// Diffuse
	vec3 norm = normalize(myNormal);
	vec3 lightDir = normalize(lightPos - myFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * mat.diffuse);
	
	// Specular
	vec3 viewDir = normalize(viewpos - myFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), (mat.shininess * 128));
	vec3 specular = light.specular * (spec * mat.specular);  
	
	// Combined
	vec3 result = (ambientLight + diffuse + specular);
	
	FragColor = texture(mytexture, myTex) * vec4(result, 1.0);
}