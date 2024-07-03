/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION  
#include "../../M3 - Adicionando Texturas/stb_image.h"

#include "../curves/Shader.h"
#include "../curves/Mesh.h"


// Prot�tipo da fun��o de callback do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color);
int loadTexture(string path);
string getTextureName(string filepath, GLuint shaderID);
void generateCurve(int pointsPerSegment);
vector<glm::vec3> loadControlPoints(string filepath);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX = false, rotateY = false, rotateZ = false;

// Adicionadas as vari�veis para possibilitar o deslocamento nos tr�s eixos, bem como a altera��o da escala

float posX1 = 0.0f, posY1 = 0.0f, posZ1 = 0.0f, scale = 1.0f;
float posX2 = 0.0f, posY2 = 0.0f, posZ2 = 0.0f;
float q, ka, ks, kd;
float fov = 45.0f;


glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05;
float pitch = 0.0, yaw = -90.0;

vector <glm::vec3> curvePoints;
vector <glm::vec3> controlPoints;

Mesh cubo1, cubo2;

Mesh* currentCube = &cubo1;
// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
	//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Vitor Zillig", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("Phong.vs", "Phong.fs");

	// Compilando e buildando o programa de shader

	// Leitura do arquivo "cube.obj" para obter a textura
	string textureName = getTextureName("cube.mtl", shader.ID);

	GLuint texID = loadTexture("cube.png");

	int nVerts = 36;

	// Leitura do arquivo "cube.obj" para obter a geometria
	GLuint VAO = loadSimpleOBJ("cube.obj", nVerts, glm::vec3(0, 1, 0));
	GLuint VAO2 = loadSimpleOBJ("cube.obj", nVerts, glm::vec3(0, 1, 0));

	
	cubo1.initialize(VAO, nVerts, &shader, texID, glm::vec3(0.0f, 0.0f, 0.0f));
	cubo2.initialize(VAO2, nVerts, &shader, texID, glm::vec3(1.0f, 0.0f, 0.0f));

	glUseProgram(shader.ID);

	//Matriz de view -- posi��o e orienta��o da c�mera
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	glEnable(GL_DEPTH_TEST);

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", ka);
	shader.setFloat("kd", kd);
	shader.setFloat("ks", ks);
	shader.setFloat("q", q);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", 0.0, 0.0, 0.0);
	shader.setVec3("lightColor", 1.0, 1.0, 0.0);

	int i = 0;

	controlPoints = loadControlPoints("controlPoints.txt");
	generateCurve(500);

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
		GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
		shader.setMat4("projection", value_ptr(projection));

		//Atualizando a posi��o e orienta��o da c�mera
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", value_ptr(view));

		//Atualizando o shader com a posi��o da c�mera
		glUniformMatrix3fv(glGetUniformLocation(shader.ID, "cameraPos"), 1, FALSE, value_ptr(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z)));

		cubo1.updatePosition(glm::vec3(posX1, posY1, posZ1));

		cubo1.update();
	
		if (rotateX)
		{

			currentCube->rotate(1, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		else if (rotateY)
		{
			currentCube->rotate(1, glm::vec3(0.0f, 1.0f, 0.0f));


		}
		else if (rotateZ)
		{
			currentCube->rotate(1, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		cubo1.draw();

		cubo2.update();
		cubo2.draw();

		glm::vec3 pointOnCurve = curvePoints.at(i);
		vector <glm::vec3> aux;
		aux.push_back(pointOnCurve);

		posX1 = pointOnCurve.x;
		posY1 = pointOnCurve.y;
		posZ1 = pointOnCurve.z;

		cubo1.updatePosition(glm::vec3(pointOnCurve.x, pointOnCurve.y, pointOnCurve.z));

		i = (i + 1) % curvePoints.size();
		

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAO2);

	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

	//Adicionados os comandos para a movimenta��o nos tr�s eixos, al�m da altera��o da escala por meio das teclas de "-" e "+"
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		posY1 += 0.1f;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		posY1 -= 0.1f;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		posX1 -= 0.1f;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		posX1 += 0.1f;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		posZ1 += 0.1f;
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		posZ1 -= 0.1f;
	}
	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
		scale -= 0.2;
	}
	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
		scale += 0.2;
	}

	float cameraSpeed = 0.05;

	if (key == GLFW_KEY_O)
	{
		cameraPos += cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_J)
	{
		cameraPos -= cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_K)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (key == GLFW_KEY_L)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (key == GLFW_KEY_0) 
	{
		currentCube = &cubo1;
	}
	if (key == GLFW_KEY_1)
	{
		currentCube = &cubo2;
	}
}

//Esta fun��o est� bastante hardcoded - objetivo � compilar e "buildar" um programa de
// shader simples e �nico neste exemplo de c�digo
// O c�digo fonte do vertex e fragment shader est� nos arrays vertexShaderSource e
// fragmentShader source no ini�io deste arquivo

// Adi��o da fun��o "loadSimpleOBJ"
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color)
{
	vector <glm::vec3> vertices;
	vector <GLuint> indices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vbuffer;

	// Leitura do arquivo

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
			//Mapeando as posi��es
			if (word == "v")
			{
				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;


				vertices.push_back(v);
			}
			//Mapeando as coordenadas da textura
			if (word == "vt")
			{
				glm::vec2 vt;
				ssline >> vt.s >> vt.t;

				texCoords.push_back(vt);
			}
			//Mapeando as coordenadas dos vetores normais
			if (word == "vn")
			{
				glm::vec3 vn;
				ssline >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			if (word == "f")
			{
				string tokens[3];

				ssline >> tokens[0] >> tokens[1] >> tokens[2];

				for (int i = 0; i < 3; i++)
				{
					//Recuperando os indices de v
					int pos = tokens[i].find("/");
					string token = tokens[i].substr(0, pos);
					int index = atoi(token.c_str()) - 1;
					indices.push_back(index);

					vbuffer.push_back(vertices[index].x);
					vbuffer.push_back(vertices[index].y);
					vbuffer.push_back(vertices[index].z);

					vbuffer.push_back(color.r);
					vbuffer.push_back(color.g);
					vbuffer.push_back(color.b);

					//Recuperando os indices de vts
					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;
					vbuffer.push_back(texCoords[index].s);
					vbuffer.push_back(texCoords[index].t);

					//Recuperando os indices de vns
					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;
					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}
		}
	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}
	inputFile.close();
	GLuint VBO, VAO;
	nVerts = vbuffer.size() / 11; // 3 pos + 3 cor + 3 normal + 2 texcoord
	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);
	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	//Atributo normal do v�rtice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}

// Adi��o da fun��o "loadTexture"
int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na mem�ria 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os par�metros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

// Fun��o que l� o arquivo passado e retorna o nome da textura
string getTextureName(string filepath, GLuint shaderID)
{

	ifstream inputFile;

	inputFile.open(filepath.c_str());

	string textureName;

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
			if (word == "map_Kd")
			{
				string tokens[1];
				ssline >> tokens[0];

				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;
				textureName = tokens[0];
			}
			if (word == "Ns")
			{
				string tokens[1];
				ssline >> tokens[0];
				string value = tokens[0];

				float parseFloat = std::stof(value);

				q = parseFloat;
			}
			if (word == "Ka")
			{
				string tokens[1];
				ssline >> tokens[0];
				string value = tokens[0];

				float parseFloat = std::stof(value);

				ka = parseFloat;
			}
			if (word == "Ks")
			{
				string tokens[1];
				ssline >> tokens[0];
				string value = tokens[0];

				float parseFloat = std::stof(value);

				ks = parseFloat;
			}
			if (word == "Kd")
			{
				string tokens[1];
				ssline >> tokens[0];
				string value = tokens[0];

				float parseFloat = std::stof(value);

				kd = parseFloat;
			}

		}
	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}
	inputFile.close();

	return textureName;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cout << xoffset;
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void generateCurve(int pointsPerSegment) {


	glm::mat4 M = glm::mat4(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0
	);


	float step = 1.0 / (float)pointsPerSegment;

	float t = 0;

	int nControlPoints = controlPoints.size();

	for (int i = 0; i < nControlPoints - 3; i += 3)
	{

		for (float t = 0.0; t <= 1.0; t += step)
		{
			glm::vec3 p;

			glm::vec4 T(t * t * t, t * t, t, 1);

			glm::vec3 P0 = controlPoints[i];
			glm::vec3 P1 = controlPoints[i + 1];
			glm::vec3 P2 = controlPoints[i + 2];
			glm::vec3 P3 = controlPoints[i + 3];

			glm::mat4x3 G(P0, P1, P2, P3);

			p = G * M * T;  //---------
			
			curvePoints.push_back(p);
		}
	}
}

vector<glm::vec3> loadControlPoints(string filepath) {
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
			//Mapeando as posi��es
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
	return vertices;

}