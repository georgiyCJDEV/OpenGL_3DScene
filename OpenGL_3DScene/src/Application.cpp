// STD includes
#include <iostream>
#include <string>
#include <format>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

// My includes
#include "GLDebug.h"
#include "Renderer.h"
#include "Shader.h"
#include "GLApp.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Model.h"
#include "CubemapTex.h"

enum MoveDirection
{
	FORW,
	BACKW,
	LT,
	RT
};

Camera camera(glm::vec3(-36.417f, 35.579f, 46.015f));

const unsigned int WIDTH = 800, HEIGHT = 600;
int WINDOW_WIDTH, WINDOW_HEIGHT;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool keys[1024];
bool firstMouse = true;

bool fixCamera = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool noTextureFlag;

int movementSpeed = 15.0f;

void processKeyboard(MoveDirection, float&);
void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void doCameraMovement();
void doObjMovement(float&);

int main()
{
	GLFWwindow* window;
	GLFWmonitor* monitor;
	const GLFWvidmode* vmode;
	
	if (!GLApp::init(&window, &monitor, &vmode, "OpenGL_4.6 Coursework"))
	{
		std::cout << "Error initializing stage." << std::endl;
		return -1;
	}
	WINDOW_WIDTH = GLApp::getWindowWidth();
	WINDOW_HEIGHT = GLApp::getWindowHeight();

	lastX = WINDOW_WIDTH / 2.0f;
	lastY = WINDOW_HEIGHT / 2.0f;

	glfwSetWindowSizeCallback(window, glfw_window_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	{
		// Loading model
		// Load model
		Model streetModel("res/models/fullstreedemo/fullstreedemo.obj");
		Model carModel("res/models/car/car.obj");

		// Load shader
		Shader shaderModel("res/shaders/Model.shader");
		Shader shaderModelNoTex("res/shaders/ModelNoTex.shader");
		Shader shadersArr[] = {shaderModel,shaderModelNoTex};

		noTextureFlag = true;

		float lamp_positions[]
		{
			// BACK
			-0.5f, -0.5f, -0.5f, // 0
			 0.5f, -0.5f, -0.5f, // 1
			 0.5f,  0.5f, -0.5f, // 2 
			 0.5f,  0.5f, -0.5f, // 3 
			-0.5f,  0.5f, -0.5f, // 4 
			-0.5f, -0.5f, -0.5f, // 5 

			// FRONT
			-0.5f, -0.5f,  0.5f, // 6
			 0.5f, -0.5f,  0.5f, // 7
			 0.5f,  0.5f,  0.5f, // 8
			 0.5f,  0.5f,  0.5f, // 9
			-0.5f,  0.5f,  0.5f, // 10
			-0.5f, -0.5f,  0.5f, // 11

			// LEFT
			-0.5f,  0.5f,  0.5f, // 12
			-0.5f,  0.5f, -0.5f, // 13 
			-0.5f, -0.5f, -0.5f, // 14 
			-0.5f, -0.5f, -0.5f, // 15 
			-0.5f, -0.5f,  0.5f, // 16 
			-0.5f,  0.5f,  0.5f, // 17 

			// RIGHT
			0.5f,  0.5f,  0.5f, // 18
			0.5f,  0.5f, -0.5f, // 19
			0.5f, -0.5f, -0.5f, // 20
			0.5f, -0.5f, -0.5f, // 21
			0.5f, -0.5f,  0.5f, // 22
			0.5f,  0.5f,  0.5f, // 23

			// BOTTOM
		   -0.5f, -0.5f, -0.5f, // 24
			0.5f, -0.5f, -0.5f, // 25 
			0.5f, -0.5f,  0.5f, // 26 
			0.5f, -0.5f,  0.5f, // 27 
		   -0.5f, -0.5f,  0.5f, // 28 
		   -0.5f, -0.5f, -0.5f, // 29

		   // TOP
		   -0.5f,  0.5f, -0.5f, // 30
			0.5f,  0.5f, -0.5f, // 31
			0.5f,  0.5f,  0.5f, // 32 
			0.5f,  0.5f,  0.5f, // 33 
		   -0.5f,  0.5f,  0.5f, // 34 
		   -0.5f,  0.5f, -0.5f, // 35
		};

		VertexBuffer vbLamp(&lamp_positions, 6 * 6 * 3 * sizeof(float));

		VertexBufferLayout layoutLamp;
		layoutLamp.push<float>(3);

		VertexArray vaLamp;
		vaLamp.addBuffer(vbLamp, layoutLamp);

		// Creating light sources
		Shader shaderLamp("res/shaders/Lamp.shader");

		glm::vec3 pointLightPositions[]
		{
			glm::vec3(6.504f,10.569f,-16.098f),
			glm::vec3(16.224f,15.634f,-4.779f),
			glm::vec3(-6.785f,15.044f,-3.127f),
			glm::vec3(8.555f,15.634f,18.584f),
			glm::vec3(30.973f,15.634f,18.584f),
			glm::vec3(39.233f,15.634f,-4.779f),
			glm::vec3(-29.204f,15.044f,-3.009f),
			glm::vec3(-39.233f,10.914f,36.136f),
			glm::vec3(3.245f,10.324f,37.316f),
		};

		float skyboxVertices[] = {
			// Positions
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		VertexBuffer vbSkyBox(&skyboxVertices, 6 * 6 * 3 * sizeof(float));

		VertexBufferLayout layoutSBox;
		layoutSBox.push<float>(3);

		VertexArray vaSBox;
		vaSBox.addBuffer(vbSkyBox, layoutSBox);

		std::vector<const char*> faces;
		faces.push_back("res/images/skybox/posx.jpg");
		faces.push_back("res/images/skybox/negx.jpg");
		faces.push_back("res/images/skybox/posy.jpg");
		faces.push_back("res/images/skybox/negy.jpg");
		faces.push_back("res/images/skybox/posz.jpg");
		faces.push_back("res/images/skybox/negz.jpg");
		unsigned int cubemapTexture = CubemapTex::loadCubemapTex(faces);

		Shader shaderSkybox("res/shaders/Skybox.shader");

		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

		glm::mat4 view;

		camera.getFront() = glm::vec3(0.634f, -0.427f, -0.645f);
		glm::vec3 streetModelTranslation(0.0f, -1.75f, 2.0f);
		glm::vec3 carModelTranslation(2.065f, -0.885f, 6.785f);
		glm::vec3 carModelScale(5.487f, 5.487f, 5.487f);

		/* ImGui */
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			// Set frame time
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			view = camera.getViewMatrix();

			renderer.clear();

			ImGui_ImplGlfwGL3_NewFrame();

			{
				// Projection, view & model matrices setup
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, streetModelTranslation);
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

				// Setup fs uniforms and draw model
				streetModel.draw(shaderModel,
					projection, view, model,
					camera.getPosition().x, camera.getPosition().y, camera.getPosition().z,
					pointLightPositions,
					camera.getFront().x, camera.getFront().y, camera.getFront().z
				);
			}

			{
				// Projection, view & model matrices setup
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, carModelTranslation);
				model = glm::scale(model, carModelScale);

				// Setup fs uniforms and draw model
				carModel.draw(shadersArr,
					noTextureFlag,
					projection, view, model,
					camera.getPosition().x, camera.getPosition().y, camera.getPosition().z,
					pointLightPositions,
					camera.getFront().x, camera.getFront().y, camera.getFront().z
				);
			}

			{
				shaderLamp.setUniformMat4f("u_Projection", projection);
				shaderLamp.setUniformMat4f("u_View", view);
				for (unsigned int i = 0; i < 9; i++)
				{
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, pointLightPositions[i]);
					model = glm::scale(model, glm::vec3(0.2f));
					shaderLamp.setUniformMat4f("u_Model", model);

					renderer.draw(GL_TRIANGLES, 0, 36, vaLamp, shaderLamp);
				}
			}

			{
				glDepthFunc(GL_LEQUAL);

				shaderSkybox.bind();
				glm::mat4 view = glm::mat4(1.0f);
				view = glm::mat4(glm::mat3(glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getFront(), camera.getCameraUp())));

				shaderSkybox.setUniformMat4f("u_Projection", projection);
				shaderSkybox.setUniformMat4f("u_View", view);

				renderer.draw(GL_TRIANGLES, 0, 36, vaSBox, shaderSkybox, cubemapTexture);
			}

			glDepthFunc(GL_LESS);

			{
				ImGui::Text("Camera translation");
				ImGui::SliderFloat("Camera translation X", &camera.getPosition().x, -100.0f, 100.0f);
				ImGui::SliderFloat("Camera translation Y", &camera.getPosition().y, -100.0f, 100.0f);
				ImGui::SliderFloat("Camera translation Z", &camera.getPosition().z, -100.0f, 100.0f);
				ImGui::SliderFloat("Camera fx", &camera.getFront().x, -100.0f, 100.0f);
				ImGui::SliderFloat("Camera fy", &camera.getFront().y, -100.0f, 100.0f);
				ImGui::SliderFloat("Camera fz", &camera.getFront().z, -100.0f, 100.0f);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window); // Swap front and back buffers (Двойная буферизация)

			glfwPollEvents();

			glfwSetKeyCallback(window, key_callback);

			doObjMovement(carModelTranslation.x);

			if (!fixCamera)
			{
				doCameraMovement();
			}

			GLCall(glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
		}
	}

		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
		GLApp::terminate();
	return 0;
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (!fixCamera)
	{
		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		float xOffset = xPos - lastX;
		float yOffset = lastY - yPos;

		lastX = xPos;
		lastY = yPos;

		camera.processMouseMovement(xOffset, yOffset);
	}
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		const GLFWvidmode* vmode = GLApp::getVmode();
		glfwSetWindowMonitor(window, NULL, WIDTH, HEIGHT, WIDTH, HEIGHT, 60);
	}
	else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		const GLFWvidmode* vmode = GLApp::getVmode();
		GLFWmonitor* monitor = GLApp::getMonitor();
		glfwSetWindowMonitor(window, monitor, GLApp::monitorLeftCornerX, GLApp::monitorLeftCornerY, vmode->width, vmode->height, 60);
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		switch (fixCamera)
		{
		case true:
			fixCamera = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			break;
		default:
			fixCamera = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			break;
		}
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void doCameraMovement()
{
	if (keys[GLFW_KEY_UP])
	{
		camera.processKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_DOWN])
	{
		camera.processKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_LEFT])
	{
		camera.processKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_RIGHT])
	{
		camera.processKeyboard(RIGHT, deltaTime);
	}
}

void doObjMovement(float& position)
{
	if (keys [GLFW_KEY_W] && position>-35.898)
	{
		processKeyboard(FORW,position);
	}

	if (keys[GLFW_KEY_S] && position<30.418)
	{
		processKeyboard(BACKW,position);
	}
}

void processKeyboard(MoveDirection direction, float& position)
{
	GLfloat velocity = movementSpeed * deltaTime;

	if (direction == FORW)
	{
		position -= velocity;
	}

	if (direction == BACKW)
	{
		position += velocity;
	}
}