#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <boost/filesystem.hpp>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "ResourcePool.h"
#include "Model.h"
#include "Camera.h"
#include "Settings.h"
#include "Utilities.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	auto Settings = Settings::GetInstance();
	Settings->SetScreenSize(width, height);
}

void processInput(GLFWwindow* window, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
	float cameraSpeed = camera.cameraSpeed * Utilities::GetInstance()->GetFrameTime();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.Position += cameraSpeed * camera.Facing;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Position -= cameraSpeed * camera.Facing;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Position -= glm::normalize(glm::cross(camera.Facing, camera.Up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Position += glm::normalize(glm::cross(camera.Facing, camera.Up)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (cam->FirstMouse)
    {
        cam->lastX = xpos;
        cam->lastY = ypos;
        cam->FirstMouse = false;
    }

    float xoffset = xpos - cam->lastX;
    float yoffset = cam->lastY - ypos;
    cam->lastX = xpos;
    cam->lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if (cam->pitch > 89.0f)
        cam->pitch = 89.0f;
    if (cam->pitch < -89.0f)
        cam->pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    direction.y = sin(glm::radians(cam->pitch));
    direction.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    cam->Facing = glm::normalize(direction);

}

int main()
{
	//Create Util and setting Singleton instances
	Settings* settings = Settings::GetInstance();
	Utilities* utilities = Utilities::GetInstance();

    boost::filesystem::current_path();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(settings->SCREEN_WIDTH, settings->SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    glViewport(0, 0, settings->SCREEN_WIDTH, settings->SCREEN_HEIGHT);


    Camera camera;
    Prespective perspective;
    glfwSetWindowUserPointer(window, &camera);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	perspective.SetView(camera.Position, camera.Facing, camera.Up);

	Model model("BoxStd2.obx");
	model.Finalize();

	int ModelUniformLocation = glGetUniformLocation(model.GetShaderProgram(), "model");
	int ViewUniformLocation = glGetUniformLocation(model.GetShaderProgram(), "view");
	int ProjectionUniformLocation = glGetUniformLocation(model.GetShaderProgram(), "projection");


    while (!glfwWindowShouldClose(window))
    {
        processInput(window, camera);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        perspective.SetView(camera.Position, camera.Facing, camera.Up);
        glUniformMatrix4fv(ModelUniformLocation, 1, GL_FALSE, glm::value_ptr(perspective.model));
        glUniformMatrix4fv(ViewUniformLocation, 1, GL_FALSE, glm::value_ptr(perspective.view));
        glUniformMatrix4fv(ProjectionUniformLocation, 1, GL_FALSE, glm::value_ptr(perspective.projection));

		glBindVertexArray(model.GetVAO());
		glDrawElements(GL_TRIANGLES, model.GetIndices().size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}