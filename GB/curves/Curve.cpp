#include "Curve.h"

void Curve::setShader(Shader* shader)
{
	this->shader = shader;
	shader->Use();
}

void Curve::drawCurve(glm::vec4 color)
{
	shader->setVec4("finalColor", color.r, color.g, color.b, color.a);

	glBindVertexArray(VAO);
	// Chamada de desenho - drawcall
	// CONTORNO e PONTOS - GL_LINE_LOOP e GL_POINTS
	glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());
	//glDrawArrays(GL_POINTS, 0, curvePoints.size());
	glBindVertexArray(0);

}

void Curve::loadControlPoints(string filepath) {
	vector <glm::vec3> vertices;

	ifstream inputFile;

	inputFile.open(filepath.c_str());
	if (inputFile.is_open())
	{
		char line[100];
		string sline;

		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			sline = line;

			string word;

			istringstream ssline(line);
			ssline >> word;
			//cout << word << " ";
			//Mapeando as posições
			if (word == "v")
			{
				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;

				vertices.push_back(v);
			}
		}
	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}
	inputFile.close();
	this->setControlPoints(vertices);
}