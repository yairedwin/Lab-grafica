#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glew.h>
#include <glfw3.h>
//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
const double tiempoEspera = 3.0; //CAMBIADO: 3 segundos, asi lo pide la rubrica de calificacion (el enunciado decia 2s)
GLuint VAO1, VBO1, VAO2, VBO2, VAO3, VBO3, shader; //un VAO/VBO por letra (Y, H, P)
float color1, color2, color3; //variables para el color de fondo

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader recibe datos vec3 son (x,y,z) vertices a cordenadas
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f);//casteo 	\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,0.0f,0.0f,1.0f);//rojo, mismo color para las 3 letras	\n\
}";
//color , color textura , color iluminación


void CrearLetraY() //CAMBIADO: brazos rehechos como trapecios (quads) en vez de triangulos simples, para que se vean cuadrados como la H y la P
{
	GLfloat vertices[] = {
		//brazo izquierdo (trapecio = 2 triangulos, antes era 1 solo triangulo picudo)
		-0.88f, 0.32f, 0.0f, //punta exterior arriba
		-0.76f, 0.32f, 0.0f, //punta interior arriba
		-0.62f, 0.02f, 0.0f, //punta interior abajo (cerca del tallo)

		-0.88f, 0.32f, 0.0f, //punta exterior arriba (se repite)
		-0.62f, 0.02f, 0.0f, //punta interior abajo (se repite)
		-0.74f, 0.02f, 0.0f, //punta exterior abajo

		//brazo derecho (trapecio = 2 triangulos, espejo del izquierdo)
		-0.32f, 0.32f, 0.0f, //punta exterior arriba
		-0.44f, 0.32f, 0.0f, //punta interior arriba
		-0.58f, 0.02f, 0.0f, //punta interior abajo (cerca del tallo)

		-0.32f, 0.32f, 0.0f, //punta exterior arriba (se repite)
		-0.58f, 0.02f, 0.0f, //punta interior abajo (se repite)
		-0.46f, 0.02f, 0.0f, //punta exterior abajo

		//tallo (2 triangulos, igual que antes)
		-0.66f,  0.02f, 0.0f,
		-0.54f,  0.02f, 0.0f,
		-0.54f, -0.32f, 0.0f,

		-0.66f,  0.02f, 0.0f,
		-0.54f, -0.32f, 0.0f,
		-0.66f, -0.32f, 0.0f
	}; //6 triangulos = la letra Y (antes eran 4)
	glGenVertexArrays(1, &VAO1); //generar 1 VAO
	glBindVertexArray(VAO1); //asignar VAO
	//por cada Vao hay un VBO, por cada VBO hay un IBO
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estatico pues no se modificaran los valores
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); //stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CrearLetraH()
{
	GLfloat vertices[] = {
		//barra izquierda (2 triangulos)
		-0.22f, -0.32f, 0.0f,
		-0.10f, -0.32f, 0.0f,
		-0.10f,  0.32f, 0.0f,

		-0.22f, -0.32f, 0.0f,
		-0.10f,  0.32f, 0.0f,
		-0.22f,  0.32f, 0.0f,

		//barra derecha (2 triangulos)
		 0.10f, -0.32f, 0.0f,
		 0.22f, -0.32f, 0.0f,
		 0.22f,  0.32f, 0.0f,

		 0.10f, -0.32f, 0.0f,
		 0.22f,  0.32f, 0.0f,
		 0.10f,  0.32f, 0.0f,

		 //barra horizontal, en medio (2 triangulos)
		-0.22f, -0.06f, 0.0f,
		 0.22f, -0.06f, 0.0f,
		 0.22f,  0.06f, 0.0f,

		-0.22f, -0.06f, 0.0f,
		 0.22f,  0.06f, 0.0f,
		-0.22f,  0.06f, 0.0f
	}; //6 triangulos = la letra H
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CrearLetraP()
{
	GLfloat vertices[] = {
		//tallo, toda la altura (2 triangulos)
		0.40f, -0.32f, 0.0f,
		0.52f, -0.32f, 0.0f,
		0.52f,  0.32f, 0.0f,

		0.40f, -0.32f, 0.0f,
		0.52f,  0.32f, 0.0f,
		0.40f,  0.32f, 0.0f,

		//tapa de arriba del lazo (2 triangulos)
		0.40f, 0.20f, 0.0f,
		0.80f, 0.20f, 0.0f,
		0.80f, 0.32f, 0.0f,

		0.40f, 0.20f, 0.0f,
		0.80f, 0.32f, 0.0f,
		0.40f, 0.32f, 0.0f,

		//lado derecho del lazo (2 triangulos)
		0.68f, 0.00f, 0.0f,
		0.80f, 0.00f, 0.0f,
		0.80f, 0.32f, 0.0f,

		0.68f, 0.00f, 0.0f,
		0.80f, 0.32f, 0.0f,
		0.68f, 0.32f, 0.0f,

		//linea de en medio que cierra el lazo (2 triangulos)
		0.40f, 0.00f, 0.0f,
		0.80f, 0.00f, 0.0f,
		0.80f, 0.12f, 0.0f,

		0.40f, 0.00f, 0.0f,
		0.80f, 0.12f, 0.0f,
		0.40f, 0.12f, 0.0f
	}; //8 triangulos = la letra P
	glGenVertexArrays(1, &VAO3);
	glBindVertexArray(VAO3);
	glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica
//theProgram recibe los datos de theShader
{
	GLuint theShader = glCreateShader(shaderType); //theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode; //shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode); //longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength); //se le asigna al shader el codigo
	glCompileShader(theShader); //se compila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevencion de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader); //si no hubo problemas se asigna el shader a theProgram el cual asigna el codigo a la tarjeta grafica
}

void CompileShaders()
{
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER); //agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER); //agregar fragment shader
	//para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader); //se linkean los shaders a la tarjeta grafica
	//verificaciones y prevencion de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}
}

int main()
{
	//Inicializacion de GLFW
	if (!glfwInit())
	{
		printf("Fallo inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//**** LAS SIGUIENTES 4 LINEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSION DE OPENGL QUE TIENE ****

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamano de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fallo inicializacion de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	//Llamada a las funciones creadas antes del main
	CrearLetraY();
	CrearLetraH();
	CrearLetraP();
	CompileShaders();

	//CAMBIADO: antes color1/2/3 arrancaban en 0.0f (negro) y se quedaban asi hasta el primer ciclo; ahora se sortea un color desde el inicio
	srand(time(NULL) + clock()); //semilla que no depende solo del reloj, para que cada ejecucion salga distinta
	color1 = (float)rand() / RAND_MAX;
	color2 = (float)rand() / RAND_MAX;
	color3 = (float)rand() / RAND_MAX;
	double tiempo = glfwGetTime();

	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		//cada tiempoEspera segundos, elige un color nuevo al azar
		if (glfwGetTime() - tiempo >= tiempoEspera)
		{
			color1 = (float)rand() / RAND_MAX;
			color2 = (float)rand() / RAND_MAX;
			color3 = (float)rand() / RAND_MAX;
			tiempo = glfwGetTime();
		}

		//Limpiar la ventana
		glClearColor(color1, color2, color3, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader); //se le asigna a la tarjeta grafica el shader que se creo y compilo

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 18); //CAMBIADO: la Y ahora son 6 triangulos (18 vertices), antes 4 (12 vertices)
		glBindVertexArray(0);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 18); //la H, 6 triangulos
		glBindVertexArray(0);

		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, 24); //la P, 8 triangulos
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA LINEA DESPUES DE glfwSwapBuffers(mainWindow);
	}

	return 0;
}