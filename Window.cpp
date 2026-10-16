#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	roverX = 0.0f;//cuanto se ha movido el rover con las teclas Y y U y aumentando el valor de muevex es de 0.2 + o -
	avionX = 0.0f;
	avionY = 0.0f;
	colorFaro = 0;
	holocronPrendido = true;
	pinzaPrendida = true;
	naveEnergia = true;
	motor1 = true;
	motor2 = true;
	//linea 115 y 119
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica08:Nombre de la practica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	//el rover avanza con Y y retrocede con U; GLFW repite la tecla mientras se mantiene presionada
	if (key == GLFW_KEY_Y && action != GLFW_RELEASE)
	{
		theWindow->roverX += 0.2f;
	}
	if (key == GLFW_KEY_U && action != GLFW_RELEASE)
	{
		theWindow->roverX -= 0.2f;
	}
	//el avion avanza y retrocede con las flechas derecha e izquierda, sube y baja con arriba y abajo
	if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE)
	{
		theWindow->avionX += 0.2f;
	}
	if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE)
	{
		theWindow->avionX -= 0.2f;
	}
	if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
	{
		theWindow->avionY += 0.1f;
	}
	if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
	{
		theWindow->avionY -= 0.1f;
	}
	//C cambia el color del faro de abajo del avion, H prende o apaga la luz del holocron y P la de la pinza;
	//con GLFW_PRESS cuenta una vez por pulsacion, sin repetirse mientras se mantiene la tecla
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		theWindow->colorFaro = (theWindow->colorFaro + 1) % 6;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		theWindow->holocronPrendido = !theWindow->holocronPrendido;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		theWindow->pinzaPrendida = !theWindow->pinzaPrendida;
	}
	//N prende y apaga al mismo tiempo la luz puntual y la spot de la nave, 1 y 2 prenden y apagan cada motor por separado
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		theWindow->naveEnergia = !theWindow->naveEnergia;
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		theWindow->motor1 = !theWindow->motor1;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		theWindow->motor2 = !theWindow->motor2;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}



	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
