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
class RawModel;

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

class Model
{
	std::shared_ptr<ShaderProgram> ModelShader;
	RawModel ModelData;
	std::string Texture;

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

public:

	inline Model(std::string FileName)
	{
		RawModel RawModelData;
		ReadFile(FileName, RawModelData);
		ModelData = RawModelData;
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

	inline RawModel& GetRawModel()
	{
		return ModelData;
	}

	void ReadFile(std::string FilePath, RawModel& RawModelData);

	inline std::shared_ptr<ShaderProgram> GetShaderProgram()
	{
		return ModelShader;
	}

	inline void SetShaderProgram(std::shared_ptr<ShaderProgram> Shader)
	{
		ModelShader = Shader;
	}

	inline void BindTextures()
	{
		if (TexturePool.find(Texture) != TexturePool.end())
		{
			TexturePool[Texture]->Bind();
		}
		else if (Texture.empty())
		{
			// Ignored.
		}
		else
		{
			std::cerr << "Texture not found in pool: " << Texture << std::endl;
		}
	}

	inline void SetTexture(std::string TextureName)
	{
		Texture = TextureName;
	}
};

#endif
