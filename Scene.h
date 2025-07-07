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
	std::vector<Model> SceneModels;
	std::vector<SceneUniform> SceneUniforms;

public:

	void AddModel(Model& model) {
		SceneModels.push_back(model);
	}

	std::vector<Model> GetModels() {
		return SceneModels;
	}

	void FinalizeScene()
	{
		for (auto& model : SceneModels) {
			model.GetShader().FinishShaderSetup();
			model.GetRawModel().Finalize();
		}
	}
};
#endif