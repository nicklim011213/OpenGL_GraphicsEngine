#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <boost/filesystem.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "Model.h"
#include "Camera.h"
#include "Settings.h"
#include "Utilities.h"
#include "Scene.h"

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

	// Shader Setup. each takes a path and a name. The name is the key for the shader hashmaps.
	ShaderBuilder shaderBuilder;
	auto VertexShader = shaderBuilder.BuildVertexShader(boost::filesystem::initial_path() / "Shaders" / "VertColorTex.vs", "Vertex");
	auto FragmentShader = shaderBuilder.BuildFragmentShader(boost::filesystem::initial_path() / "Shaders" / "VertColorTex.fs", "Fragment");
	auto CrateShaderProgram = shaderBuilder.BuildShaderProgram(VertexShader, FragmentShader, "CrateProgram");

	glUseProgram(CrateShaderProgram->GetProgramID());
    CrateShaderProgram->SetUniform3F("mat.ambient", glm::vec3(0.2125f, 0.1275f, 0.054f));
    CrateShaderProgram->SetUniform3F("mat.diffuse", glm::vec3(0.714f, 0.4284f, 0.18144f));
    CrateShaderProgram->SetUniform3F("mat.specular", glm::vec3(0.393548f, 0.271906f, 0.166721f));
    CrateShaderProgram->SetUniform1F("mat.shininess", 0.2f);


	auto LightFragShader = shaderBuilder.BuildFragmentShader(boost::filesystem::initial_path() / "Shaders" / "light.fs", "LightFragment");
	auto LightShaderProgram = shaderBuilder.BuildShaderProgram(VertexShader, LightFragShader, "LightProgram");
    auto CrateTexture = shaderBuilder.BuildTexture(boost::filesystem::initial_path() / "Textures" / "container.jpg", "CrateTexture");
    // End of Shader Setup


    Model model("BoxStd2.obx");
    model.SetPosition(0.0f, 0.0f, -2.0f);
	model.SetShaderProgram(ShaderProgramPool["CrateProgram"]);
	model.SetTexture("CrateTexture");

    Model LightBox("BoxStd2.obx");
    LightBox.SetPosition(3.0f, 0.0f, 1.0f);
	LightBox.SetShaderProgram(ShaderProgramPool["LightProgram"]);

    Scene scene;
    scene.AddModel(LightBox);
    scene.AddModel(model);
    scene.AddGlobalUniform("3F", std::make_shared<glm::vec3>(1.0f, 0.0f, 1.0f), "lightcolor");
    scene.AddGlobalUniform("3F", std::make_shared<glm::vec3>(LightBox.GetModelMatrix()[3]), "lightPos");
    scene.AddGlobalUniform("4FV", std::make_shared<glm::mat4>(model.GetProjectionMatrix()), "projection");
	scene.AddGlobalUniform("3F", std::make_shared<glm::vec3>(0.2f, 0.2f, 0.2f), "light.ambient");
	scene.AddGlobalUniform("3F", std::make_shared<glm::vec3>(0.5f, 0.5f, 0.5f), "light.diffuse");
	scene.AddGlobalUniform("3F", std::make_shared<glm::vec3>(1.0f, 1.0f, 1.0f), "light.specular");
    scene.AddFrameUniforms("3F", std::make_shared<glm::vec3>(camera.Position.x, camera.Position.y, camera.Position.z), "viewpos");
	scene.FinalizeAll();

    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window, camera);

        scene.RecalculateCamera(camera.Position);
        scene.UpdateFrameUniforms();

		for (auto& model : scene.GetModels())
		{
            glUseProgram(model->GetShaderProgram()->GetProgramID());

			model->BindTextures();

			model->SetView(camera.Position, camera.Facing, camera.Up);
            model->GetShaderProgram()->SetUniform4FV("model", glm::value_ptr(model->GetModelMatrix()));
            model->GetShaderProgram()->SetUniform4FV("view", glm::value_ptr(model->GetViewMatrix()));

            glBindVertexArray(model->GetRawModel().GetVAO());
            glDrawElements(GL_TRIANGLES, model->GetRawModel().GetIndices().size(), GL_UNSIGNED_INT, 0);
		}
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}