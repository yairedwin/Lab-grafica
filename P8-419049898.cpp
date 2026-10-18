/*
Práctica 8: Iluminación 2 
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




//la nave espacial que modele en Blender, con dos motores (nave_p8.obj, sus texturas las lee de nave_p8.mtl)
Model Nave_M;
//textura de la flama azul de los motores, se dibuja en dos planos cruzados, el mismo mesh de la vegetacion
Texture flamaTexture;

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

	Nave_M = Model();
	Nave_M.LoadModel("Models/nave_p8.obj"); //nave espacial con dos motores, sus texturas estan en nave_p8.mtl

	flamaTexture = Texture("Textures/llama_azul.png"); //plasma azul con transparencia (PNG con alfa)
	flamaTexture.LoadTextureA();


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
		0.0f, -1.0f, -0.5f); //cae desde arriba
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//luz puntual de la nave, blanca calida y arriba de la cabina; su posicion se pone en cada ciclo con la matriz de la nave
	pointLights[0] = PointLight(1.0f, 0.9f, 0.7f,
		0.1f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.004f, 0.02f);
	pointLightCount++;

	//luz puntual azul de la flama del motor 1 (el derecho)
	pointLights[1] = PointLight(0.2f, 0.5f, 1.0f,
		0.05f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.02f, 0.04f);
	pointLightCount++;

	//luz puntual azul de la flama del motor 2 (el izquierdo), con valores distintos a la del motor 1
	pointLights[2] = PointLight(0.3f, 0.6f, 1.0f,
		0.1f, 1.8f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.01f, 0.03f);
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

	//spotlight de la nave, debajo del casco y apuntando al piso; su posicion y direccion se ponen en cada ciclo
	spotLights[1] = SpotLight(1.0f, 0.95f, 0.8f,
		0.0f, 1.2f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.01f, 0.005f,
		30.0f);
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	//variables que se usan dentro del while, declaradas aqui afuera para que no se creen en cada ciclo; adentro solo se les asigna
	glm::mat4 naveM(1.0);
	glm::mat4 flamaM(1.0);
	glm::vec3 posLuz(0.0f);
	PointLight activas[3];
	unsigned int cuantas = 0;
	SpotLight spotsActivas[2];
	unsigned int cuantasSpot = 0;
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


		//matriz de la nave: flotando a 3.8 sobre el piso (el piso esta en y = -1, y el modelo esta centrado en su origen); la luz de arriba, la spot de abajo y las flamas cuelgan de esta matriz
		naveM = glm::mat4(1.0);
		naveM = glm::translate(naveM, glm::vec3(6.0f, 2.8f, -8.0f));
		naveM = glm::rotate(naveM, 30 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		naveM = glm::scale(naveM, glm::vec3(0.9f, 0.9f, 0.9f));

		//luz puntual de la nave, arriba de la cabina (no esta en los motores)
		posLuz = glm::vec3(naveM * glm::vec4(1.7f, 1.04f, 0.0f, 1.0f));
		pointLights[0] = PointLight(1.0f, 0.9f, 0.7f, 0.1f, 1.0f,
			posLuz.x, posLuz.y, posLuz.z, 1.0f, 0.004f, 0.02f);
		//luz de cada flama, justo atras de la tobera de su motor (en el modelo las toberas estan en x = -4.2 y los motores en z = 2.1 y -2.1)
		posLuz = glm::vec3(naveM * glm::vec4(-4.9f, -0.66f, 2.1f, 1.0f));
		pointLights[1] = PointLight(0.2f, 0.5f, 1.0f, 0.05f, 1.5f,
			posLuz.x, posLuz.y, posLuz.z, 1.0f, 0.02f, 0.04f);
		posLuz = glm::vec3(naveM * glm::vec4(-4.9f, -0.66f, -2.1f, 1.0f));
		pointLights[2] = PointLight(0.3f, 0.6f, 1.0f, 0.1f, 1.8f,
			posLuz.x, posLuz.y, posLuz.z, 1.0f, 0.01f, 0.03f);
		//spot de la nave: debajo del casco, apuntando al piso
		spotLights[1].SetFlash(glm::vec3(naveM * glm::vec4(0.5f, -1.46f, 0.0f, 1.0f)),
			glm::normalize(glm::vec3(naveM * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f))));

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		//para apagar de verdad una luz, el shader no tiene que calcularla: solo le mando las prendidas y cuento cuantas son.
		//N apaga al mismo tiempo la luz puntual y la spot de la nave; 1 y 2 prenden y apagan cada motor (su luz y su flama)
		cuantas = 0;
		if (mainWindow.getnaveEnergia()) { activas[cuantas] = pointLights[0]; cuantas++; }
		if (mainWindow.getmotor1()) { activas[cuantas] = pointLights[1]; cuantas++; }
		if (mainWindow.getmotor2()) { activas[cuantas] = pointLights[2]; cuantas++; }
		shaderList[0].SetPointLights(activas, cuantas);
		cuantasSpot = 0;
		spotsActivas[cuantasSpot] = spotLights[0]; //la linterna siempre va
		cuantasSpot++;
		if (mainWindow.getnaveEnergia()) { spotsActivas[cuantasSpot] = spotLights[1]; cuantasSpot++; }
		shaderList[0].SetSpotLights(spotsActivas, cuantasSpot);



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
	
		//Nave: su matriz es naveM, la misma de donde salen sus luces y sus flamas
		model = naveM;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();



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

		//Flamas de los motores: dos planos cruzados con la textura de plasma azul. Solo se dibujan si el motor esta prendido,
		//y se suman al fondo (GL_ONE) para que brillen; no escriben en el buffer de profundidad para no taparse entre ellos
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		color = glm::vec3(3.0f, 3.0f, 3.0f); //multiplica a la textura y la hace mas brillante
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		flamaTexture.UseTexture();
		if (mainWindow.getmotor1())
		{
			flamaM = naveM;
			flamaM = glm::translate(flamaM, glm::vec3(-6.0f, -0.66f, 2.1f)); //sale de la tobera hacia atras
			flamaM = glm::rotate(flamaM, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //la punta del plano apunta hacia atras (-x)
			flamaM = glm::scale(flamaM, glm::vec3(1.0f, 3.6f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(flamaM));
			meshListModel[3]->RenderMeshModel();
		}
		if (mainWindow.getmotor2())
		{
			flamaM = naveM;
			flamaM = glm::translate(flamaM, glm::vec3(-6.0f, -0.66f, -2.1f));
			flamaM = glm::rotate(flamaM, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			flamaM = glm::scale(flamaM, glm::vec3(1.0f, 3.6f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(flamaM));
			meshListModel[3]->RenderMeshModel();
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
