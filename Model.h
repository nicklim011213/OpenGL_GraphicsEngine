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

class ModelPopulator
{
public:

	void ReadFile(std::string FilePath, RawModel& RawModelData, Shader& RawShaderData);
};

class RawModel
{
	unsigned int VAO, VBO, EBO;
	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;

public:

	int GetVertsize()
	{
		return Vertices.size() / 8;
	}

	int GetTriangles()
	{
		return Indices.size() / 3;
	}

	std::vector<unsigned int> GetIndices() {
		return Indices;
	}

	int GetVAO() {
		return VAO;
	}

	void Finalize();

	void AddVertex(float Value)
	{
		Vertices.push_back(Value);
	}

	void AddIndex(unsigned int Value)
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

	int GetShaderProgram() {
		return shaderProgram;
	}

	void SetShaderVertex(std::string name) {
		auto VShaderPath = boost::filesystem::initial_path();
		VShaderPath /= "Shaders";
		VShaderPath /= name;
		auto VshaderContents = FilePathToFileData(VShaderPath);
		VertShader = VshaderContents;
	}

	void SetShaderFragment(std::string name) {
		auto FShaderPath = boost::filesystem::initial_path();
		FShaderPath /= "Shaders";
		FShaderPath /= name;
		auto FShaderContents = FilePathToFileData(FShaderPath);
		FragShader = FShaderContents;
	}

	std::string GetTextureID() {
		return TextureID;
	}

	void SetTextureID(std::string ID) {
		TextureID = ID;
	}

	void FinishShaderSetup();

	void SetUniform4FV(std::string Location, const float* Value)
	{
		int LocationID = glGetUniformLocation(shaderProgram, Location.c_str());
		glUniformMatrix4fv(LocationID, 1, GL_FALSE, Value);
	}

	void SetUniform3F(std::string Location, float x, float y, float z)
	{
		int LocationID = glGetUniformLocation(shaderProgram, Location.c_str());
		glUniform3f(LocationID, x, y, z);
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

	Model(std::string FileName)
	{
		ModelPopulator InternalPopulator;
		RawModel RawModelData;
		Shader Shader;
		InternalPopulator.ReadFile(FileName, RawModelData, Shader);
		ModelData = RawModelData;
		ModelShader = Shader;
	}

	void SetPosition(float x, float y, float z) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	}

	void SetView(glm::vec3 position, glm::vec3 facing, glm::vec3 up)
	{
		view = glm::lookAt(position, position + facing, up);
	}
	
	glm::mat4 GetView() const
	{
		return view;
	}

	glm::mat4 GetProjection() const
	{
		return projection;
	}

	glm::mat4 GetModel() const
	{
		return model;
	}

	Shader& GetShader()
	{
		return ModelShader;
	}

	RawModel& GetRawModel()
	{
		return ModelData;
	}

};

#endif
