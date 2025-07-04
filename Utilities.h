#ifndef UTILITIES_H
#define UTILITIES_H

#include <glad.h>
#include <glfw3.h>

class Utilities
{
private:
	static Utilities* instance;
	Utilities() {};
	Utilities(const Utilities&) = delete;
	Utilities& operator=(const Utilities&) = delete;

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

public:
	static Utilities* GetInstance()
	{
		if (!instance)
			instance = new Utilities();
		return instance;
	}

	float GetFrameTime()
	{
		currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		return deltaTime;
	}

};

#endif