#include "Model.h"
#include <glad.h>
#include <glfw3.h>

void Model::ReadFile(std::string FilePath, RawModel& RawModelData)
{
	auto RootPath = boost::filesystem::initial_path();
	auto ModelPath = RootPath / "Models";
	ModelPath /= FilePath;

	auto FileContents = FilePathToFileData(ModelPath);

	std::vector<std::string> lines;
	boost::algorithm::replace_all(FileContents, "\r\n", "\n");
	boost::algorithm::split(lines, FileContents, boost::is_any_of("\n"));

	for (std::string line : lines)
	{
		if (line[0] == '@')
		{
			line = line.substr(1);
			std::vector<std::string> parts;
			boost::algorithm::split(parts, line, boost::is_any_of(","));
			std::vector<float> Temp;

			switch (parts.size())
			{
				case 11:
				{
					float Xn = boost::lexical_cast<float>(parts[8]);
					float Yn = boost::lexical_cast<float>(parts[9]);
					float Zn = boost::lexical_cast<float>(parts[10]);
					Temp.push_back(Xn);
					Temp.push_back(Yn);
					Temp.push_back(Zn);
				}
				case 8:
				{
					float R = boost::lexical_cast<float>(parts[5]);
					float G = boost::lexical_cast<float>(parts[6]);
					float B = boost::lexical_cast<float>(parts[7]);
					Temp.push_back(R);
					Temp.push_back(G);
					Temp.push_back(B);
				}
				case 5:
				{
					float Tex_Y = boost::lexical_cast<float>(parts[4]);
					float Tex_X = boost::lexical_cast<float>(parts[3]);
					Temp.push_back(Tex_Y);
					Temp.push_back(Tex_X);
				}
				case 3:
				{
					float X = boost::lexical_cast<float>(parts[0]);
					float Y = boost::lexical_cast<float>(parts[1]);
					float Z = boost::lexical_cast<float>(parts[2]);
					Temp.push_back(X);
					Temp.push_back(Y);
					Temp.push_back(Z);
					break;
				}
			}
			for (auto i = Temp.rbegin(); i != Temp.rend(); ++i)
			{
				RawModelData.AddVertex(*i);
			}
		}
		else if (line[0] == '$')
		{
			line = line.substr(1);
			std::vector<std::string> parts;
			boost::algorithm::split(parts, line, boost::is_any_of(","));
			for (std::string part : parts)
			{
				int Index = boost::lexical_cast<int>(part);
				RawModelData.AddIndex(Index);
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