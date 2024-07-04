#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, int nVertices, Shader* shader, GLuint textureID, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0), float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();
	void draw();
	void updatePosition(glm::vec3 position);
	void rotate(float speed = 1, glm::vec3 axis = glm::vec3(0.0, 0.0, 0.0));
	void increase(float increaseNumber = 0.1f);
	void decrease(float decreaseNumber = 0.1f);
	void resetScale(float scale = 1.0f);
	void setCurvePoints(vector<glm::vec3> curvePoints);
	void moveObject();

protected:
	GLuint VAO; //Identificador do Vertex Array Object - Vértices e seus atributos
	int nVertices;

	//Informações sobre as transformações a serem aplicadas no objeto
	glm::vec3 position;
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;
	vector<glm::vec3> curvePoints;

	//Referência (endereço) do shader
	Shader* shader;

	GLuint textureID;
};
