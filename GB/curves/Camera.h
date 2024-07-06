#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


class Camera
{
public:
	Camera() {}
	~Camera() {}
	void initialize(Shader* shader, int width, int height, float sensitivity, float pitch, float yaw, glm::vec3 cameraFront, glm::vec3 cameraPos, glm::vec3 cameraUp);
	void rotate(GLFWwindow* window, double xpos, double ypos);
	void translate(GLFWwindow* window, int key, int action);
	void update();
	void zoom(GLFWwindow* window, double xoffset, double yoffset);

protected:
	Shader* shader;
	int width, height;
	bool firstMouse, rotateX, rotateY, rotateZ;
	float lastX, lastY, pitch, yaw, fov = 45.0f;
	float sensitivity;
	glm::vec3 cameraFront, cameraPos, cameraUp;
};
#pragma once
