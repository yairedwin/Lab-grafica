/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
bool avanza = true; //true: el rover va hacia adelante (+x), false: va de reversa (-x)

float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
float movCoche = 0.0f;
float movOffset = 1.0f;
float limiteX1 = 296.0f;   //extremo derecho del piso (300) menos lo que mide el rover hacia adelante
float limiteX2 = -297.0f;  //extremo izquierdo del piso (-300) menos lo que mide el rover hacia atras
float girollanta = 0.0f;
float girollantaincremental = 15.0f;

//variables del avion (vuelta curva en forma de pista)
float avionX = -30.0f;
float avionY = 10.0f;
float avionZ = -25.0f;
float avionGiro = 0.0f;            //hacia donde apunta el avion, en grados (0 = hacia +x)
float avionOffset = 0.5f;          //lo que avanza por ciclo
float avionGiroOffset = 2.0f;      //lo que gira por ciclo cuando va en la curva
float avionGiroAnterior = 0.0f;
float avionGiroMedio = 0.0f;
float avionRadio = 0.0f;
int avionFase = 0;                 //0 recta de ida, 1 curva, 2 recta de regreso, 3 curva
float avionX1 = -30.0f;            //donde empieza y termina cada recta
float avionX2 = 30.0f;
float avionZ1 = -25.0f;            //carril de ida

//variables de la nave de carreras: 2 rectas, 2 curvas y una rampa de subida y otra de bajada
float naveX = -42.0f;
float naveY = 13.2f;               //un poco arriba de la pista para que flote
float naveZ = 73.0f;
float naveGiro = 0.0f;             //hacia donde apunta la nave, en grados (0 = hacia +x)
float naveInclina = 0.0f;          //cabeceo: sube la nariz en la subida y la baja en la bajada
float naveOffset = 0.8f;           //lo que avanza por ciclo
float naveGiroOffset = 0.0f;       //lo que gira por ciclo en las curvas
float naveGiroAnterior = 0.0f;
float naveGiroMedio = 0.0f;
float naveRadio = 42.0f;           //radio de las curvas
int naveFase = 0;                  //0 recta de ida, 1 curva, 2 recta de regreso, 3 curva
float pistaX1 = -42.0f;            //donde empiezan y terminan las rectas
float pistaX2 = 42.0f;
float pistaZ1 = 73.0f;             //carril de ida (el de regreso queda en z = -11)
float pistaY1 = 12.0f;             //altura de la pista abajo y arriba
float pistaY2 = 24.0f;
float rampaSubeX1 = -20.0f;        //la rampa de subida va de x = -20 a x = 10
float rampaSubeX2 = 10.0f;
float rampaBajaX1 = 20.0f;         //la de bajada va de x = 20 a x = -10
float rampaBajaX2 = -10.0f;
float rampaPendiente = 0.4f;       //12 de altura en 30 de largo
float rampaAngulo = 0.0f;          //angulo de las rampas y lo que miden
float rampaLargo = 0.0f;
float pistaYM = 0.0f;              //altura a la mitad de las rampas
float pistaZ2 = 0.0f;              //carril de regreso
float curvaAngulo = 0.0f;          //rumbo de cada pedazo de curva y su posicion
float curvaX = 0.0f;
float curvaZ = 0.0f;
float pistaZM = 0.0f;              //z del centro de las curvas

//variables del satelite: recorre un rectangulo y sus partes se mueven solas
float satX = -30.0f;
float satY = 40.0f;
float satZ = 51.0f;
float satRumbo = 0.0f;             //hacia donde apunta el satelite, en grados
float satObjetivo = 0.0f;          //rumbo al que va a girar en la esquina
float satOffset = 0.4f;
float satGiroOffset = 2.0f;
float satYOffset = 0.05f;
float satY1 = 38.0f;               //limites de la subida y bajada del satelite
float satY2 = 44.0f;
float satX1 = -30.0f;              //esquinas del rectangulo
float satX2 = 30.0f;
float satZ1 = 51.0f;
float satZ2 = 11.0f;
int satLado = 0;                   //0 va hacia +x, 1 hacia -z, 2 hacia -x, 3 hacia +z
bool satGirando = false;
bool satSube = true;
float panelGiro = 0.0f;
float panelOffset = 1.5f;
float torreAngulo = 0.0f;
float torreOffset = 1.0f;
bool torreSube = true;
float antenaAngulo = 0.0f;
float antenaOffset = 1.5f;
bool antenaSube = true;
float instrumentoGiro = 0.0f;
float instrumentoOffset = 3.0f;
GLfloat escalaSat = 0.35f;

Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture pistaTexture;



//rover por piezas (practica 5), las llantas ya son solo la llanta con su origen en el centro
Model Cuerpo_M;
Model Brazo_M;
Model PataFD_M, PataFI_M, PataMD_M, PataMI_M, PataTD_M, PataTI_M; //pata de cada llanta, sin girar
Model LlantaFD_M; //frontal derecha
Model LlantaFI_M; //frontal izquierda
Model LlantaMD_M; //media derecha
Model LlantaMI_M; //media izquierda
Model LlantaTD_M; //trasera derecha
Model LlantaTI_M; //trasera izquierda
//mi avion de la practica 6
Model Avion_M;
//nave de carreras (la nave de la practica 8)
Model Nave_M;
//satelite SAC-C de la practica 5: el cuerpo y las partes que se mueven en su union con el cuerpo
Model SatCuerpo_M, SatPanelIzq_M, SatPanelDer_M, SatTorre_M, SatInstrumento_M, SatAntena_M;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

	//pedazo de pista: plano de 1 x 1, la textura va de orilla a orilla; se escala en el main
	unsigned int pistaIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat pistaVertices[] = {
		-0.5f, 0.0f, -0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, 0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f
	};

	MeshModel* obj8 = new MeshModel();
	obj8->CreateMeshModel(pistaVertices, pistaIndices, 32, 6);
	meshList.push_back(obj8); // pedazo de pista

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(2400, 1600); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 60.0f, 150.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -22.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

	Cuerpo_M = Model();
	Cuerpo_M.LoadModel("Models/Rover/Cuerpo.obj");
	Brazo_M = Model();
	Brazo_M.LoadModel("Models/Rover/Brazo.obj");
	PataFD_M = Model();
	PataFD_M.LoadModel("Models/Rover/PataFD.obj");
	PataFI_M = Model();
	PataFI_M.LoadModel("Models/Rover/PataFI.obj");
	PataMD_M = Model();
	PataMD_M.LoadModel("Models/Rover/PataMD.obj");
	PataMI_M = Model();
	PataMI_M.LoadModel("Models/Rover/PataMI.obj");
	PataTD_M = Model();
	PataTD_M.LoadModel("Models/Rover/PataTD.obj");
	PataTI_M = Model();
	PataTI_M.LoadModel("Models/Rover/PataTI.obj");
	LlantaFD_M = Model();
	LlantaFD_M.LoadModel("Models/Rover/LlantaFD.obj");
	LlantaFI_M = Model();
	LlantaFI_M.LoadModel("Models/Rover/LlantaFI.obj");
	LlantaMD_M = Model();
	LlantaMD_M.LoadModel("Models/Rover/LlantaMD.obj");
	LlantaMI_M = Model();
	LlantaMI_M.LoadModel("Models/Rover/LlantaMI.obj");
	LlantaTD_M = Model();
	LlantaTD_M.LoadModel("Models/Rover/LlantaTD.obj");
	LlantaTI_M = Model();
	LlantaTI_M.LoadModel("Models/Rover/LlantaTI.obj");

	Avion_M = Model();
	Avion_M.LoadModel("Models/avion_49.obj"); //sus texturas las lee de avion_49.mtl

	pistaTexture = Texture("Textures/pista_reporte.png");
	pistaTexture.LoadTextureA();

	Nave_M = Model();
	Nave_M.LoadModel("Models/nave_p8.obj"); //sus texturas las lee de nave_p8.mtl

	SatCuerpo_M = Model();
	SatCuerpo_M.LoadModel("Models/Satelite/SatCuerpo.obj");
	SatPanelIzq_M = Model();
	SatPanelIzq_M.LoadModel("Models/Satelite/SatPanelIzq.obj");
	SatPanelDer_M = Model();
	SatPanelDer_M.LoadModel("Models/Satelite/SatPanelDer.obj");
	SatTorre_M = Model();
	SatTorre_M.LoadModel("Models/Satelite/SatTorre.obj");
	SatInstrumento_M = Model();
	SatInstrumento_M.LoadModel("Models/Satelite/SatInstrumento.obj");
	SatAntena_M = Model();
	SatAntena_M.LoadModel("Models/Satelite/SatAntena.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.7f, 0.5f,
		0.0f, -1.0f, -0.3f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.01f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.005f, 0.002f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	avionRadio = avionOffset / glm::radians(avionGiroOffset); //radio de la curva con esos incrementos
	naveGiroOffset = glm::degrees(naveOffset / naveRadio); //giro por ciclo para ese radio
	rampaAngulo = atan(rampaPendiente);
	rampaLargo = 30.0f / cos(rampaAngulo);
	pistaYM = (pistaY1 + pistaY2) / 2.0f;
	pistaZ2 = pistaZ1 - 2 * naveRadio;
	pistaZM = pistaZ1 - naveRadio;


	glm::vec3 lowerLight(0.0f,0.0f,0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 roverM(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

		//rover avanza hasta la orilla, ahi invierte la bandera y regresa de reversa
		//las llantas giran dentro del mismo if, con un signo distinto segun el sentido
		if (avanza)
		{
			movCoche += movOffset * deltaTime;
			girollanta -= girollantaincremental * deltaTime;
			if (movCoche >= limiteX1)
				avanza = false;
		}
		else
		{
			movCoche -= movOffset * deltaTime;
			girollanta += girollantaincremental * deltaTime;
			if (movCoche <= limiteX2)
				avanza = true;
		}

		//avion: siempre avanza hacia donde apunta, nunca va de reversa
		//recta de ida, curva, recta de regreso y otra curva
		avionGiroAnterior = avionGiro;
		if (avionFase == 1 || avionFase == 3)
			avionGiro += avionGiroOffset * deltaTime;
		avionGiroMedio = (avionGiroAnterior + avionGiro) / 2.0f;
		avionX += avionOffset * cos(glm::radians(avionGiroMedio)) * deltaTime;
		avionZ -= avionOffset * sin(glm::radians(avionGiroMedio)) * deltaTime;
		if (avionFase == 0 && avionX >= avionX2)
		{
			avionX = avionX2;
			avionFase = 1;
		}
		if (avionFase == 1 && avionGiro >= 180.0f)
		{
			avionGiro = 180.0f;
			avionZ = avionZ1 - 2 * avionRadio;
			avionFase = 2;
		}
		if (avionFase == 2 && avionX <= avionX1)
		{
			avionX = avionX1;
			avionFase = 3;
		}
		if (avionFase == 3 && avionGiro >= 360.0f)
		{
			avionGiro = 0.0f;
			avionZ = avionZ1;
			avionFase = 0;
		}

		//nave de carreras: recta con rampa de subida, curva, recta con rampa de bajada y curva
		naveGiroAnterior = naveGiro;
		if (naveFase == 1 || naveFase == 3)
			naveGiro += naveGiroOffset * deltaTime;
		naveGiroMedio = (naveGiroAnterior + naveGiro) / 2.0f;
		naveX += naveOffset * cos(glm::radians(naveGiroMedio)) * deltaTime;
		naveZ -= naveOffset * sin(glm::radians(naveGiroMedio)) * deltaTime;
		if (naveFase == 0)
		{
			if (naveX >= rampaSubeX1 && naveX < rampaSubeX2)
			{
				naveY += naveOffset * rampaPendiente * deltaTime;
				naveInclina = glm::degrees(atan(rampaPendiente));
			}
			else
			{
				naveInclina = 0.0f;
				if (naveX >= rampaSubeX2)
					naveY = pistaY2 + 1.2f;
			}
			if (naveX >= pistaX2)
			{
				naveX = pistaX2;
				naveFase = 1;
			}
		}
		if (naveFase == 1 && naveGiro >= 180.0f)
		{
			naveGiro = 180.0f;
			naveZ = pistaZ2;
			naveFase = 2;
		}
		if (naveFase == 2)
		{
			if (naveX <= rampaBajaX1 && naveX > rampaBajaX2)
			{
				naveY -= naveOffset * rampaPendiente * deltaTime;
				naveInclina = -glm::degrees(atan(rampaPendiente));
			}
			else
			{
				naveInclina = 0.0f;
				if (naveX <= rampaBajaX2)
					naveY = pistaY1 + 1.2f;
			}
			if (naveX <= pistaX1)
			{
				naveX = pistaX1;
				naveFase = 3;
			}
		}
		if (naveFase == 3 && naveGiro >= 360.0f)
		{
			naveGiro = 0.0f;
			naveZ = pistaZ1;
			naveFase = 0;
		}

		//satelite: recorre un rectangulo, en cada esquina se detiene y gira 90 grados
		if (satGirando)
		{
			satRumbo += satGiroOffset * deltaTime;
			if (satRumbo >= satObjetivo)
			{
				satRumbo = satObjetivo;
				satGirando = false;
				if (satRumbo >= 360.0f)
					satRumbo = 0.0f;
			}
		}
		else
		{
			satX += satOffset * cos(glm::radians(satRumbo)) * deltaTime;
			satZ -= satOffset * sin(glm::radians(satRumbo)) * deltaTime;
			if (satLado == 0 && satX >= satX2)
			{
				satX = satX2;
				satObjetivo = 90.0f;
				satGirando = true;
				satLado = 1;
			}
			if (satLado == 1 && satZ <= satZ2)
			{
				satZ = satZ2;
				satObjetivo = 180.0f;
				satGirando = true;
				satLado = 2;
			}
			if (satLado == 2 && satX <= satX1)
			{
				satX = satX1;
				satObjetivo = 270.0f;
				satGirando = true;
				satLado = 3;
			}
			if (satLado == 3 && satZ >= satZ1)
			{
				satZ = satZ1;
				satObjetivo = 360.0f;
				satGirando = true;
				satLado = 0;
			}
		}
		if (satSube)
		{
			satY += satYOffset * deltaTime;
			if (satY >= satY2)
				satSube = false;
		}
		else
		{
			satY -= satYOffset * deltaTime;
			if (satY <= satY1)
				satSube = true;
		}
		//las partes del satelite: cada una con su variable de angulo y su incremento
		panelGiro += panelOffset * deltaTime;
		instrumentoGiro += instrumentoOffset * deltaTime;
		if (torreSube)
		{
			torreAngulo += torreOffset * deltaTime;
			if (torreAngulo >= 60.0f)
				torreSube = false;
		}
		else
		{
			torreAngulo -= torreOffset * deltaTime;
			if (torreAngulo <= -60.0f)
				torreSube = true;
		}
		if (antenaSube)
		{
			antenaAngulo += antenaOffset * deltaTime;
			if (antenaAngulo >= 45.0f)
				antenaSube = false;
		}
		else
		{
			antenaAngulo -= antenaOffset * deltaTime;
			if (antenaAngulo <= -45.0f)
				antenaSube = true;
		}


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model= glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMeshModel();



		//Rover con jerarquia: todo parte de roverM y se mueve con movCoche
		roverM = glm::mat4(1.0);
		roverM = glm::translate(roverM, glm::vec3(movCoche, -0.016f, 0.0f)); //llantas en el piso
		roverM = glm::scale(roverM, glm::vec3(0.4f, 0.4f, 0.4f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//cuerpo gris
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		model = roverM;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();
		modelaux = model; //brazo y patas cuelgan del cuerpo
		//brazo magenta
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.1716f, -0.6512f, -1.1f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Brazo_M.RenderModel();
		//pata y llanta delantera derecha rojo: la pata queda quieta y la llanta gira sobre su centro
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5716f, -0.9262f, 3.055f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PataFD_M.RenderModel();
		model = glm::translate(model, glm::vec3(3.6124f, -2.5f, 0.015f));
		model = glm::rotate(model, glm::radians(girollanta), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFD_M.RenderModel();
		//pata y llanta delantera izquierda naranja: la pata queda quieta y la llanta gira sobre su centro
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5716f, -0.9262f, -3.065f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PataFI_M.RenderModel();
		model = glm::translate(model, glm::vec3(3.6099f, -2.5f, -0.005f));
		model = glm::rotate(model, glm::radians(girollanta), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFI_M.RenderModel();
		//pata y llanta media derecha amarillo: la pata queda quieta y la llanta gira sobre su centro
		color = glm::vec3(0.9f, 0.8f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0684f, -1.5512f, 2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PataMD_M.RenderModel();
		model = glm::translate(model, glm::vec3(2.5953f, -2.1679f, 1.2989f));
		model = glm::rotate(model, glm::radians(girollanta), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMD_M.RenderModel();
		//pata y llanta media izquierda verde: la pata queda quieta y la llanta gira sobre su centro
		color = glm::vec3(0.0f, 0.7f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0684f, -1.5512f, -2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PataMI_M.RenderModel();
		model = glm::translate(model, glm::vec3(2.5928f, -2.1679f, -1.299f));
		model = glm::rotate(model, glm::radians(girollanta), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMI_M.RenderModel();
		//pata y llanta trasera derecha azul claro: la pata queda quieta y la llanta gira sobre su centro
		color = glm::vec3(0.0f, 0.5f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0684f, -1.5512f, 2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PataTD_M.RenderModel();
		model = glm::translate(model, glm::vec3(-2.5905f, -2.1679f, 1.2989f));
		model = glm::rotate(model, glm::radians(girollanta), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTD_M.RenderModel();
		//pata y llanta trasera izquierda azul: la pata queda quieta y la llanta gira sobre su centro
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0684f, -1.5512f, -2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PataTI_M.RenderModel();
		model = glm::translate(model, glm::vec3(-2.5734f, -2.1942f, -1.2989f));
		model = glm::rotate(model, glm::radians(girollanta), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTI_M.RenderModel();

		//regreso el color a blanco para que no pinte las texturas de lo que sigue
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Avion: se mueve a (avionX, avionY, avionZ) y gira en Y hacia donde apunta
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(avionX, avionY, avionZ));
		model = glm::rotate(model, glm::radians(avionGiro), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Avion_M.RenderModel();


		//Pista de carreras: pedazos del plano meshList[7], de 10 de ancho
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pistaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//recta de ida (z = 73): plana, rampa de subida y plana arriba
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-31.0f, pistaY1, pistaZ1));
		model = glm::scale(model, glm::vec3(22.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, pistaYM, pistaZ1));
		model = glm::rotate(model, rampaAngulo, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(rampaLargo, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(26.0f, pistaY2, pistaZ1));
		model = glm::scale(model, glm::vec3(32.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();
		//recta de regreso (z = -11): plana arriba, rampa de bajada y plana abajo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.0f, pistaY2, pistaZ2));
		model = glm::scale(model, glm::vec3(22.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, pistaYM, pistaZ2));
		model = glm::rotate(model, rampaAngulo, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(rampaLargo, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-26.0f, pistaY1, pistaZ2));
		model = glm::scale(model, glm::vec3(32.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();
		//las dos curvas: 36 pedazos cada una, cada pedazo gira 5 grados mas que el anterior
		for (int k = 0; k < 36; k++)
		{
			curvaAngulo = 5.0f * k + 2.5f;
			curvaX = naveRadio * sin(glm::radians(curvaAngulo));
			curvaZ = naveRadio * cos(glm::radians(curvaAngulo));
			//curva de arriba (x = 42)
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(pistaX2 + curvaX, pistaY2, pistaZM + curvaZ));
			model = glm::rotate(model, glm::radians(curvaAngulo), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(naveRadio * glm::radians(5.0f) * 1.05f, 1.0f, 10.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[7]->RenderMeshModel();
			//curva de abajo (x = -42): del otro lado y 180 grados mas
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(pistaX1 - curvaX, pistaY1, pistaZM - curvaZ));
			model = glm::rotate(model, glm::radians(180.0f + curvaAngulo), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(naveRadio * glm::radians(5.0f) * 1.05f, 1.0f, 10.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[7]->RenderMeshModel();
		}

		//Nave de carreras: se traslada, apunta hacia donde va (Y) y se inclina (Z)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(naveX, naveY, naveZ));
		model = glm::rotate(model, glm::radians(naveGiro), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(naveInclina), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		//Satelite SAC-C: cada parte tiene su origen en su union con el cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(satX, satY, satZ));
		model = glm::rotate(model, glm::radians(satRumbo), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//cuerpo
		color = glm::vec3(0.85f, 0.85f, 0.9f);
		model = modelaux;
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatCuerpo_M.RenderModel();
		//paneles solares: giran sobre su eje largo (X) desde donde se unen al cuerpo
		color = glm::vec3(0.1f, 0.25f, 0.7f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.2363f, -5.3684f, -0.0013f) * escalaSat);
		model = glm::rotate(model, glm::radians(panelGiro), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatPanelIzq_M.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.4337f, -5.3684f, -0.0013f) * escalaSat);
		model = glm::rotate(model, glm::radians(panelGiro), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatPanelDer_M.RenderModel();
		//torre de abajo: se inclina en Z de -60 a 60
		color = glm::vec3(0.9f, 0.5f, 0.1f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0432f, -8.0084f, -0.0013f) * escalaSat);
		model = glm::rotate(model, glm::radians(torreAngulo), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatTorre_M.RenderModel();
		//instrumento de arriba: gira en Y sobre la tapa del cuerpo
		color = glm::vec3(0.2f, 0.8f, 0.3f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0437f, 7.9816f, 0.4687f) * escalaSat);
		model = glm::rotate(model, glm::radians(instrumentoGiro), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatInstrumento_M.RenderModel();
		//antena: se inclina en X desde su base de -45 a 45
		color = glm::vec3(0.9f, 0.1f, 0.1f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0237f, 7.9916f, -3.4113f) * escalaSat);
		model = glm::rotate(model, glm::radians(antenaAngulo), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatAntena_M.RenderModel();
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));


		//Agave ¿qué sucede si lo renderizan antes del Rover?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMeshModel();
		
		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMeshModel();

		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		for (int i = 1; i<4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();

		 }

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();
		}
 

		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		*/
		toffsetnumerocambiau += 0.25; 
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		//cambiar automáticamente entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();
		
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
