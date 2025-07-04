#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "Settings.h"


class Camera
{
	public:
		glm::vec3 Position = glm::vec3(0.0f,0.0f,3.0f);
		glm::vec3 Facing = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
		float cameraSpeed = 2.0f;
		float yaw = -90.0f;
		float pitch = 0.0f;
		float lastY = Settings::GetInstance()->SCREEN_HEIGHT / 2.0f;
		float lastX = Settings::GetInstance()->SCREEN_WIDTH / 2.0f;
		bool FirstMouse = true;

		void RecalculateDirection()
		{
			Facing.x = cos(glm::radians(yaw));
			Facing.y = sin(glm::radians(pitch));
			Facing.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		}
};

class Prespective
{
	public:
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);

		void SetView(glm::vec3 position, glm::vec3 facing, glm::vec3 up)
		{
			view = glm::lookAt(position, position + facing, up);
		}
};

#endif
