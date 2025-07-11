#ifndef MODEL_H
#define MODEL_H
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

#include "ResourcePool.h"

class Model;
class Shader;
class RawModel;
class ModelPopulator;

inline std::string FilePathToFileData(const boost::filesystem::path& path)
{
	boost::filesystem::ifstream File(path.string());
	std::stringstream FileStream;
	FileStream << File.rdbuf();
	std::string FileContents = FileStream.str();
	File.close();
	return FileContents;
}

class RawModel
{
	unsigned int VAO, VBO, EBO;
	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;

public:

	inline int GetVertsize()
	{
		return Vertices.size() / 8;
	}

	inline int GetTriangles()
	{
		return Indices.size() / 3;
	}

	inline std::vector<unsigned int> GetIndices() {
		return Indices;
	}

	inline int GetVAO() {
		return VAO;
	}

	void Finalize();

	inline void AddVertex(float Value)
	{
		Vertices.push_back(Value);
	}

	inline void AddIndex(unsigned int Value)
	{
		Indices.push_back(Value);
	}
};

class Shader
{
	std::string VertShader;
	std::string FragShader;
	unsigned int shaderProgram;
	std::string TextureID;

public:

	inline int GetShaderProgram() {
		return shaderProgram;
	}

	inline void SetShaderVertex(std::string name) {
		auto VShaderPath = boost::filesystem::initial_path();
		VShaderPath /= "Shaders";
		VShaderPath /= name;
		auto VshaderContents = FilePathToFileData(VShaderPath);
		VertShader = VshaderContents;
	}

	inline void SetShaderFragment(std::string name) {
		auto FShaderPath = boost::filesystem::initial_path();
		FShaderPath /= "Shaders";
		FShaderPath /= name;
		auto FShaderContents = FilePathToFileData(FShaderPath);
		FragShader = FShaderContents;
	}

	inline std::string GetTextureID() {
		return TextureID;
	}

	inline void SetTextureID(std::string ID) {
		TextureID = ID;
	}

	void FinishShaderSetup();

	inline void SetUniform4FV(std::string Location, const float* Value)
	{
		int LocationID = glGetUniformLocation(shaderProgram, Location.c_str());
		glUniformMatrix4fv(LocationID, 1, GL_FALSE, Value);
	}

	inline void SetUniform3F(std::string Location, float x, float y, float z)
	{
		int LocationID = glGetUniformLocation(shaderProgram, Location.c_str());
		glUniform3f(LocationID, x, y, z);
	}

	inline void SetUniform3F(std::string Location, glm::vec3 Value)
	{
		int LocationID = glGetUniformLocation(shaderProgram, Location.c_str());
		glUniform3f(LocationID, Value.x, Value.y, Value.z);
	}

	inline void BindTexture()
	{
		if (this->GetTextureID() != "")
		{
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			auto TextData = TexturePool[this->GetTextureID()];
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextData.Width, TextData.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextData.RawData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
};

class Model
{
	Shader ModelShader;
	RawModel ModelData;

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

public:

	inline Model(std::string FileName)
	{
		RawModel RawModelData;
		Shader Shader;
		ReadFile(FileName, RawModelData, Shader);
		ModelData = RawModelData;
		ModelShader = Shader;
	}

	inline void SetPosition(float x, float y, float z) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	}

	inline void SetView(glm::vec3 position, glm::vec3 facing, glm::vec3 up)
	{
		view = glm::lookAt(position, position + facing, up);
	}
	
	inline glm::mat4 GetViewMatrix() const
	{
		return view;
	}

	inline glm::mat4 GetProjectionMatrix() const
	{
		return projection;
	}

	inline glm::mat4 GetModelMatrix() const
	{
		return model;
	}

	inline Shader& GetShader()
	{
		return ModelShader;
	}

	inline RawModel& GetRawModel()
	{
		return ModelData;
	}

	void ReadFile(std::string FilePath, RawModel& RawModelData, Shader& RawShaderData);

	void AddTextureEntry(std::string Name, boost::filesystem::path TexturePath);

};

#endif
