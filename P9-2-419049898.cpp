/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
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
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float angulovaria = 0.0f;
float segundos = 0.0f;             //lo que dura cada ciclo en segundos reales
float tiempo = 0.0f;               //segundos reales desde que abrió la ventana

//variables del holocron: los triangulos se separan, dan vueltas y regresan (el gif del profe)
float holoEsc = 6.0f;              //escala del holocron (el modelo mide 5)
float holoDistMax = 4.5f;          //cuanto se separan los triangulos, en unidades del modelo
float holoPeriodo = 7.4f;          //segundos que dura un ciclo completo (abrir y cerrar)
float holoFase = 0.0f;             //en que parte del ciclo va, de 0 a 1
float holoS = 0.0f;                //0.5 - 0.5 cos(2 pi fase): sube de 0 a 1 y baja de 1 a 0
float holoAbre = 0.0f;             //que tan abierto esta, de 0 (cerrado) a 1 (todo abierto)
float holoDist = 0.0f;             //distancia a la que viaja cada triangulo
float holoVuelta = 0.0f;           //giro de cada triangulo sobre si mismo
float holoGiro = 0.0f;             //giro de todo el holocron alrededor de Y
float holoAltura = 0.0f;           //sube y baja flotando
float holoCentroide = 1.667f;      //cada triangulo esta en (+-1.667, +-1.667, +-1.667)
glm::vec3 holoSigno[8] = {         //octante de cada triangulo, en el orden del arreglo
	glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f) };
glm::vec3 holoDir(0.0f);           //hacia donde se aleja el triangulo que se esta dibujando
glm::vec3 holoEje(0.0f);           //eje sobre el que da vueltas ese triangulo
glm::vec3 holoCentro(0.0f);        //su centro, para girarla sobre si misma
int holoI = 0;                     //triangulo que se esta dibujando
glm::mat4 holoM(1.0);

//variables del cohete: la trayectoria sale de ecuaciones que se integran en cada ciclo
float cohEsc = 4.5f;               //escala del cohete (9 de alto en el modelo)
float cohPadX = 25.0f;             //donde esta la pista
float cohPadZ = -10.0f;
float cohY0 = 18.25f;              //altura del centro sobre la pista (-2 + 4.5 * escala)
float cohCuenta = 5.0f;            //segundos de cuenta regresiva antes de despegar
float cohFin = 30.0f;              //segundos de vuelo con motor, despues se reinicia
float cohG = 0.25f;                //gravedad
float cohA0 = 0.45f;               //aceleracion del empuje al inicio (empuje / masa)
float cohQuema = 0.012f;           //fraccion de masa que se gasta por segundo
float cohTGiro = 5.0f;             //cuando empieza a inclinarse
float cohDurGiro = 24.0f;          //cuanto dura la inclinacion
float cohFiMax = 62.0f;            //inclinacion final del empuje, desde la vertical
float cohT = 0.0f;                 //reloj de la secuencia
float cohTs = 0.0f;                //segundos desde el despegue
float cohX = 0.0f, cohY = 0.0f;    //desplazamiento desde la pista
float cohVX = 0.0f, cohVY = 0.0f;  //velocidad
float cohMasa = 1.0f;
float cohMasaSep = 0.0f;           //masa que se tira al soltar una etapa
float cohA = 0.0f;                 //aceleracion del empuje en este instante
float cohU = 0.0f;
float cohFi = 0.0f;                //direccion del empuje
float cohInc = 0.0f;               //inclinacion del cohete, la de su velocidad
float cohRoll = 0.0f;              //vuelta sobre su propio eje
float cohTemblor = 0.0f;
float cohPX = 0.0f, cohPY = 0.0f, cohPZ = 0.0f;
glm::mat4 cohM(1.0);
//etapas: 0 es la de abajo (primera) y 1 la de en medio, se sueltan a los 14 y 27 segundos
bool sepHecha[2] = { false, false };
float sepTs[2] = { 14.0f, 27.0f };
float sepMasa[2] = { 0.15f, 0.08f };
float sepPivote[2] = { -2.9f, 0.1f };   //centro de cada etapa, para girarla
float sepX[2], sepY[2], sepVX[2], sepVY[2], sepInc[2], sepVGiro[2];
float cohBase[3] = { -4.5f, -1.0f, 1.25f };  //tobera de cada etapa en el modelo
float llamaAncho[3] = { 10.0f, 6.5f, 4.4f };
float llamaLargo[3] = { 24.0f, 15.0f, 9.5f };
int cohEtapa = 0;                  //etapa que esta encendida
int sepK = 0;                      //etapa que se esta revisando o dibujando
float llamaBase = 0.0f;            //donde empieza la llama, en el cohete ya escalado
float llamaAn = 0.0f;              //ancho de la llama de esta etapa
glm::vec3 ejeZ(0.0f, 0.0f, 1.0f);
float llamaIgn = 0.0f;             //0 apagada, 1 encendida del todo
float llamaLong = 0.0f;
int llamaCuadro = 0;
Model* etapaM[2];

//variables del humo: cada bocanada vive humoVida segundos y cambia de cuadro con su edad
#define MAX_HUMO 300
glm::vec3 humoPos[MAX_HUMO];       //donde esta cada bocanada
glm::vec3 humoVel[MAX_HUMO];       //hacia donde se mueve
float humoEdad[MAX_HUMO];          //segundos que lleva viva; al llegar a humoVida deja de existir
float humoVida = 4.0f;
float humoCada = 0.03f;            //cada cuantos segundos sale humo de la tobera
float humoAcum = 0.0f;
int humoSig = 0;                   //siguiente lugar del arreglo que se va a usar
int humoCont = 0;
int humoCuadro = 0;
float humoTam = 0.0f;
float humoAng = 0.0f;
float humoDisp = 0.0f;
glm::vec3 tobera(0.0f);
float camX = 0.0f;                 //posicion de la camara, para que el humo la mire
float camZ = 0.0f;

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
Texture HumoTexture;
Texture LlamaTexture;
Texture HoloTexture;



//cohete Saturn V partido en 3 etapas, con las coordenadas del modelo completo
Model Etapa1_M;
Model Etapa2_M;
Model Etapa3_M;



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

	//plano vertical del humo: usa 1/8 de la textura, el resto se recorre con el offset
	unsigned int humoIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLfloat humoVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		0.125f, 0.0f,	0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		0.125f, 1.0f,	0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	MeshModel* obj8 = new MeshModel();
	obj8->CreateMeshModel(humoVertices, humoIndices, 32, 6);
	meshList.push_back(obj8); // humo

	//dos planos cruzados para la llama, cuelgan del origen y usan 1/8 de la textura
	unsigned int llamaIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};

	GLfloat llamaVertices[] = {
		-0.5f, -1.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -1.0f, 0.0f,		0.125f, 0.0f,	0.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,		0.125f, 1.0f,	0.0f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -1.0f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.5f,		0.125f, 0.0f,	0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f,		0.125f, 1.0f,	0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	MeshModel* obj9 = new MeshModel();
	obj9->CreateMeshModel(llamaVertices, llamaIndices, 64, 12);
	meshList.push_back(obj9); // llama

	//holocron de mi practica 6: 6 cuadrados y 8 triangulos, cada cara con sus vertices y su textura
	unsigned int holocronIndices[] = {
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

	GLfloat holocronVertices[] = {
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
	holocron->CreateMeshModel(holocronVertices, holocronIndices, 384, 36);
	meshList.push_back(holocron); // cuerpo del holocron: los 6 cuadrados

	//cada triangulo es una malla propia con los mismos vertices, para poder separarlo del cuerpo
	for (int i = 0; i < 8; i++)
	{
		unsigned int trianguloIndices[] = { 24 + 3 * i, 25 + 3 * i, 26 + 3 * i };
		MeshModel* triangulo = new MeshModel();
		triangulo->CreateMeshModel(holocronVertices, trianguloIndices, 384, 3);
		meshList.push_back(triangulo); // triangulos del holocron: meshList[10] a meshList[17]
	}

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

	camera = Camera(glm::vec3(10.0f, 62.0f, 170.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.5f, 0.5f);

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
	HumoTexture = Texture("Textures/humo.png");
	HumoTexture.LoadTextureA();
	LlamaTexture = Texture("Textures/llama_atlas.png");
	LlamaTexture.LoadTextureA();
	HoloTexture = Texture("Textures/holocron_textura.png");
	HoloTexture.LoadTextureA();

	Etapa1_M = Model();
	Etapa1_M.LoadModel("Models/Cohete/Etapa1.obj"); //sus texturas las lee de saturn_v.mtl
	Etapa2_M = Model();
	Etapa2_M.LoadModel("Models/Cohete/Etapa2.obj");
	Etapa3_M = Model();
	Etapa3_M.LoadModel("Models/Cohete/Etapa3.obj");
	etapaM[0] = &Etapa1_M;
	etapaM[1] = &Etapa2_M;


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
		0.6f, 0.8f,
		-0.3f, -0.5f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.01f,
		5.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//al empezar no hay humo: todas las bocanadas ya cumplieron su vida
	for (humoCont = 0; humoCont < MAX_HUMO; humoCont++)
		humoEdad[humoCont] = humoVida;


	glm::vec3 lowerLight(0.0f,0.0f,0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		segundos = now - lastTime;
		if (segundos > 0.1f)
			segundos = 0.1f;
		lastTime = now;
		tiempo += segundos;

		angulovaria += 0.5f*deltaTime;

		//holocron: todo sale de funciones del tiempo, sin banderas
		//fase: de 0 a 1 en cada ciclo; holoS sube de 0 a 1 y baja de 1 a 0 con un coseno
		holoFase = fmod(tiempo, holoPeriodo) / holoPeriodo;
		holoS = 0.5f - 0.5f * cos(2 * 3.14159265f * holoFase);
		//seno de un cuarto de onda: arranca suave y se queda un rato arriba, como en el gif
		holoAbre = sin(0.5f * 3.14159265f * holoS);
		holoDist = holoDistMax * holoAbre;
		holoVuelta = 540.0f * holoAbre;
		holoGiro = 25.0f * tiempo;
		holoAltura = 36.0f + 2.0f * sin(0.8f * tiempo);

		//cohete: reloj de la secuencia; pasado el vuelo y 3 s mas se reinicia todo
		cohT += segundos;
		if (cohT > cohCuenta + cohFin + 3.0f)
		{
			cohT = 0.0f;
			cohX = 0.0f; cohY = 0.0f;
			cohVX = 0.0f; cohVY = 0.0f;
			cohMasaSep = 0.0f;
			sepHecha[0] = false;
			sepHecha[1] = false;
		}
		cohTs = cohT - cohCuenta;
		cohEtapa = 0;
		if (sepHecha[0])
			cohEtapa = 1;
		if (sepHecha[1])
			cohEtapa = 2;

		//vuelo con motor: baja la masa y crece la aceleracion; el empuje se inclina con una curva
		if (cohTs > 0.0f && cohTs < cohFin)
		{
			cohMasa = 1.0f - cohQuema * cohTs - cohMasaSep;
			cohA = cohA0 / cohMasa;
			cohU = (cohTs - cohTGiro) / cohDurGiro;
			if (cohU < 0.0f)
				cohU = 0.0f;
			if (cohU > 1.0f)
				cohU = 1.0f;
			cohFi = cohFiMax * toRadians * (3 * cohU * cohU - 2 * cohU * cohU * cohU);
			cohVX += cohA * sin(cohFi) * segundos;
			cohVY += (cohA * cos(cohFi) - cohG) * segundos;
			cohX += cohVX * segundos;
			cohY += cohVY * segundos;
		}
		else if (cohTs >= cohFin)
		{
			//sin motor solo actua la gravedad
			cohVY -= cohG * segundos;
			cohX += cohVX * segundos;
			cohY += cohVY * segundos;
		}
		//el cohete apunta hacia donde va, la derivada de su trayectoria
		if (cohVY > 0.0f || cohVX > 0.0f)
			cohInc = glm::degrees(atan2(cohVX, cohVY));
		else
			cohInc = 0.0f;
		//antes de despegar el motor lo hace temblar, ya en el aire da vueltas sobre su eje
		cohTemblor = 0.0f;
		if (cohTs > -2.0f && cohTs < 0.0f)
			cohTemblor = 0.12f * sin(90.0f * cohT) * (cohTs + 2.0f) / 2.0f;
		cohRoll = 18.0f * cohTs;
		if (cohRoll < 0.0f)
			cohRoll = 0.0f;
		cohPX = cohPadX + cohX + cohTemblor;
		cohPY = cohY0 + cohY;
		cohPZ = cohPadZ;
		cohM = glm::mat4(1.0);
		cohM = glm::translate(cohM, glm::vec3(cohPX, cohPY, cohPZ));
		cohM = glm::rotate(cohM, glm::radians(-cohInc), ejeZ);

		//separacion de etapas: sale con la velocidad del cohete y un empujon hacia atras
		for (sepK = 0; sepK < 2; sepK++)
		{
			if (!sepHecha[sepK] && cohTs >= sepTs[sepK])
			{
				sepHecha[sepK] = true;
				cohMasaSep += sepMasa[sepK];
				sepX[sepK] = cohPX + cohEsc * sepPivote[sepK] * sin(cohInc * toRadians);
				sepY[sepK] = cohPY + cohEsc * sepPivote[sepK] * cos(cohInc * toRadians);
				sepVX[sepK] = cohVX - 0.8f * sin(cohInc * toRadians);
				sepVY[sepK] = cohVY - 0.8f * cos(cohInc * toRadians);
				sepInc[sepK] = cohInc;
				sepVGiro[sepK] = 20.0f + 15.0f * sepK;
			}
			//ya suelta, es un tiro libre: solo la gravedad cambia su velocidad
			if (sepHecha[sepK])
			{
				sepVY[sepK] -= cohG * segundos;
				sepX[sepK] += sepVX[sepK] * segundos;
				sepY[sepK] += sepVY[sepK] * segundos;
				sepInc[sepK] += sepVGiro[sepK] * segundos;
			}
		}

		//llama: se enciende 2 s antes del despegue, su largo depende de la etapa y parpadea
		llamaIgn = (cohTs + 2.0f) / 1.5f;
		if (llamaIgn < 0.0f)
			llamaIgn = 0.0f;
		if (llamaIgn > 1.0f)
			llamaIgn = 1.0f;
		if (cohTs >= cohFin)
			llamaIgn = 0.0f;
		llamaLong = llamaLargo[cohEtapa] * llamaIgn * (1.0f + 0.1f * sin(50.0f * cohT));
		llamaAn = llamaAncho[cohEtapa];
		llamaBase = cohBase[cohEtapa] * cohEsc;
		llamaCuadro = (int)(cohT * 24.0f) % 8;

		//humo: cada humoCada segundos sale una bocanada de la tobera, cerca del suelo se abre
		tobera = glm::vec3(cohM * glm::vec4(0.0f, llamaBase, 0.0f, 1.0f));
		if (tobera.y < -1.5f)
			tobera.y = -1.5f;
		if (llamaIgn > 0.0f)
			humoAcum += segundos;
		while (humoAcum >= humoCada)
		{
			humoAcum -= humoCada;
			humoDisp = 3.0f;
			if (cohY < 10.0f)
				humoDisp = 18.0f;
			humoPos[humoSig] = tobera;
			humoVel[humoSig] = glm::vec3(((rand() % 1000) / 1000.0f - 0.5f) * humoDisp,
				0.4f + (rand() % 1000) / 1000.0f,
				((rand() % 1000) / 1000.0f - 0.5f) * humoDisp);
			humoEdad[humoSig] = 0.0f;
			humoSig = (humoSig + 1) % MAX_HUMO;
		}
		//cada bocanada envejece, avanza y se frena un poco; al llegar a humoVida ya no se dibuja
		for (humoCont = 0; humoCont < MAX_HUMO; humoCont++)
		{
			if (humoEdad[humoCont] < humoVida)
			{
				humoEdad[humoCont] += segundos;
				humoPos[humoCont] += humoVel[humoCont] * segundos;
				humoVel[humoCont] *= (1.0f - 0.8f * segundos);
			}
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

		//pista de lanzamiento: el mismo piso, mas chico y de tierra, un poco arriba para que no se encimen
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(cohPadX, -1.95f, cohPadZ));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		meshList[2]->RenderMeshModel();



		//Holocron con jerarquia: holoM lo coloca, lo hace flotar y lo gira alrededor de Y
		holoM = glm::mat4(1.0);
		holoM = glm::translate(holoM, glm::vec3(-62.0f, holoAltura, cohPadZ));
		holoM = glm::rotate(holoM, glm::radians(holoGiro), glm::vec3(0.0f, 1.0f, 0.0f));
		holoM = glm::scale(holoM, glm::vec3(holoEsc, holoEsc, holoEsc));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//cuerpo (los 6 cuadrados): no se mueve respecto al holocron
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = holoM;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloTexture.UseTexture();
		meshList[9]->RenderMeshModel();
		//los 8 triangulos: cada uno se aleja por la diagonal de su octante y gira sobre su centro
		for (holoI = 0; holoI < 8; holoI++)
		{
			holoDir = glm::normalize(holoSigno[holoI]);
			holoCentro = holoSigno[holoI] * holoCentroide;
			holoEje = glm::cross(holoSigno[holoI], glm::vec3(0.3f, 1.0f, 0.2f));
			holoEje = glm::normalize(holoEje);
			model = holoM;
			model = glm::translate(model, holoDir * holoDist);
			model = glm::translate(model, holoCentro);
			model = glm::rotate(model, glm::radians(holoVuelta), holoEje);
			model = glm::translate(model, -holoCentro);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[10 + holoI]->RenderMeshModel();
		}

		//regreso el color a blanco para que no pinte las texturas de lo que sigue
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Cohete: la etapa de arriba siempre va; las de abajo solo si no se han soltado
		model = cohM;
		model = glm::rotate(model, glm::radians(cohRoll), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(cohEsc, cohEsc, cohEsc));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Etapa3_M.RenderModel();
		if (!sepHecha[1])
			Etapa2_M.RenderModel();
		if (!sepHecha[0])
			Etapa1_M.RenderModel();
		//etapas sueltas: caen con su posicion y giran sobre su centro hasta llegar a la pista
		for (sepK = 0; sepK < 2; sepK++)
		{
			if (sepHecha[sepK] && sepY[sepK] > 0.0f)
			{
				model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3(sepX[sepK], sepY[sepK], cohPadZ));
				model = glm::rotate(model, glm::radians(-sepInc[sepK]), ejeZ);
				model = glm::scale(model, glm::vec3(cohEsc, cohEsc, cohEsc));
				model = glm::translate(model, glm::vec3(0.0f, -sepPivote[sepK], 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				etapaM[sepK]->RenderModel();
			}
		}

		//Humo y llama llevan blending y no escriben en el buffer de profundidad
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		camX = camera.getCameraPosition().x;
		camZ = camera.getCameraPosition().z;

		//Humo: cada bocanada es un plano que mira a la camara; su cuadro depende de su edad
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		HumoTexture.UseTexture();
		color = glm::vec3(1.8f, 1.8f, 1.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (humoCont = 0; humoCont < MAX_HUMO; humoCont++)
		{
			if (humoEdad[humoCont] < humoVida)
			{
				humoCuadro = (int)(humoEdad[humoCont] / humoVida * 8.0f);
				if (humoCuadro > 7)
					humoCuadro = 7;
				toffset = glm::vec2(humoCuadro / 8.0f, 0.0f);
				glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
				humoTam = 4.0f + 12.0f * humoEdad[humoCont] / humoVida;
				humoAng = atan2(camX - humoPos[humoCont].x, camZ - humoPos[humoCont].z);
				model = glm::mat4(1.0);
				model = glm::translate(model, humoPos[humoCont]);
				model = glm::rotate(model, humoAng, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(humoTam, humoTam, humoTam));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[7]->RenderMeshModel();
			}
		}

		//Llama: textura animada de 8 cuadros, se suma a lo de atras (blending aditivo)
		if (llamaIgn > 0.0f)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			toffset = glm::vec2(llamaCuadro / 8.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			color = glm::vec3(2.4f, 2.4f, 2.4f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = cohM;
			model = glm::translate(model, glm::vec3(0.0f, llamaBase, 0.0f));
			model = glm::scale(model, glm::vec3(llamaAn, llamaLong, llamaAn));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			LlamaTexture.UseTexture();
			meshList[8]->RenderMeshModel();
		}
		//el offset vuelve a 0 para que nada mas se mueva
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
