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


class Model
{
	unsigned int VAO, VBO, EBO;
	unsigned int shaderProgram;
	std::string TextureID;

	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;

	std::string VertShader;
	std::string FragShader;

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

public:

	Model(std::string FilePath);

	void Finalize();

	std::string FilePathToFileData(const boost::filesystem::path& path);

	std::string GetTextureID() {
		return TextureID;
	}

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
#endif
