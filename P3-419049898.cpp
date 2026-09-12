//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* piramidet = new Mesh();
	piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(piramidet);

}


//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,//frontal
		3,2,4,//izquierda
		2,1,4,//trasera
		1,0,4,//derecha
		0,1,2,//abajo1
		0,2,3//abajo2

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramidec = new Mesh();
	piramidec->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramidec);
}

//función para crear una sola cara triangular de una pirámide cuadrangular
//sirve para pintar cada cara de un color distinto, girándola 90 grados en Y para llegar a las otras 3
void CrearCaraLateral()
{
	unsigned int caralateral_indices[] = {
		0,1,2
	};
	GLfloat caralateral_vertices[] = {
		0.5f,-0.5f,0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* caraLateral = new Mesh();
	caraLateral->CreateMesh(caralateral_vertices, caralateral_indices, 9, 3);
	meshList.push_back(caraLateral);
}

//función para crear la base cuadrada de la pirámide, aparte de las caras laterales
void CrearBaseCuadrada()
{
	unsigned int basecuadrada_indices[] = {
		0,1,2,
		0,2,3
	};
	GLfloat basecuadrada_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
	};
	Mesh* baseCuadrada = new Mesh();
	baseCuadrada->CreateMesh(basecuadrada_vertices, basecuadrada_indices, 12, 6);
	meshList.push_back(baseCuadrada);
}



/*
Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(1280, 960);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(5, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CrearCaraLateral();//índice 5 en MeshList
	CrearBaseCuadrada();//índice 6 en MeshList
	CrearCilindro(20, 1.0f);//índice 7 en MeshList, cilindro liso para el cuerpo del cohete
	CreateShaders();



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);


	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//---------- reporte practica 3, ejercicio 1: cohete espacial ----------
		//usa las 5 primitivas pedidas: cilindros (cuerpo), conos (nariz y tobera), esferas (ventana),
		//piramides (aletas) y cubos (anillo y base del motor)
		glm::vec3 cohetePos = glm::vec3(-3.0f, 0.0f, -6.0f);

		//cuerpo del cohete: cilindro, va de y=-1 a y=1
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.72f, 0.73f, 0.78f); //gris claro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[7]->RenderMeshGeometry();

		//anillo donde pega la nariz con el cuerpo: cubo aplastado
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, 0.92f, 0.0f));
		model = glm::scale(model, glm::vec3(0.58f, 0.12f, 0.58f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.4f); //gris medio
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//nariz del cohete: cono, la base queda justo en la punta del cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, 1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.75f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.15f, 0.1f); //rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		//ventanilla del cohete: esfera, un poco salida del cuerpo para que se note
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.45f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.75f, 0.85f); //celeste
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//aleta derecha: piramide cuadrangular aplastada, pegada al cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.42f, -0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.45f, 0.9f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.15f, 0.1f); //rojo, igual que la nariz
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//aleta izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(-0.42f, -0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.45f, 0.9f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.15f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//aleta de enfrente
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.42f));
		model = glm::scale(model, glm::vec3(0.1f, 0.9f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.15f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//aleta de atras
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, -0.42f));
		model = glm::scale(model, glm::vec3(0.1f, 0.9f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.15f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//base del motor: cubo, tapa la union entre el cuerpo y la tobera
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, -1.05f, 0.0f));
		model = glm::scale(model, glm::vec3(0.45f, 0.22f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.28f, 0.28f, 0.32f); //gris oscuro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//tobera: cono con la punta hacia arriba para que la boca ancha quede abajo
		model = glm::mat4(1.0);
		model = glm::translate(model, cohetePos);
		model = glm::translate(model, glm::vec3(0.0f, -1.38f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.5f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.2f, 0.2f, 0.24f); //casi negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		//---------- reporte practica 3, ejercicio 2: octaedro estrellado (stella octangula) ----------
		//el nucleo es un octaedro (2 piramides cuadrangulares pegadas por su base) y encima
		//lleva una piramide en cada una de sus 8 caras
		//cada cara del octaedro esta inclinada 54.7356 grados respecto a la vertical (arccos(1/raiz3)),
		//4 caras arriba y 4 abajo, repartidas cada 90 grados
		glm::vec3 estrellaPos = glm::vec3(3.0f, 1.0f, -6.0f);
		float ladoOct = 1.3f;
		float altoOct = ladoOct * 0.707f; //1/raiz(2), para que el octaedro salga regular
		float anchoPico = 0.8f;
		float altoPico = 0.7f;
		float distPico = 0.88f; //del centro a la base del pico, ya pegado sobre la cara

		//---- nucleo: octaedro azul ----
		//piramide de arriba
		model = glm::mat4(1.0);
		model = glm::translate(model, estrellaPos);
		model = glm::scale(model, glm::vec3(ladoOct, altoOct, ladoOct));
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f); //azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//piramide de abajo, la misma volteada
		model = glm::mat4(1.0);
		model = glm::translate(model, estrellaPos);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(ladoOct, altoOct, ladoOct));
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f); //azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//---- pico 1: cara de arriba que mira al frente ----
		glm::mat4 pico1 = glm::mat4(1.0);
		pico1 = glm::translate(pico1, estrellaPos);
		pico1 = glm::rotate(pico1, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico1 = glm::rotate(pico1, glm::radians(54.7356f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico1 = glm::translate(pico1, glm::vec3(0.0f, distPico, 0.0f));
		pico1 = glm::scale(pico1, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico1, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico1, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 2: cara de arriba que mira a la derecha ----
		glm::mat4 pico2 = glm::mat4(1.0);
		pico2 = glm::translate(pico2, estrellaPos);
		pico2 = glm::rotate(pico2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico2 = glm::rotate(pico2, glm::radians(54.7356f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico2 = glm::translate(pico2, glm::vec3(0.0f, distPico, 0.0f));
		pico2 = glm::scale(pico2, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico2, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico2, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 3: cara de arriba que mira hacia atras ----
		glm::mat4 pico3 = glm::mat4(1.0);
		pico3 = glm::translate(pico3, estrellaPos);
		pico3 = glm::rotate(pico3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico3 = glm::rotate(pico3, glm::radians(54.7356f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico3 = glm::translate(pico3, glm::vec3(0.0f, distPico, 0.0f));
		pico3 = glm::scale(pico3, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico3, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico3, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 4: cara de arriba que mira a la izquierda ----
		glm::mat4 pico4 = glm::mat4(1.0);
		pico4 = glm::translate(pico4, estrellaPos);
		pico4 = glm::rotate(pico4, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico4 = glm::rotate(pico4, glm::radians(54.7356f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico4 = glm::translate(pico4, glm::vec3(0.0f, distPico, 0.0f));
		pico4 = glm::scale(pico4, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico4, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico4, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico4, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico4, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 5: cara de abajo que mira al frente ----
		glm::mat4 pico5 = glm::mat4(1.0);
		pico5 = glm::translate(pico5, estrellaPos);
		pico5 = glm::rotate(pico5, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico5 = glm::rotate(pico5, glm::radians(125.264f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico5 = glm::translate(pico5, glm::vec3(0.0f, distPico, 0.0f));
		pico5 = glm::scale(pico5, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico5, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico5, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico5, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico5, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 6: cara de abajo que mira a la derecha ----
		glm::mat4 pico6 = glm::mat4(1.0);
		pico6 = glm::translate(pico6, estrellaPos);
		pico6 = glm::rotate(pico6, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico6 = glm::rotate(pico6, glm::radians(125.264f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico6 = glm::translate(pico6, glm::vec3(0.0f, distPico, 0.0f));
		pico6 = glm::scale(pico6, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico6, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico6, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico6, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico6, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 7: cara de abajo que mira hacia atras ----
		glm::mat4 pico7 = glm::mat4(1.0);
		pico7 = glm::translate(pico7, estrellaPos);
		pico7 = glm::rotate(pico7, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico7 = glm::rotate(pico7, glm::radians(125.264f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico7 = glm::translate(pico7, glm::vec3(0.0f, distPico, 0.0f));
		pico7 = glm::scale(pico7, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico7, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico7, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico7, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico7, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		//---- pico 8: cara de abajo que mira a la izquierda ----
		glm::mat4 pico8 = glm::mat4(1.0);
		pico8 = glm::translate(pico8, estrellaPos);
		pico8 = glm::rotate(pico8, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pico8 = glm::rotate(pico8, glm::radians(125.264f), glm::vec3(1.0f, 0.0f, 0.0f));
		pico8 = glm::translate(pico8, glm::vec3(0.0f, distPico, 0.0f));
		pico8 = glm::scale(pico8, glm::vec3(anchoPico, altoPico, anchoPico));

		model = glm::rotate(pico8, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); //roja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico8, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico8, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f); //magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		model = glm::rotate(pico8, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}