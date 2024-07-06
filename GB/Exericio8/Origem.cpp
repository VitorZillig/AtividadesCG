/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 12/05/2023
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
#include "../curves/Camera.h"
#include "../curves/Bezier.h"

// Protótipo da função de callback do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
vector<GLuint> loadSimpleOBJ(string filepath, glm::vec3 color);
int loadTexture(string path);
string getTextureName(string filepath);
void readConfigFile(string path, string obj);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1600, HEIGHT = 1000;

bool rotateX = false, rotateY = false, rotateZ = false;

float q, ka, ks, kd;

bool startScene = false;

string nomeOBJ, controlPointsName;

glm::vec3 pos, eixo;
float escala, angulo;

glm::vec3 lightPos, lightColor;

float sensitivity, pitch, yaw;
glm::vec3 cameraFront, cameraPos, cameraUp;

Camera camera;


Mesh bolinha, raqueteLeft, raqueteRight, mesa;

Mesh* currentObject = &bolinha;
Mesh* notSelectedObject = &raqueteRight;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
	//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Vitor Zillig, Marcelo Fontana e Nicolas Raimundo", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("Phong.vs", "Phong.fs");

	// Leitura do arquivo "cube.obj" para obter a geometria

	readConfigFile("config.txt", "bolinha");
	vector<GLuint> VAO = loadSimpleOBJ(nomeOBJ, glm::vec3(0, 1, 0));
	bolinha.initialize(VAO[0], VAO[1], &shader, VAO[2], pos, escala, angulo, eixo);
	Bezier trajetoriaBolinha;
	trajetoriaBolinha.loadControlPoints(controlPointsName);


	readConfigFile("config.txt", "rightBat");
	VAO = loadSimpleOBJ(nomeOBJ, glm::vec3(0, 1, 0));
	raqueteRight.initialize(VAO[0], VAO[1], &shader, VAO[2], pos, escala, angulo, eixo);
	Bezier trajetoriaRaqueteRight;
	trajetoriaRaqueteRight.loadControlPoints(controlPointsName);


	readConfigFile("config.txt", "leftBat");
	raqueteLeft.initialize(VAO[0], VAO[1], &shader, VAO[2], pos, escala, angulo, eixo);
	Bezier trajetoriaRaqueteLeft;
	trajetoriaRaqueteLeft.loadControlPoints(controlPointsName);


	readConfigFile("config.txt", "table");
	VAO = loadSimpleOBJ(nomeOBJ, glm::vec3(0, 1, 0));
	mesa.initialize(VAO[0], VAO[1], &shader, VAO[2], pos, escala, angulo, eixo);

	glUseProgram(shader.ID);


	readConfigFile("config.txt", "camera");
	camera.initialize(&shader, width, height, sensitivity, pitch, yaw, cameraFront, cameraPos, glm::vec3(cameraUp.x, cameraUp.y, 0.0));

	glEnable(GL_DEPTH_TEST);

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", ka);
	shader.setFloat("kd", kd);
	shader.setFloat("ks", ks);
	shader.setFloat("q", q);

	readConfigFile("config.txt", "light");
	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
	shader.setVec3("lightColor", lightColor.r, lightColor.g, lightColor.b);

	int iBolinha = 0;
	trajetoriaBolinha.generateCurve(300);
	int nbCurvePointsBolinha = trajetoriaBolinha.getNbCurvePoints();

	int iRaqueteRight = 0;
	trajetoriaRaqueteRight.generateCurve(550);
	int nbCurvePointsRaqueteRight = trajetoriaRaqueteRight.getNbCurvePoints();

	int iRaqueteLeft = 0;
	trajetoriaRaqueteLeft.generateCurve(550);
	int nbCurvePointsRaqueteLeft = trajetoriaRaqueteLeft.getNbCurvePoints();
	
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		camera.update();

		mesa.rotate(glm::vec3(1.0f, 1.0f, 1.0f),10.5f);
		mesa.draw();
		
		currentObject->update();
		
		if (rotateX)
		{
			currentObject->rotate(glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotateY)
		{
			currentObject->rotate(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotateZ)
		{
			currentObject->rotate(glm::vec3(0.0f, 0.0f, 1.0f));
		}

		currentObject->draw();

		notSelectedObject->update();
		notSelectedObject->draw();

		raqueteLeft.update();
		raqueteLeft.draw();

		if (startScene) {
			glm::vec3 pointOnCurveBolinha = trajetoriaBolinha.getPointOnCurve(iBolinha);
			bolinha.updatePosition(glm::vec3(pointOnCurveBolinha.x, pointOnCurveBolinha.y, pointOnCurveBolinha.z));
			iBolinha = (iBolinha + 1) % nbCurvePointsBolinha;

			glm::vec3 pointOnCurveRaqueteRight = trajetoriaRaqueteRight.getPointOnCurve(iRaqueteRight);
			raqueteRight.updatePosition(glm::vec3(pointOnCurveRaqueteRight.x, pointOnCurveRaqueteRight.y, pointOnCurveRaqueteRight.z));
			iRaqueteRight = (iRaqueteRight + 1) % nbCurvePointsRaqueteRight;

			glm::vec3 pointOnCurveRaqueteLeft = trajetoriaRaqueteLeft.getPointOnCurve(iRaqueteLeft);
			raqueteLeft.updatePosition(glm::vec3(pointOnCurveRaqueteLeft.x, pointOnCurveRaqueteLeft.y, pointOnCurveRaqueteLeft.z));
			iRaqueteLeft = (iRaqueteLeft + 1) % nbCurvePointsRaqueteLeft;
		}
		
		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO[0]);

	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
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

	//Adicionados os comandos para a movimentação nos três eixos, além da alteração da escala por meio das teclas de "-" e "+"
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		currentObject->translateObject('y', 0.1f);
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		currentObject->translateObject('y', -0.1f);
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		currentObject->translateObject('x', -0.1f);
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		currentObject->translateObject('x', 0.1f);
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		currentObject->translateObject('z', 0.1f);
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		currentObject->translateObject('z', -0.1f);
	}
	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
		currentObject->setScale(-0.2);
	}
	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
		currentObject->setScale(0.2);
	}

	camera.translate(window, key, action);

	if (key == GLFW_KEY_1) 
	{
		rotateX = false;
		rotateY = false;
		rotateZ = false;

		currentObject = &bolinha;
		notSelectedObject = &raqueteRight;
	}
	if (key == GLFW_KEY_2)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = false;

		currentObject = &raqueteRight;
		notSelectedObject = &bolinha;
	}
	if (key == GLFW_KEY_E)
	{
		startScene = false;
	}
	if (key == GLFW_KEY_P)
	{
		startScene = true;
	}
}

// Adição da função "loadSimpleOBJ"
vector<GLuint> loadSimpleOBJ(string filepath, glm::vec3 color)
{
	vector <glm::vec3> vertices;
	vector <GLuint> indices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vbuffer;

	string textureName;

	GLuint texID;

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
			//Mapeando as posições
			if (word == "mtllib") {
				string tokens[1];

				ssline >> tokens[0];
				
				textureName = tokens[0];

			}
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
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);
	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	//Atributo normal do vértice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	
	textureName = getTextureName(textureName);

	texID = loadTexture(textureName);
	
	return {VAO, vbuffer.size()/11, texID};
}

// Adição da função "loadTexture"
int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
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

// Função que lê o arquivo passado e retorna o nome da textura
string getTextureName(string filepath)
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
	camera.rotate(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.zoom(window, xoffset, yoffset);
}

void readConfigFile(string path, string obj) {
	ifstream inputFile;
	inputFile.open(path.c_str());
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
			if (word == obj) {
				if (word == "light") {
					string tokens[6];
					float parsedValues[6];
					ssline >> tokens[0] >> tokens[1] >> tokens[2] >> tokens[3] >> tokens[4] >> tokens[5];

					for (size_t i = 0; i < 5; i++)
					{
						parsedValues[i] = std::stof(tokens[i]);
					}

					lightPos = glm::vec3(parsedValues[0], parsedValues[1], parsedValues[2]);
					lightColor = glm::vec3(parsedValues[3], parsedValues[4], parsedValues[5]);


				}
				else if (word == "camera") {
					string tokens[12];
					float parsedValues[12];
					ssline >> tokens[0] >> tokens[1] >> tokens[2] >> tokens[3] >> tokens[4] >> tokens[5] >> tokens[6] >> tokens[7] >> tokens[8] >> tokens[9] >> tokens[10] >> tokens[11];

					for (size_t i = 0; i < 11; i++)
					{
						parsedValues[i] = std::stof(tokens[i]);
					}

					sensitivity = parsedValues[0];
					pitch = parsedValues[1];
					yaw = parsedValues[2];
					cameraFront = glm::vec3(parsedValues[3], parsedValues[4], parsedValues[5]);
					cameraPos = glm::vec3(parsedValues[6], parsedValues[7], parsedValues[8]);
					cameraUp = glm::vec3(parsedValues[9], parsedValues[10], parsedValues[11]);
				}
				else {
					string tokens[10];
					float parsedValues[8];

					ssline >> tokens[0] >> tokens[1] >> tokens[2] >> tokens[3] >> tokens[4] >> tokens[5] >> tokens[6] >> tokens[7] >> tokens[8] >> tokens[9];
		
					for (size_t i = 2; i < 9; i++)
					{
						parsedValues[i-2] = std::stof(tokens[i]);
					}
					nomeOBJ = tokens[0];
					controlPointsName = tokens[1];
					pos = glm::vec3(parsedValues[0], parsedValues[1], parsedValues[2]);
					escala = parsedValues[3];
					angulo = parsedValues[4];
					eixo = glm::vec3(parsedValues[5], parsedValues[6], parsedValues[7]);
					}
				}
		}
	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << path << endl;
	}
	inputFile.close();
}
