#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Model.h"

struct SceneUniform
{
	std::string type;
	std::shared_ptr<void> Value;
	std::string Location;

	SceneUniform(std::string ty, std::shared_ptr<void> Val, std::string Loc)
	{
		type = ty;
		Value = Val;
		Location = Loc;
	}
};

class Scene {
	std::vector<Model*> SceneModels;
	std::vector<SceneUniform> GlobalSceneUniforms;
	std::vector<SceneUniform> FrameSceneUniforms;

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

	void SetGlobalSceneUniforms(Model* model)
	{
		for (auto& uniform : GlobalSceneUniforms) {
			if (uniform.type == "3F") {
				glm::vec3 vecValue = *std::static_pointer_cast<glm::vec3>(uniform.Value);
				model->GetShader().SetUniform3F(uniform.Location, vecValue);
			}
			else if (uniform.type == "4FV")
			{
				glm::mat4 matValue = *std::static_pointer_cast<glm::mat4>(uniform.Value);
				model->GetShader().SetUniform4FV(uniform.Location, glm::value_ptr(matValue));
			}
		}
	}

	void AddGlobalUniform(std::string type, std::shared_ptr<void> value, std::string location) {
		SceneUniform uniform(type, value, location);
		GlobalSceneUniforms.push_back(uniform);
	}

	void FinalizeGlobalSceneUniforms()
	{
		for (auto& model : SceneModels)
		{
			glUseProgram(model->GetShader().GetShaderProgram());
			this->SetGlobalSceneUniforms(model);
		}
	}

	void AddFrameUniforms(std::string type, std::shared_ptr<void> value, std::string location)
	{
		SceneUniform uniform(type, value, location);
		FrameSceneUniforms.push_back(uniform);
	}

	void UpdateFrameUniforms()
	{
		for (auto& Model : SceneModels) {

			glUseProgram(Model->GetShader().GetShaderProgram());

			for (auto& uniform : FrameSceneUniforms) {
				if (uniform.type == "3F") {
					glm::vec3 vecValue = *std::static_pointer_cast<glm::vec3>(uniform.Value);
					Model->GetShader().SetUniform3F(uniform.Location, vecValue);
				}
				else if (uniform.type == "4FV")
				{
					glm::mat4 matValue = *std::static_pointer_cast<glm::mat4>(uniform.Value);
					Model->GetShader().SetUniform4FV(uniform.Location, glm::value_ptr(matValue));
				}
			}
		}
	}

	void RecalculateCamera(const glm::vec3& cameraPosition)
	{
		for (auto& uniform : FrameSceneUniforms)
		{
			if (uniform.type == "3F" && uniform.Location == "viewpos")
			{
				auto vecPtr = std::static_pointer_cast<glm::vec3>(uniform.Value);
				*vecPtr = cameraPosition;
			}
			// add other uniforms as needed, or leave them alone here
		}
	}

	void FinalizeAll()
	{
		FinalizeScene();
		FinalizeGlobalSceneUniforms();
		UpdateFrameUniforms();
	}
};
#endif