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


class Model
{
	unsigned int VAO, VBO, EBO;
	unsigned int shaderProgram;
	std::string TextureID;

	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;

	std::string VertShader;
	std::string FragShader;

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
};

#endif
