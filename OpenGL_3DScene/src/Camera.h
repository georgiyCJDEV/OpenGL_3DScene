#pragma once

// GLEW
#include <GL/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW        = -90.0f;
const float PITCH      =  0.0f;
const float SPEED      =  20.0f;
const float SENSITIVTY =  0.25f;
const float ZOOM       =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;

	glm::vec3 right;
	glm::vec3 worldUp;

	// Eular Angles
	float yaw;
	float pitch;

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	// Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors();

public:
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    inline glm::mat4 getViewMatrix( ) { return glm::lookAt( position, position + front, up ); }
    
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime);
    
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    
    inline float getZoom() { return zoom; }
    
    inline glm::vec3& getPosition() { return position; }
    
    inline glm::vec3& getFront() { return front; }

    inline glm::vec3 getCameraUp() { return up; }
};
