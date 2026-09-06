//Práctica 2 - letras con color por vértice + torre/cuadrado/trifuerza
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"

const float toRadians = 3.14159265f / 180.0;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;

//Shaders de color sólido para la escena (uno por color, todos comparten el mismo shader.frag)
static const char* fShader = "shaders/shader.frag";
static const char* vShaderRojo = "shaders/shaderrojo.vert";       //el que dio el profe de ejemplo
static const char* vShaderVerde = "shaders/shaderverde.vert";
static const char* vShaderAzul = "shaders/shaderazul.vert";
static const char* vShaderCafe = "shaders/shadercafe.vert";
static const char* vShaderMagenta = "shaders/shadermagenta.vert";
static const char* vShaderAmarillo = "shaders/shaderamarillo.vert";

//Shader de color por vértice, para las letras (el mismo de siempre)
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

//Pirámide triangular regular (para la torre) 
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};
	GLfloat vertices[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3
	};
	Mesh* piramidetriangular = new Mesh();
	piramidetriangular->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(piramidetriangular);
}

//Pirámide de base cuadrada (para el cuadrado anidado y la trifuerza) 
//del archivo "Main con pirámide cuadrangular" que subió el profe, sin modificarla
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4
	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

//Cubo — sin cambios respecto al que ya tenías
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		4, 5, 1, 1, 0, 4,
		3, 2, 6, 6, 7, 3
	};
	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

//Punto 1: iniciales Y, H, P con degradado de color por vértice (mismos vértices que
//P1-419049898.cpp, con una columna de color agregada a cada vértice)
void CrearLetras()
{
	GLfloat vertices_Y[] = {
		//X		Y		Z		R		G		B
		-0.88f,	0.32f,	0.0f,	1.00f,	0.92f,	0.25f,
		-0.76f,	0.32f,	0.0f,	1.00f,	0.92f,	0.25f,
		-0.62f,	0.02f,	0.0f,	0.93f,	0.70f,	0.14f,

		-0.88f,	0.32f,	0.0f,	1.00f,	0.92f,	0.25f,
		-0.62f,	0.02f,	0.0f,	0.93f,	0.70f,	0.14f,
		-0.74f,	0.02f,	0.0f,	0.93f,	0.70f,	0.14f,

		-0.32f,	0.32f,	0.0f,	1.00f,	0.92f,	0.25f,
		-0.44f,	0.32f,	0.0f,	1.00f,	0.92f,	0.25f,
		-0.58f,	0.02f,	0.0f,	0.93f,	0.70f,	0.14f,

		-0.32f,	0.32f,	0.0f,	1.00f,	0.92f,	0.25f,
		-0.58f,	0.02f,	0.0f,	0.93f,	0.70f,	0.14f,
		-0.46f,	0.02f,	0.0f,	0.93f,	0.70f,	0.14f,

		-0.66f,	 0.02f,	0.0f,	0.93f,	0.70f,	0.14f,
		-0.54f,	 0.02f,	0.0f,	0.93f,	0.70f,	0.14f,
		-0.54f,	-0.32f,	0.0f,	0.85f,	0.45f,	0.02f,

		-0.66f,	 0.02f,	0.0f,	0.93f,	0.70f,	0.14f,
		-0.54f,	-0.32f,	0.0f,	0.85f,	0.45f,	0.02f,
		-0.66f,	-0.32f,	0.0f,	0.85f,	0.45f,	0.02f,
	};
	MeshColor* letraY = new MeshColor();
	letraY->CreateMeshColor(vertices_Y, 108);
	meshColorList.push_back(letraY);

	GLfloat vertices_H[] = {
		//X		Y		Z		R		G		B
		-0.22f,	-0.32f,	0.0f,	0.55f,	0.02f,	0.35f,
		-0.10f,	-0.32f,	0.0f,	0.55f,	0.02f,	0.35f,
		-0.10f,	 0.32f,	0.0f,	1.00f,	0.35f,	0.75f,

		-0.22f,	-0.32f,	0.0f,	0.55f,	0.02f,	0.35f,
		-0.10f,	 0.32f,	0.0f,	1.00f,	0.35f,	0.75f,
		-0.22f,	 0.32f,	0.0f,	1.00f,	0.35f,	0.75f,

		 0.10f,	-0.32f,	0.0f,	0.55f,	0.02f,	0.35f,
		 0.22f,	-0.32f,	0.0f,	0.55f,	0.02f,	0.35f,
		 0.22f,	 0.32f,	0.0f,	1.00f,	0.35f,	0.75f,

		 0.10f,	-0.32f,	0.0f,	0.55f,	0.02f,	0.35f,
		 0.22f,	 0.32f,	0.0f,	1.00f,	0.35f,	0.75f,
		 0.10f,	 0.32f,	0.0f,	1.00f,	0.35f,	0.75f,

		-0.22f,	-0.06f,	0.0f,	0.73f,	0.15f,	0.51f,
		 0.22f,	-0.06f,	0.0f,	0.73f,	0.15f,	0.51f,
		 0.22f,	 0.06f,	0.0f,	0.82f,	0.22f,	0.59f,

		-0.22f,	-0.06f,	0.0f,	0.73f,	0.15f,	0.51f,
		 0.22f,	 0.06f,	0.0f,	0.82f,	0.22f,	0.59f,
		-0.22f,	 0.06f,	0.0f,	0.82f,	0.22f,	0.59f,
	};
	MeshColor* letraH = new MeshColor();
	letraH->CreateMeshColor(vertices_H, 108);
	meshColorList.push_back(letraH);

	GLfloat vertices_P[] = {
		//X		Y		Z		R		G		B
		0.40f,	-0.32f,	0.0f,	0.02f,	0.25f,	0.55f,
		0.52f,	-0.32f,	0.0f,	0.02f,	0.25f,	0.55f,
		0.52f,	 0.32f,	0.0f,	0.35f,	0.85f,	1.00f,

		0.40f,	-0.32f,	0.0f,	0.02f,	0.25f,	0.55f,
		0.52f,	 0.32f,	0.0f,	0.35f,	0.85f,	1.00f,
		0.40f,	 0.32f,	0.0f,	0.35f,	0.85f,	1.00f,

		0.40f,	0.20f,	0.0f,	0.29f,	0.74f,	0.92f,
		0.80f,	0.20f,	0.0f,	0.29f,	0.74f,	0.92f,
		0.80f,	0.32f,	0.0f,	0.35f,	0.85f,	1.00f,

		0.40f,	0.20f,	0.0f,	0.29f,	0.74f,	0.92f,
		0.80f,	0.32f,	0.0f,	0.35f,	0.85f,	1.00f,
		0.40f,	0.32f,	0.0f,	0.35f,	0.85f,	1.00f,

		0.68f,	0.00f,	0.0f,	0.19f,	0.55f,	0.78f,
		0.80f,	0.00f,	0.0f,	0.19f,	0.55f,	0.78f,
		0.80f,	0.32f,	0.0f,	0.35f,	0.85f,	1.00f,

		0.68f,	0.00f,	0.0f,	0.19f,	0.55f,	0.78f,
		0.80f,	0.32f,	0.0f,	0.35f,	0.85f,	1.00f,
		0.68f,	0.32f,	0.0f,	0.35f,	0.85f,	1.00f,

		0.40f,	0.00f,	0.0f,	0.19f,	0.55f,	0.78f,
		0.80f,	0.00f,	0.0f,	0.19f,	0.55f,	0.78f,
		0.80f,	0.12f,	0.0f,	0.25f,	0.66f,	0.86f,

		0.40f,	0.00f,	0.0f,	0.19f,	0.55f,	0.78f,
		0.80f,	0.12f,	0.0f,	0.25f,	0.66f,	0.86f,
		0.40f,	0.12f,	0.0f,	0.25f,	0.66f,	0.86f,
	};
	MeshColor* letraP = new MeshColor();
	letraP->CreateMeshColor(vertices_P, 144);
	meshColorList.push_back(letraP);
}

void CreateShaders()
{
	Shader* s0 = new Shader(); s0->CreateFromFiles(vShaderRojo, fShader); shaderList.push_back(*s0);
	Shader* s1 = new Shader(); s1->CreateFromFiles(vShaderVerde, fShader); shaderList.push_back(*s1);
	Shader* s2 = new Shader(); s2->CreateFromFiles(vShaderAzul, fShader); shaderList.push_back(*s2);
	Shader* s3 = new Shader(); s3->CreateFromFiles(vShaderCafe, fShader); shaderList.push_back(*s3);
	Shader* s4 = new Shader(); s4->CreateFromFiles(vShaderMagenta, fShader); shaderList.push_back(*s4);
	Shader* s5 = new Shader(); s5->CreateFromFiles(vShaderColor, fShaderColor); shaderList.push_back(*s5); //letras
	Shader* s6 = new Shader(); s6->CreateFromFiles(vShaderAmarillo, fShader); shaderList.push_back(*s6); //amarillo
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	CreaPiramide();               //meshList[0]: pirámide triangular (torre)
	CrearCubo();                  //meshList[1]: cubo (postes y rombos)
	CrearPiramideCuadrangular();  //meshList[2]: pirámide cuadrangular (cuadrado y trifuerza)
	CrearLetras();                //meshColorList[0]=Y, [1]=H, [2]=P
	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	//Perspectiva (obligatoria para esta parte, según la clase): FOV 45°, aspecto de la ventana
	glm::mat4 projection = glm::perspective(45.0f * toRadians, 800.0f / 600.0f, 0.1f, 100.0f);
	glm::mat4 model(1.0);

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();
		glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//---- Punto 1: letras Y, H, P (se quedan conectadas aquí, aunque el release que se
		//sube es el que muestra las figuras) ----
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, -3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor(); //Y

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, -3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[1]->RenderMeshColor(); //H

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, -3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[2]->RenderMeshColor(); //P

		//---- Punto 2: torre, cuadrado anidado y trifuerza con pirámides y cubos ----

		//-- Torre: postes de cubo café + 3 pirámides triangulares apuntando hacia abajo --
		shaderList[3].useShader(); //café
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.85f, -0.3f, -2.5f));
		model = glm::scale(model, glm::vec3(0.1f, 1.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); //poste izquierdo

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.45f, -0.3f, -2.5f));
		model = glm::scale(model, glm::vec3(0.1f, 1.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); //poste derecho

		shaderList[0].useShader(); //rojo
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.65f, 0.1f, -2.5f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.2f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); //pirámide de arriba

		shaderList[1].useShader(); //verde
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.65f, -0.3f, -2.5f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.2f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); //pirámide de en medio

		shaderList[4].useShader(); //magenta
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.65f, -0.7f, -2.5f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.2f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); //pirámide de abajo

		//-- Cuadrado anidado: 4 pirámides cuadrangulares apuntando al centro + 2 cubos como rombos --
		//Z escalonado en las 4 (una un poco más cerca de la cámara que la anterior): sin esto,
		//al estar todas al mismo Z, la que se dibuja primero le gana el traslape a las demás
		shaderList[6].useShader(); //amarillo (ya nadie más lo usa: sin traslape de color con nada)
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.50f, -2.5f));
		model = glm::scale(model, glm::vec3(0.6f, 0.3f, 0.3f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //arriba

		shaderList[0].useShader(); //rojo
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.15f, -0.65f, -2.49f));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //derecha

		shaderList[1].useShader(); //verde
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.80f, -2.48f));
		model = glm::scale(model, glm::vec3(0.6f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //abajo

		shaderList[4].useShader(); //magenta
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.15f, -0.65f, -2.47f));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //izquierda

		shaderList[2].useShader(); //azul (rombo grande)
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.65f, -2.4f));
		model = glm::scale(model, glm::vec3(0.32f, 0.32f, 0.05f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		shaderList[3].useShader(); //café (rombo chico)
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.65f, -2.3f));
		model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.05f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//-- Trifuerza: 4 pirámides cuadrangulares (subdivisión por puntos medios) --
		//Mismo escalonado de Z que el cuadrado, por la misma razón
		shaderList[4].useShader(); //magenta
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.88f, -0.50f, -2.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //arriba

		shaderList[1].useShader(); //verde
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.73f, -0.80f, -2.49f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //abajo-izquierda

		shaderList[0].useShader(); //rojo
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.03f, -0.80f, -2.48f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh(); //abajo-derecha

		shaderList[2].useShader(); //azul (centro invertido)
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.88f, -0.80f, -2.47f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}