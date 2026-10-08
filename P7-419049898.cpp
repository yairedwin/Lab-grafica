/*
Práctica 7: Iluminación 1 
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

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
// Texturas del dado y del holocron
Texture dadoTexture;
Texture holocronTexture;


//mi rover de la practica 5, por piezas para tener jerarquia
Model Cuerpo_M;
Model Brazo_M;
Model LlantaFD_M; //frontal derecha
Model LlantaFI_M; //frontal izquierda
Model LlantaMD_M; //media derecha
Model LlantaMI_M; //media izquierda
Model LlantaTD_M; //trasera derecha
Model LlantaTI_M; //trasera izquierda
//mi avion de la practica 6, en lugar del helicoptero
Model Avion_M;

//cuanto se ha movido el rover (Y y U) y el avion (flechas)
float roverX = 0.0f;
float avionX = 0.0f;
float avionY = 0.0f;



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


//función de calculo de normales por promedio de vértices 
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
	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado() //mi dado de la practica 6, con las normales invertidas como pide este shader
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};
	//la textura es una plantilla de cubo de 1024x1024, cada cara mide 256 px = 0.25 en S y T
	GLfloat cubo_vertices[] = {
		// front (Rebel Alliance)
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.25f,  0.375f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.5f,	0.375f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.5f,	0.625f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.25f,	0.625f,		0.0f,	0.0f,	-1.0f,	//3
		// right (Galactic Empire)
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.5f,  0.375f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.75f,	0.375f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.75f,	0.625f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.5f,	0.625f,		-1.0f,	0.0f,	0.0f,
		// back (First Order), las S van al reves para que no salga espejeado
		-0.5f, -0.5f, -0.5f,	1.0f,  0.375f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.75f,	0.375f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.75f,	0.625f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	1.0f,	0.625f,		0.0f,	0.0f,	1.0f,

		// left (Old Galactic Republic)
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.375f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.25f,	0.375f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.25f,	0.625f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.625f,		1.0f,	0.0f,	0.0f,

		// bottom (SITH Empire)
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.25f,  0.375f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.5f,	0.375f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.5f,	0.125f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.25f,	0.125f,		0.0f,	1.0f,	0.0f,

		//UP (Boba Fett Crest)
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.25f,  0.625f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.5f,	0.625f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.5f,	0.875f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.25f,	0.875f,		0.0f,	-1.0f,	0.0f,

	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);

}


void CrearHolocron() //mi holocron de la practica 6
{
	//holocron modificado: le quite los 4 vertices de mas que partian el cuadrado de abajo y lo deje centrado en el origen,
	//quedan 6 cuadrados y 8 triangulos y cada cara lleva sus propios vertices para tener sus coordenadas de textura
	unsigned int holocron_indices[] = {
		0, 1, 2,	2, 3, 0,
		4, 5, 6,	6, 7, 4,
		8, 9, 10,	10, 11, 8,
		12, 13, 14,	14, 15, 12,
		16, 17, 18,	18, 19, 16,
		20, 21, 22,	22, 23, 20,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35,
		36, 37, 38,
		39, 40, 41,
		42, 43, 44,
		45, 46, 47,
	};

	//la textura es de 1024x1024 en celdas de 256: los 6 cuadrados usan el rombo de su celda y los 8 triangulos el triangulo dorado
	GLfloat holocron_vertices[] = {
		//x		y		z			S		T			NX		NY		NZ
		// cuadrado de derecha
		2.50f,	2.50f,	0.00f,		0.125f,	0.995f,		-1.00f,	0.00f,	0.00f,
		2.50f,	0.00f,	2.50f,		0.005f,	0.875f,		-1.00f,	0.00f,	0.00f,
		2.50f,	-2.50f,	0.00f,		0.125f,	0.755f,		-1.00f,	0.00f,	0.00f,
		2.50f,	0.00f,	-2.50f,		0.245f,	0.875f,		-1.00f,	0.00f,	0.00f,
		// cuadrado de izquierda
		-2.50f,	2.50f,	0.00f,		0.375f,	0.995f,		1.00f,	0.00f,	0.00f,
		-2.50f,	0.00f,	-2.50f,		0.255f,	0.875f,		1.00f,	0.00f,	0.00f,
		-2.50f,	-2.50f,	0.00f,		0.375f,	0.755f,		1.00f,	0.00f,	0.00f,
		-2.50f,	0.00f,	2.50f,		0.495f,	0.875f,		1.00f,	0.00f,	0.00f,
		// cuadrado de arriba
		0.00f,	2.50f,	-2.50f,		0.625f,	0.995f,		0.00f,	-1.00f,	0.00f,
		-2.50f,	2.50f,	0.00f,		0.505f,	0.875f,		0.00f,	-1.00f,	0.00f,
		0.00f,	2.50f,	2.50f,		0.625f,	0.755f,		0.00f,	-1.00f,	0.00f,
		2.50f,	2.50f,	0.00f,		0.745f,	0.875f,		0.00f,	-1.00f,	0.00f,
		// cuadrado de abajo
		0.00f,	-2.50f,	2.50f,		0.875f,	0.995f,		0.00f,	1.00f,	0.00f,
		-2.50f,	-2.50f,	0.00f,		0.755f,	0.875f,		0.00f,	1.00f,	0.00f,
		0.00f,	-2.50f,	-2.50f,		0.875f,	0.755f,		0.00f,	1.00f,	0.00f,
		2.50f,	-2.50f,	0.00f,		0.995f,	0.875f,		0.00f,	1.00f,	0.00f,
		// cuadrado de frente
		0.00f,	2.50f,	2.50f,		0.125f,	0.745f,		0.00f,	0.00f,	-1.00f,
		-2.50f,	0.00f,	2.50f,		0.005f,	0.625f,		0.00f,	0.00f,	-1.00f,
		0.00f,	-2.50f,	2.50f,		0.125f,	0.505f,		0.00f,	0.00f,	-1.00f,
		2.50f,	0.00f,	2.50f,		0.245f,	0.625f,		0.00f,	0.00f,	-1.00f,
		// cuadrado de atras
		0.00f,	2.50f,	-2.50f,		0.375f,	0.745f,		0.00f,	0.00f,	1.00f,
		2.50f,	0.00f,	-2.50f,		0.255f,	0.625f,		0.00f,	0.00f,	1.00f,
		0.00f,	-2.50f,	-2.50f,		0.375f,	0.505f,		0.00f,	0.00f,	1.00f,
		-2.50f,	0.00f,	-2.50f,		0.495f,	0.625f,		0.00f,	0.00f,	1.00f,
		// triangulo de la esquina (+, +, +)
		2.50f,	0.00f,	2.50f,		0.519f,	0.564f,		-0.58f,	-0.58f,	-0.58f,
		2.50f,	2.50f,	0.00f,		0.731f,	0.564f,		-0.58f,	-0.58f,	-0.58f,
		0.00f,	2.50f,	2.50f,		0.625f,	0.747f,		-0.58f,	-0.58f,	-0.58f,
		// triangulo de la esquina (+, +, -)
		0.00f,	2.50f,	-2.50f,		0.769f,	0.564f,		-0.58f,	-0.58f,	0.58f,
		2.50f,	2.50f,	0.00f,		0.981f,	0.564f,		-0.58f,	-0.58f,	0.58f,
		2.50f,	0.00f,	-2.50f,		0.875f,	0.747f,		-0.58f,	-0.58f,	0.58f,
		// triangulo de la esquina (+, -, +)
		0.00f,	-2.50f,	2.50f,		0.019f,	0.314f,		-0.58f,	0.58f,	-0.58f,
		2.50f,	-2.50f,	0.00f,		0.231f,	0.314f,		-0.58f,	0.58f,	-0.58f,
		2.50f,	0.00f,	2.50f,		0.125f,	0.497f,		-0.58f,	0.58f,	-0.58f,
		// triangulo de la esquina (+, -, -)
		2.50f,	0.00f,	-2.50f,		0.269f,	0.314f,		-0.58f,	0.58f,	0.58f,
		2.50f,	-2.50f,	0.00f,		0.481f,	0.314f,		-0.58f,	0.58f,	0.58f,
		0.00f,	-2.50f,	-2.50f,		0.375f,	0.497f,		-0.58f,	0.58f,	0.58f,
		// triangulo de la esquina (-, +, +)
		0.00f,	2.50f,	2.50f,		0.519f,	0.314f,		0.58f,	-0.58f,	-0.58f,
		-2.50f,	2.50f,	0.00f,		0.731f,	0.314f,		0.58f,	-0.58f,	-0.58f,
		-2.50f,	0.00f,	2.50f,		0.625f,	0.497f,		0.58f,	-0.58f,	-0.58f,
		// triangulo de la esquina (-, +, -)
		-2.50f,	0.00f,	-2.50f,		0.769f,	0.314f,		0.58f,	-0.58f,	0.58f,
		-2.50f,	2.50f,	0.00f,		0.981f,	0.314f,		0.58f,	-0.58f,	0.58f,
		0.00f,	2.50f,	-2.50f,		0.875f,	0.497f,		0.58f,	-0.58f,	0.58f,
		// triangulo de la esquina (-, -, +)
		-2.50f,	0.00f,	2.50f,		0.019f,	0.064f,		0.58f,	0.58f,	-0.58f,
		-2.50f,	-2.50f,	0.00f,		0.231f,	0.064f,		0.58f,	0.58f,	-0.58f,
		0.00f,	-2.50f,	2.50f,		0.125f,	0.247f,		0.58f,	0.58f,	-0.58f,
		// triangulo de la esquina (-, -, -)
		0.00f,	-2.50f,	-2.50f,		0.269f,	0.064f,		0.58f,	0.58f,	0.58f,
		-2.50f,	-2.50f,	0.00f,		0.481f,	0.064f,		0.58f,	0.58f,	0.58f,
		-2.50f,	0.00f,	-2.50f,		0.375f,	0.247f,		0.58f,	0.58f,	0.58f,
	};

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(holocron_vertices, holocron_indices, 384, 60);
	meshListModel.push_back(holocron);

}



int main()
{
	mainWindow = Window(2400, 1200); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

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
	dadoTexture = Texture("Textures/logos_star_wars.png");
	dadoTexture.LoadTextureA();
	holocronTexture = Texture("Textures/holocron_textura.png");
	holocronTexture.LoadTextureA();

	Cuerpo_M = Model();
	Cuerpo_M.LoadModel("Models/Rover/Cuerpo.obj");
	Brazo_M = Model();
	Brazo_M.LoadModel("Models/Rover/Brazo.obj");
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
		0.3f, 0.3f,
		0.0f, -1.0f, -0.5f); //cae desde arriba; la deje en 0.3 como venia para que se noten los faros
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//luz puntual blanca de frente y a la derecha, ilumina las caras que ven hacia +z y +x
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-3.0f, 6.0f, 9.0f,
		1.0f, 0.05f, 0.01f);
	pointLightCount++;

	//luz puntual blanca de atras y a la izquierda, ilumina las caras que ven hacia -z y -x
	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-10.0f, 6.0f, 3.0f,
		1.0f, 0.05f, 0.01f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//faro del rover: amarillo, apunta hacia adelante (+x del rover); la posicion y la direccion
	//se ponen en cada ciclo con SetFlash a partir de la matriz del rover
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.05f, 0.01f,
		20.0f);
	spotLightCount++;

	//luz de abajo del avion: azul, ilumina hacia el piso
	spotLights[2] = SpotLight(0.2f, 0.4f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.005f,
		25.0f);
	spotLightCount++;

	//faro de enfrente del avion: verde, ilumina hacia adelante (+x del avion)
	spotLights[3] = SpotLight(0.2f, 1.0f, 0.2f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.02f, 0.005f,
		15.0f);
	spotLightCount++;
	//con la linterna son 4 spotlight, el maximo que tiene MAX_SPOT_LIGHTS

	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

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

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		//movimiento del rover (Y avanza, U retrocede) y del avion (flechas)
		bool* teclas = mainWindow.getsKeys();
		if (teclas[GLFW_KEY_Y]) roverX += 0.05f * deltaTime;
		if (teclas[GLFW_KEY_U]) roverX -= 0.05f * deltaTime;
		if (teclas[GLFW_KEY_RIGHT]) avionX += 0.1f * deltaTime;
		if (teclas[GLFW_KEY_LEFT]) avionX -= 0.1f * deltaTime;
		if (teclas[GLFW_KEY_UP]) avionY += 0.05f * deltaTime;
		if (teclas[GLFW_KEY_DOWN]) avionY -= 0.05f * deltaTime;

		//matriz del cuerpo del rover y del avion; las luces cuelgan de estas matrices igual que las piezas,
		//por eso se calculan aqui, antes de mandar las luces al shader
		glm::mat4 roverM(1.0);
		roverM = glm::translate(roverM, glm::vec3(1.0f + roverX, 1.12f, 6.0f)); //1.12 deja las llantas en el piso (y = -1)
		roverM = glm::scale(roverM, glm::vec3(0.4f, 0.4f, 0.4f));
		glm::mat4 avionM(1.0);
		avionM = glm::translate(avionM, glm::vec3(avionX, 5.0f + avionY, 0.0f));

		//faro del rover: en el frente del chasis (x = 5.2 del modelo) y apuntando hacia adelante y un poco abajo
		spotLights[1].SetFlash(glm::vec3(roverM * glm::vec4(5.2f, 0.5f, 0.0f, 1.0f)),
			glm::normalize(glm::vec3(roverM * glm::vec4(1.0f, -0.5f, 0.0f, 0.0f))));
		//luz de abajo del avion: debajo del fuselaje, apuntando al piso
		spotLights[2].SetFlash(glm::vec3(avionM * glm::vec4(0.0f, -0.9f, 0.0f, 1.0f)),
			glm::normalize(glm::vec3(avionM * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f))));
		//faro del avion: en la helice (x = 3.4) apuntando hacia adelante y un poco abajo para que se vea en el piso
		spotLights[3].SetFlash(glm::vec3(avionM * glm::vec4(3.4f, 0.0f, 0.0f, 1.0f)),
			glm::normalize(glm::vec3(avionM * glm::vec4(1.0f, -0.6f, 0.0f, 0.0f))));

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();
	
		//Rover con jerarquia: todo parte de roverM, la misma matriz de donde sale su faro
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//cuerpo gris
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		model = roverM;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();
		modelaux = model; //brazo y llantas cuelgan del cuerpo
		//brazo magenta
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.1716f, -0.6512f, -1.1f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Brazo_M.RenderModel();
		//llanta delantera derecha roja
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5716f, -0.9262f, 3.055f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFD_M.RenderModel();
		//llanta delantera izquierda naranja
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5716f, -0.9262f, -3.065f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFI_M.RenderModel();
		//llanta media derecha amarilla
		color = glm::vec3(0.9f, 0.8f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0684f, -1.5512f, 2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMD_M.RenderModel();
		//llanta media izquierda verde
		color = glm::vec3(0.0f, 0.7f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0684f, -1.5512f, -2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMI_M.RenderModel();
		//llanta trasera derecha azul claro
		color = glm::vec3(0.0f, 0.5f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0684f, -1.5512f, 2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTD_M.RenderModel();
		//llanta trasera izquierda azul
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0684f, -1.5512f, -2.86f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTI_M.RenderModel();

		//regreso el color a blanco para que no pinte las texturas de lo que sigue
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Avion: su matriz es avionM, la misma de sus dos luces
		model = avionM;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Avion_M.RenderModel();



		//Dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[4]->RenderMeshModel();

		//Holocron
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, 7.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocronTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[5]->RenderMeshModel();


		


		//Agave ¿qué sucede si lo renderizan antes del helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
