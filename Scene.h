#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Model.h"

struct SceneUniform
{
	std::string type;
	void* Value;
	std::string Location;

	SceneUniform(std::string ty, void* Val, std::string Loc)
	{
		type = ty;
		Value = Val;
		Location = Loc;
	}
};

class Scene {
	std::vector<Model*> SceneModels;
	std::vector<SceneUniform> SceneUniforms;

public:

	void AddModel(Model& model) {
		SceneModels.push_back(&model);
	}

	std::vector<Model*> GetModels() {
		return SceneModels;
	}

	void FinalizeScene()
	{
		for (auto& model : SceneModels) {
			model->GetShader().FinishShaderSetup();
			model->GetRawModel().Finalize();
		}
	}

	void SetSceneUniforms(Model* model)
	{
		for (auto& uniform : SceneUniforms) {
			if (uniform.type == "3F") {
				glm::vec3* vecValue = static_cast<glm::vec3*>(uniform.Value);
				model->GetShader().SetUniform3F(uniform.Location, *vecValue);
			}
			else if (uniform.type == "4FV")
			{
				glm::mat4* matValue = static_cast<glm::mat4*>(uniform.Value);
				model->GetShader().SetUniform4FV(uniform.Location, glm::value_ptr(*matValue));
			}
		}
	}

	void AddUniform(std::string type, void* value, std::string location) {
		SceneUniform uniform(type, value, location);
		SceneUniforms.push_back(uniform);
	}
};
#endif