/*
Pr�ctica 5: Optimizaci�n y Carga de Modelos
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
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;
//Lista de Modelos a importar, uno por cada pieza de la jerarquia
Model Cuerpo_M;
Model Brazo_M;
Model LlantaFD_M; //frontal derecha
Model LlantaFI_M; //frontal izquierda
Model LlantaMD_M; //media derecha
Model LlantaMI_M; //media izquierda
Model LlantaTD_M; //trasera derecha
Model LlantaTI_M; //trasera izquierda

//Holocron: el centro y sus 8 esquinas (todas las piezas tienen su origen en el centro del holocron)
Model HoloCentro_M;
Model HoloEsquina1_M;
Model HoloEsquina2_M;
Model HoloEsquina3_M;
Model HoloEsquina4_M;
Model HoloEsquina5_M;
Model HoloEsquina6_M;
Model HoloEsquina7_M;
Model HoloEsquina8_M;

//Satelite SAC-C: cuerpo y las partes que giran en su union con el cuerpo
Model SatCuerpo_M;
Model SatPanelIzq_M;
Model SatPanelDer_M;
Model SatTorre_M;
Model SatInstrumento_M;
Model SatAntena_M;

//Lista de Skybox a crear
Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";


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

	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(2400, 1200); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	//camara de frente y un poco atras para que al abrir se vean juntos el rover, el holocron y el satelite
	camera = Camera(glm::vec3(1.0f, 7.0f, 40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -4.0f, 0.3f, 0.3f);
	//Cargar modelos, ya las 7 piezas separadas en Blender
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

	//Holocron
	HoloCentro_M = Model();
	HoloCentro_M.LoadModel("Models/Holocron/HoloCentro.obj");
	HoloEsquina1_M = Model();
	HoloEsquina1_M.LoadModel("Models/Holocron/HoloEsquina1.obj");
	HoloEsquina2_M = Model();
	HoloEsquina2_M.LoadModel("Models/Holocron/HoloEsquina2.obj");
	HoloEsquina3_M = Model();
	HoloEsquina3_M.LoadModel("Models/Holocron/HoloEsquina3.obj");
	HoloEsquina4_M = Model();
	HoloEsquina4_M.LoadModel("Models/Holocron/HoloEsquina4.obj");
	HoloEsquina5_M = Model();
	HoloEsquina5_M.LoadModel("Models/Holocron/HoloEsquina5.obj");
	HoloEsquina6_M = Model();
	HoloEsquina6_M.LoadModel("Models/Holocron/HoloEsquina6.obj");
	HoloEsquina7_M = Model();
	HoloEsquina7_M.LoadModel("Models/Holocron/HoloEsquina7.obj");
	HoloEsquina8_M = Model();
	HoloEsquina8_M.LoadModel("Models/Holocron/HoloEsquina8.obj");

	//Satelite
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

	//Crear Skybox con sus 6 texturas
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

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
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		/* INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[2]->RenderMeshModel();
		*/

		//------------*INICIA DIBUJO DE NUESTROS DEM�S OBJETOS-------------------*
		//Cuerpo del rover
		//lo pongo en el mismo lugar donde la plantilla dibujaba el rover completo (llantas tocando y = -2),
		//como en Blender le deje el origen en su centro, aqui le sumo ese centro (-1.4716, 5.1312, 0)
		color = glm::vec3(0.6f, 0.6f, 0.6f); //cuerpo gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.4716f, 3.1312f, -1.5f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		//guardo la matriz del cuerpo aqui, ya trasladada, para que todo lo que cuelgue de el
		//(brazo y llantas) parta de este mismo punto y nunca se reinicie a la identidad
		modelaux = model;

		//Brazo (solo jerarquia, el enunciado no pide que rote en esta practica)
		//el offset es de la base de la torreta del brazo, donde se apoya en el chasis
		color = glm::vec3(1.0f, 0.0f, 1.0f); //brazo magenta
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.1716f, -0.6512f, -1.1f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Brazo_M.RenderModel();

		//Cada llanta es la rueda completa con su pata (llanta, rin, eje y barras),
		//y el offset es el perno de arriba de la pata, que es donde gira.
		//Rota en Z porque en este modelo X es adelante/atras y Z es izquierda/derecha,
		//asi la pata se mueve hacia adelante y hacia atras como si fuera avanzando

		//Llanta delantera derecha (articulacion 4, tecla J)
		color = glm::vec3(1.0f, 0.0f, 0.0f); //rojo
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5716f, -0.9262f, 3.055f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFD_M.RenderModel();

		//Llanta delantera izquierda (articulacion 5, tecla K)
		color = glm::vec3(1.0f, 0.5f, 0.0f); //naranja
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5716f, -0.9262f, -3.065f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFI_M.RenderModel();

		//Llanta media derecha (articulacion 6, tecla L)
		color = glm::vec3(0.9f, 0.8f, 0.0f); //amarillo
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0684f, -1.5512f, 2.86f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMD_M.RenderModel();

		//Llanta media izquierda (articulacion 7, tecla I)
		color = glm::vec3(0.0f, 0.7f, 0.0f); //verde
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0684f, -1.5512f, -2.86f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMI_M.RenderModel();

		//Llanta trasera derecha (articulacion 8, tecla M)
		color = glm::vec3(0.0f, 0.5f, 1.0f); //azul claro
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0684f, -1.5512f, 2.86f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTD_M.RenderModel();

		//Llanta trasera izquierda (articulacion 9, tecla N)
		color = glm::vec3(0.0f, 0.0f, 1.0f); //azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0684f, -1.5512f, -2.86f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTI_M.RenderModel();

		//------------------------------ HOLOCRON ------------------------------
		//en Blender le deje a todas las piezas el origen en el centro del holocron,
		//asi cada esquina gira alrededor del centro sin tener que trasladarla primero.
		//Orden translate -> rotate -> scale, y la matriz auxiliar se guarda antes de escalar
		GLfloat escalaHolo = 0.5f;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-17.0f, 0.64f, -3.0f)); //a un lado del rover, apoyado en y = -2
		modelaux = model;

		//Centro (cristal, interior y caras)
		color = glm::vec3(0.15f, 0.3f, 0.75f); //azul
		model = modelaux;
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloCentro_M.RenderModel();

		//Esquinas: cada una gira en Y alrededor del centro del holocron, con un dorado distinto para distinguirlas
		//Esquina 1 (arriba, +x +z), tecla 1
		color = glm::vec3(1.0f, 0.84f, 0.0f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina1()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina1_M.RenderModel();

		//Esquina 2 (arriba, -x +z), tecla 2
		color = glm::vec3(0.85f, 0.55f, 0.1f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina2()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina2_M.RenderModel();

		//Esquina 3 (arriba, -x -z), tecla 3
		color = glm::vec3(1.0f, 0.7f, 0.3f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina3()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina3_M.RenderModel();

		//Esquina 4 (arriba, +x -z), tecla 4
		color = glm::vec3(0.7f, 0.45f, 0.05f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina4()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina4_M.RenderModel();

		//Esquina 5 (abajo, +x +z), tecla 5
		color = glm::vec3(0.95f, 0.8f, 0.4f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina5()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina5_M.RenderModel();

		//Esquina 6 (abajo, -x +z), tecla 6
		color = glm::vec3(0.8f, 0.6f, 0.2f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina6()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina6_M.RenderModel();

		//Esquina 7 (abajo, -x -z), tecla 7
		color = glm::vec3(1.0f, 0.6f, 0.1f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina7()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina7_M.RenderModel();

		//Esquina 8 (abajo, +x -z), tecla 8
		color = glm::vec3(0.6f, 0.5f, 0.15f);
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getesquina8()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHolo, escalaHolo, escalaHolo));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloEsquina8_M.RenderModel();

		//------------------------------ SATELITE ------------------------------
		//el cuerpo tiene su origen en su centro; cada parte tiene su origen en la union con el cuerpo.
		//Los offsets estan en las unidades del modelo, por eso se multiplican por la escala
		GLfloat escalaSat = 0.35f;
		model = glm::mat4(1.0);
		//posicion inicial flotando arriba del rover, mas lo que se mueva con flechas y O/P
		model = glm::translate(model, glm::vec3(18.0f + mainWindow.getsatX(), 14.0f + mainWindow.getsatY(), -6.0f + mainWindow.getsatZ()));
		//giro de todo el satelite en los 3 ejes (teclas X, Y, Z)
		model = glm::rotate(model, glm::radians(mainWindow.getsatGiroX()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getsatGiroY()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getsatGiroZ()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		//Cuerpo
		color = glm::vec3(0.85f, 0.85f, 0.9f); //gris claro
		model = modelaux;
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatCuerpo_M.RenderModel();

		//Panel solar izquierdo (tecla C): gira sobre su eje largo (X) desde donde sus puntales se unen al cuerpo
		color = glm::vec3(0.1f, 0.25f, 0.7f); //azul de celdas solares
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.2363f, -5.3684f, -0.0013f) * escalaSat);
		model = glm::rotate(model, glm::radians(mainWindow.getpanelIzq()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatPanelIzq_M.RenderModel();

		//Panel solar derecho (tecla V)
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.4337f, -5.3684f, -0.0013f) * escalaSat);
		model = glm::rotate(model, glm::radians(mainWindow.getpanelDer()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatPanelDer_M.RenderModel();

		//Torre de abajo (tecla B): se inclina en Z desde la base del cuerpo
		color = glm::vec3(0.9f, 0.5f, 0.1f); //naranja
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0432f, -8.0084f, -0.0013f) * escalaSat);
		model = glm::rotate(model, glm::radians(mainWindow.gettorre()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatTorre_M.RenderModel();

		//Instrumento de arriba (tecla Q): gira en Y sobre la tapa del cuerpo
		color = glm::vec3(0.2f, 0.8f, 0.3f); //verde
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0437f, 7.9816f, 0.4687f) * escalaSat);
		model = glm::rotate(model, glm::radians(mainWindow.getinstrumento()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatInstrumento_M.RenderModel();

		//Antena (tecla E): se inclina en X desde su base
		color = glm::vec3(0.9f, 0.1f, 0.1f); //rojo
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0237f, 7.9916f, -3.4113f) * escalaSat);
		model = glm::rotate(model, glm::radians(mainWindow.getantena()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSat, escalaSat, escalaSat));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatAntena_M.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
