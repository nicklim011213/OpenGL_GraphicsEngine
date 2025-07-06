#include "Model.h"
#include <glad.h>
#include <glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "ResourcePool.h"

void ModelPopulator::ReadFile(std::string FilePath, RawModel& RawModelData, Shader& RawShaderData)
{
	auto RootPath = boost::filesystem::initial_path();
	auto ModelPath = RootPath / "Models";
	ModelPath /= FilePath;

	auto FileContents = FilePathToFileData(ModelPath);

	std::vector<std::string> lines;
	boost::algorithm::replace_all(FileContents, "\r\n", "\n");
	boost::algorithm::split(lines, FileContents, boost::is_any_of("\n"));

	for (auto line : lines)
	{
		if (line[0] == '@')
		{
			line = line.substr(1);
			std::vector<std::string> parts;
			boost::algorithm::split(parts, line, boost::is_any_of(","));
			if (parts.size() >= 3)
			{
				float X = boost::lexical_cast<float>(parts[0]);
				float Y = boost::lexical_cast<float>(parts[1]);
				float Z = boost::lexical_cast<float>(parts[2]);
				RawModelData.AddVertex(X);
				RawModelData.AddVertex(Y);
				RawModelData.AddVertex(Z);
			}
			if (parts.size() >= 5)
			{
				float Tex_X = boost::lexical_cast<float>(parts[3]);
				float Tex_Y = boost::lexical_cast<float>(parts[4]);
				RawModelData.AddVertex(Tex_X);
				RawModelData.AddVertex(Tex_Y);
			}
			if (parts.size() >= 8)
			{
				float R = boost::lexical_cast<float>(parts[5]);
				float G = boost::lexical_cast<float>(parts[6]);
				float B = boost::lexical_cast<float>(parts[7]);
				RawModelData.AddVertex(R);
				RawModelData.AddVertex(G);
				RawModelData.AddVertex(B);
			}
			if (parts.size() >= 11)
			{
				float Xn = boost::lexical_cast<float>(parts[8]);
				float Yn = boost::lexical_cast<float>(parts[9]);
				float Zn = boost::lexical_cast<float>(parts[10]);
				RawModelData.AddVertex(Xn);
				RawModelData.AddVertex(Yn);
				RawModelData.AddVertex(Zn);
			}
		}
		else if (line[0] == '$')
		{
			line = line.substr(1);
			std::vector<std::string> parts;
			boost::algorithm::split(parts, line, boost::is_any_of(","));
			for (auto part : parts)
			{
				int Index = boost::lexical_cast<int>(part);
				RawModelData.AddIndex(Index);
			}
		}
		else if (boost::algorithm::contains(line, "VShader ="))
		{
			line = line.substr(9);
			RawShaderData.SetShaderVertex(line);

		}
		else if (boost::algorithm::contains(line, "FShader ="))
		{
			line = line.substr(9);
			RawShaderData.SetShaderFragment(line);
		}
		else if (boost::algorithm::contains(line, "Texture ="))
		{
			line = line.substr(9);
			auto TexturePath = RootPath / "Textures";
			TexturePath /= line;
			if (TexturePool.find(line) == TexturePool.end())
			{
				int width, height, nrChannels;
				unsigned char* data = stbi_load(TexturePath.string().c_str(), &width, &height, &nrChannels, 0);
				TextureDataEntry Entry(data, width, height, nrChannels, (unsigned int)0);
				glGenTextures(1, &Entry.TextureID);
				glBindTexture(GL_TEXTURE_2D, Entry.TextureID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				RawShaderData.SetTextureID(line);
				TexturePool[line] = Entry;
			}
		}
	}
}

void RawModel::Finalize()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);

	this->VAO = VAO;
	this->VBO = VBO;
	this->EBO = EBO;
}

void Shader::FinishShaderSetup()
{
	const char* VertShaderSource = VertShader.c_str();
	const char* FragShaderSource = FragShader.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VertShaderSource, NULL);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FragShaderSource, NULL);
	glCompileShader(fragmentShader);
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	this->shaderProgram = shaderProgram;
	glUseProgram(shaderProgram);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	auto TextData = TexturePool[TextureID];
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextData.Width, TextData.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextData.RawData);
	glGenerateMipmap(GL_TEXTURE_2D);
}