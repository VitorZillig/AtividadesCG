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
string getTextureName(string filepath, GLuint shaderID);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1600, HEIGHT = 1000;

bool rotateX = false, rotateY = false, rotateZ = false;

// Adicionadas as variáveis para possibilitar o deslocamento nos três eixos, bem como a alteração da escala

float q, ka, ks, kd;

bool startScene = false;

vector <glm::vec3> curvePointsBolinha;
vector <glm::vec3> curvePointsRaquete;

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Vitor Zillig", nullptr, nullptr);
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

	// Compilando e buildando o programa de shader

	// Leitura do arquivo "cube.obj" para obter a textura
	string textureNameBolinha = getTextureName("ping_pong_bolinha.mtl", shader.ID);
	string textureNameRacket = getTextureName("ping_pong_bat.mtl", shader.ID);
	string textureNameTable = getTextureName("ping_pong_table.mtl", shader.ID);

	GLuint texIDBolinha = loadTexture(textureNameBolinha);
	GLuint texIDBat = loadTexture(textureNameRacket);
	GLuint texIDTable = loadTexture(textureNameTable);

	// Leitura do arquivo "cube.obj" para obter a geometria
	vector<GLuint> VAO = loadSimpleOBJ("ping_pong_bolinha.obj", glm::vec3(0, 1, 0));
	vector<GLuint> VAO2 = loadSimpleOBJ("ping_pong_bat.obj", glm::vec3(0, 1, 0));
	vector<GLuint> VAO3 = loadSimpleOBJ("ping_pong_bat.obj", glm::vec3(0, 1, 0));
	vector<GLuint> VAO4 = loadSimpleOBJ("ping_pong_table.obj", glm::vec3(0, 1, 0));

	bolinha.initialize(VAO[0], VAO[1], &shader, texIDBolinha, glm::vec3(0.0f, 0.0f, 0.0f), 0.03f);
	raqueteRight.initialize(VAO2[0], VAO2[1], &shader, texIDBat, glm::vec3(1.5f, 0.3f, 0.0f), 0.03f, 90.0f, glm::vec3(0.0, 0.0, 1.0));
	raqueteLeft.initialize(VAO3[0], VAO3[1], &shader, texIDBat, glm::vec3(-1.5f, 0.3f, 0.0f), 0.03f, 90.0f, glm::vec3(0.0, 0.0, 1.0));
	mesa.initialize(VAO4[0], VAO4[1], &shader, texIDTable, glm::vec3(-0.2f, -0.85f, 0.0f), 0.01f, 180.0f, glm::vec3(0.0, 1.0, 1.0));


	glUseProgram(shader.ID);

	camera.initialize(&shader, width, height, 0.05f, 0.0f, -90.0, glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 1.0, 0.0));

	glEnable(GL_DEPTH_TEST);

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", ka);
	shader.setFloat("kd", kd);
	shader.setFloat("ks", ks);
	shader.setFloat("q", q);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", 0.0, 0.0, 0.0);
	shader.setVec3("lightColor", 1.0, 1.0, 0.0);

	int iBolinha = 0;
	Bezier trajetoriaBolinha;
	trajetoriaBolinha.loadControlPoints("controlPointsBolinha.txt");
	trajetoriaBolinha.generateCurve(300);
	int nbCurvePointsBolinha = trajetoriaBolinha.getNbCurvePoints();

	int iRaqueteRight = 0;
	Bezier trajetoriaRaqueteRight;
	trajetoriaRaqueteRight.loadControlPoints("controlPointsRaqueteRight.txt");
	trajetoriaRaqueteRight.generateCurve(550);
	int nbCurvePointsRaqueteRight = trajetoriaRaqueteRight.getNbCurvePoints();

	int iRaqueteLeft = 0;
	Bezier trajetoriaRaqueteLeft;
	trajetoriaRaqueteLeft.loadControlPoints("controlPointsRaqueteLeft.txt");
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

		//mesa.rotate(glm::vec3(0.0, 0.0, 1.0), 90.0f);
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
	glDeleteVertexArrays(1, &VAO2[0]);

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

	camera.move(window, key, action);

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
	

	return {VAO, vbuffer.size()/11};
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
	camera.rotate(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.zoom(window, xoffset, yoffset);
}
