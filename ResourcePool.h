#ifndef RESOURCEPOOL_H
#define RESOURCEPOOL_H
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <glad.h>
#include <glfw3.h>

#include "stb_image.h"

class Texture
{
	unsigned char* RawData = nullptr;
	int Width = 1, Height = -1, Channels = -1;
	unsigned int TextureID = -1;

public:
	Texture(boost::filesystem::path ImagePath)
	{
		unsigned char* data = stbi_load(ImagePath.string().c_str(), &Width, &Height, &Channels, 0);
		RawData = data;

		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (Channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, RawData);
		else if (Channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, RawData);

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	Texture()
	{

	}

	void Bind() const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);
	}

	void Bind(int TextureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + TextureUnit);
		glBindTexture(GL_TEXTURE_2D, TextureID);
	}

	unsigned int GetTextureID() const
	{
		return TextureID;
	}
};

extern std::unordered_map<std::string, std::shared_ptr<Texture>> TexturePool;

class VertexShader
{
	std::string ShaderCode;
	std::string Name;
	int VertexShaderLocation = 0;

	void Build()
	{
		VertexShaderLocation = glCreateShader(GL_VERTEX_SHADER);
		auto ShaderSource = ShaderCode.c_str();
		glShaderSource(VertexShaderLocation, 1, &ShaderSource, NULL);
		glCompileShader(VertexShaderLocation);
		ErrorCheck();
	}

	void ErrorCheck() const
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(VertexShaderLocation, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(VertexShaderLocation, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

public:

	VertexShader(boost::filesystem::path shaderPath, std::string Name)
	{
		boost::filesystem::ifstream shaderFile(shaderPath);
		std::stringstream buffer;
		buffer << shaderFile.rdbuf();
		ShaderCode = buffer.str();

		this->Name = Name;
		Build();
	}

	std::string GetShaderCode() const
	{
		return ShaderCode;
	}

	int GetShaderLocation() const
	{
		return VertexShaderLocation;
	}
};

extern std::unordered_map<std::string, std::shared_ptr<VertexShader>> VertexShaderPool;

class FragmentShader
{
	std::string ShaderCode;
	std::string Name;
	int FragmentShaderLocation = 0;

	void Build()
	{
		FragmentShaderLocation = glCreateShader(GL_FRAGMENT_SHADER);
		auto ShaderSource = ShaderCode.c_str();
		glShaderSource(FragmentShaderLocation, 1, &ShaderSource, NULL);
		glCompileShader(FragmentShaderLocation);
		ErrorCheck();
	}

	void ErrorCheck() const
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(FragmentShaderLocation, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(FragmentShaderLocation, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

public:
	FragmentShader(boost::filesystem::path shaderPath, std::string Name)
	{
		boost::filesystem::ifstream shaderFile(shaderPath);
		std::stringstream buffer;
		buffer << shaderFile.rdbuf();
		ShaderCode = buffer.str();

		this->Name = Name;
		Build();
	}

	std::string GetShaderCode() const
	{
		return ShaderCode;
	}

	int GetShaderLocation() const
	{
		return FragmentShaderLocation;
	}
};

extern std::unordered_map<std::string, std::shared_ptr<FragmentShader>> FragmentShaderPool;

class ShaderProgram
{
	unsigned int ProgramID;

	std::shared_ptr<VertexShader> InternalVertShsader;
	std::shared_ptr<FragmentShader> InternalFragShader;
	std::string ShaderName;

	void BuildProgram()
	{
		ProgramID = glCreateProgram();
		glAttachShader(ProgramID, InternalVertShsader->GetShaderLocation());
		glAttachShader(ProgramID, InternalFragShader->GetShaderLocation());
		glLinkProgram(ProgramID);
		ErrorCheck();
	}

	void ErrorCheck() const
	{
		int  success;
		char infoLog[512];
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}

public:

	ShaderProgram(VertexShader Vert, FragmentShader Frag)
	{
		auto VertPointer = std::make_shared<VertexShader>(Vert);
		auto FragPointer = std::make_shared<FragmentShader>(Frag);
		InternalVertShsader = VertPointer;
		InternalFragShader = FragPointer;
		BuildProgram();
	}

	ShaderProgram(std::shared_ptr<VertexShader> Vert, std::shared_ptr<FragmentShader> Frag, std::string Name)
		: InternalVertShsader(Vert), InternalFragShader(Frag)
	{
		ShaderName = Name;
		BuildProgram();
	}

	std::shared_ptr<VertexShader> GetVertexShader() const
	{
		return InternalVertShsader;
	}

	std::shared_ptr<FragmentShader> GetFragmentShader() const
	{
		return InternalFragShader;
	}

	inline void SetUniform4FV(std::string Location, const float* Value) const
	{
		int LocationID = glGetUniformLocation(ProgramID, Location.c_str());
		glUniformMatrix4fv(LocationID, 1, GL_FALSE, Value);
	}

	inline void SetUniform3F(std::string Location, glm::vec3 Value) const
	{
		int LocationID = glGetUniformLocation(ProgramID, Location.c_str());
		glUniform3f(LocationID, Value.x, Value.y, Value.z);
	}

	inline void SetUniform1F(std::string Location, float Value) const
	{
		int LocationID = glGetUniformLocation(ProgramID, Location.c_str());
		glUniform1f(LocationID, Value);
	}

	inline void SetUniform1(std::string Location, int Value) const
	{
		int LocationID = glGetUniformLocation(ProgramID, Location.c_str());
		glUniform1i(LocationID, Value);
	}

	inline int GetProgramID() const
	{
		return ProgramID;
	}
};

extern std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgramPool;

class ShaderBuilder
{
public:
	ShaderBuilder()
	{
	}

	std::shared_ptr<VertexShader> BuildVertexShader(boost::filesystem::path path, std::string name)
	{
		auto NewShader = std::make_shared<VertexShader>(path, name);
		VertexShaderPool[name] = NewShader;
		return NewShader;
	}

	std::shared_ptr<FragmentShader> BuildFragmentShader(boost::filesystem::path path, std::string name)
	{
		auto NewShader = std::make_shared<FragmentShader>(path, name);
		FragmentShaderPool[name] = NewShader;
		return NewShader;
	}

	std::shared_ptr<ShaderProgram> BuildShaderProgram(std::shared_ptr<VertexShader> vert, std::shared_ptr<FragmentShader> frag, std::string ProgramName)
	{
		auto NewShaderProgram = std::make_shared<ShaderProgram>(vert, frag, ProgramName);
		ShaderProgramPool[ProgramName] = NewShaderProgram;
		return NewShaderProgram;
	}

	std::shared_ptr<Texture> BuildTexture(boost::filesystem::path TexturePath, std::string Name)
	{
		if (TexturePool.find(Name) == TexturePool.end())
		{
			auto NewTexture = std::make_shared<Texture>(TexturePath);
			TexturePool[Name] = NewTexture;
		}
		return TexturePool[Name];
	}
};

class Material
{
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;

public:

	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
		: Ambient(ambient), Diffuse(diffuse), Specular(specular), Shininess(shininess)
	{

	}
};

extern std::unordered_map<std::string, std::shared_ptr<Material>> MaterialPool;

#endif
