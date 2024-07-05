#include "Mesh.h"
#include <vector>
void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, GLuint textureID, glm::vec3 position, float scale, float angle, glm::vec3 axis)
{
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->position = position;
	this->scale = scale;
	this->angle = angle;
	this->axis = axis;
	this->textureID = textureID;
	this->curvePoints;
}

void Mesh::update()
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, glm::vec3(scale));
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::updatePosition(glm::vec3 position) {
	this->position = position;
}

void Mesh::translateObject(char axis, float value) {
	
	if (axis == 'x') this->position.x += value;
	else if (axis == 'y') this->position.y += value;
	else this->position.z += value;
}

void Mesh::rotate(glm::vec3 axis, float angle) {
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, angle == 0.0f ? (GLfloat)glfwGetTime() : angle, axis);
	model = glm::scale(model, glm::vec3(scale));
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::setScale(float scaleValue) {
	float inc = this->scale + scaleValue;

	this->scale = inc;

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, glm::vec3(scale));
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::setCurvePoints(vector<glm::vec3> curvePoints) {
	this->curvePoints = curvePoints;
}

void Mesh::moveObject() {
	int i = 0;
	glm::vec3 pointOnCurve = this->curvePoints.at(i);
	this->updatePosition(glm::vec3(pointOnCurve.x, pointOnCurve.y, pointOnCurve.z));
	i = (i + 1) % curvePoints.size();
}