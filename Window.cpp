#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	mouseFirstMoved = true;
	for (size_t i = 0; i < 1024; i++) keys[i] = 0;
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	mouseFirstMoved = true;

	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	articulacion6 = 0.0f;
	articulacion7 = 0.0f;
	articulacion8 = 0.0f;
	articulacion9 = 0.0f;

	//todas arrancan subiendo hacia el sentido positivo
	sentido4 = true;
	sentido5 = true;
	sentido6 = true;
	sentido7 = true;
	sentido8 = true;
	sentido9 = true;

	//holocron y satelite arrancan sin giro ni desplazamiento
	esquina1 = 0.0f;
	esquina2 = 0.0f;
	esquina3 = 0.0f;
	esquina4 = 0.0f;
	esquina5 = 0.0f;
	esquina6 = 0.0f;
	esquina7 = 0.0f;
	esquina8 = 0.0f;
	satX = 0.0f;
	satY = 0.0f;
	satZ = 0.0f;
	satGiroX = 0.0f;
	satGiroY = 0.0f;
	satGiroZ = 0.0f;
	panelIzq = 0.0f;
	panelDer = 0.0f;
	torre = 0.0f;
	instrumento = 0.0f;
	antena = 0.0f;

	for (size_t i = 0; i < 1024; i++) keys[i] = 0;
}
int Window::Initialise()
{
	if (!glfwInit())
	{
		printf("Fallo inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Practica 05: Optimizacion y Carga de Modelos", NULL, NULL);
	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}

	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);
	createCallbacks();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Fallo inicializacion de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, bufferWidth, bufferHeight);
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
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

	//uso action != GLFW_RELEASE para que sume tanto al presionar como al mantener detenida la tecla,
	//pero sin sumar otra vez cuando la sueltas

	//llanta delantera derecha, tecla J
	if (key == GLFW_KEY_J && action != GLFW_RELEASE)
	{
		if (theWindow->sentido4) theWindow->articulacion4 += 2.0f;
		else theWindow->articulacion4 -= 2.0f;
		if (theWindow->articulacion4 >= 45.0f) { theWindow->articulacion4 = 45.0f; theWindow->sentido4 = false; }
		if (theWindow->articulacion4 <= -45.0f) { theWindow->articulacion4 = -45.0f; theWindow->sentido4 = true; }
	}
	//llanta delantera izquierda, tecla K
	if (key == GLFW_KEY_K && action != GLFW_RELEASE)
	{
		if (theWindow->sentido5) theWindow->articulacion5 += 2.0f;
		else theWindow->articulacion5 -= 2.0f;
		if (theWindow->articulacion5 >= 45.0f) { theWindow->articulacion5 = 45.0f; theWindow->sentido5 = false; }
		if (theWindow->articulacion5 <= -45.0f) { theWindow->articulacion5 = -45.0f; theWindow->sentido5 = true; }
	}
	//llanta media derecha, tecla L
	if (key == GLFW_KEY_L && action != GLFW_RELEASE)
	{
		if (theWindow->sentido6) theWindow->articulacion6 += 2.0f;
		else theWindow->articulacion6 -= 2.0f;
		if (theWindow->articulacion6 >= 45.0f) { theWindow->articulacion6 = 45.0f; theWindow->sentido6 = false; }
		if (theWindow->articulacion6 <= -45.0f) { theWindow->articulacion6 = -45.0f; theWindow->sentido6 = true; }
	}
	//llanta media izquierda, tecla I
	if (key == GLFW_KEY_I && action != GLFW_RELEASE)
	{
		if (theWindow->sentido7) theWindow->articulacion7 += 2.0f;
		else theWindow->articulacion7 -= 2.0f;
		if (theWindow->articulacion7 >= 45.0f) { theWindow->articulacion7 = 45.0f; theWindow->sentido7 = false; }
		if (theWindow->articulacion7 <= -45.0f) { theWindow->articulacion7 = -45.0f; theWindow->sentido7 = true; }
	}
	//llanta trasera derecha, tecla M
	if (key == GLFW_KEY_M && action != GLFW_RELEASE)
	{
		if (theWindow->sentido8) theWindow->articulacion8 += 2.0f;
		else theWindow->articulacion8 -= 2.0f;
		if (theWindow->articulacion8 >= 45.0f) { theWindow->articulacion8 = 45.0f; theWindow->sentido8 = false; }
		if (theWindow->articulacion8 <= -45.0f) { theWindow->articulacion8 = -45.0f; theWindow->sentido8 = true; }
	}
	//llanta trasera izquierda, tecla N
	if (key == GLFW_KEY_N && action != GLFW_RELEASE)
	{
		if (theWindow->sentido9) theWindow->articulacion9 += 2.0f;
		else theWindow->articulacion9 -= 2.0f;
		if (theWindow->articulacion9 >= 45.0f) { theWindow->articulacion9 = 45.0f; theWindow->sentido9 = false; }
		if (theWindow->articulacion9 <= -45.0f) { theWindow->articulacion9 = -45.0f; theWindow->sentido9 = true; }
	}

	//para lo nuevo de la practica: la tecla sola gira/mueve en un sentido y con Shift en el contrario
	GLfloat signo = 1.0f;
	if (mode & GLFW_MOD_SHIFT) signo = -1.0f;

	//Holocron: cada esquina gira alrededor del centro, teclas 1 a 8
	if (key == GLFW_KEY_1 && action != GLFW_RELEASE) theWindow->esquina1 += 5.0f * signo;
	if (key == GLFW_KEY_2 && action != GLFW_RELEASE) theWindow->esquina2 += 5.0f * signo;
	if (key == GLFW_KEY_3 && action != GLFW_RELEASE) theWindow->esquina3 += 5.0f * signo;
	if (key == GLFW_KEY_4 && action != GLFW_RELEASE) theWindow->esquina4 += 5.0f * signo;
	if (key == GLFW_KEY_5 && action != GLFW_RELEASE) theWindow->esquina5 += 5.0f * signo;
	if (key == GLFW_KEY_6 && action != GLFW_RELEASE) theWindow->esquina6 += 5.0f * signo;
	if (key == GLFW_KEY_7 && action != GLFW_RELEASE) theWindow->esquina7 += 5.0f * signo;
	if (key == GLFW_KEY_8 && action != GLFW_RELEASE) theWindow->esquina8 += 5.0f * signo;

	//Satelite: mover en X con las flechas izquierda/derecha, en Y con arriba/abajo, en Z con O/P
	if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) theWindow->satX += 0.5f;
	if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) theWindow->satX -= 0.5f;
	if (key == GLFW_KEY_UP && action != GLFW_RELEASE) theWindow->satY += 0.5f;
	if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) theWindow->satY -= 0.5f;
	if (key == GLFW_KEY_P && action != GLFW_RELEASE) theWindow->satZ += 0.5f;
	if (key == GLFW_KEY_O && action != GLFW_RELEASE) theWindow->satZ -= 0.5f;

	//Satelite: girar todo el satelite sobre su centro, tecla X en el eje X, Y en el eje Y, Z en el eje Z
	if (key == GLFW_KEY_X && action != GLFW_RELEASE) theWindow->satGiroX += 5.0f * signo;
	if (key == GLFW_KEY_Y && action != GLFW_RELEASE) theWindow->satGiroY += 5.0f * signo;
	if (key == GLFW_KEY_Z && action != GLFW_RELEASE) theWindow->satGiroZ += 5.0f * signo;

	//Satelite: paneles solares (C izquierdo, V derecho), giran completos sobre su eje largo para apuntar al sol
	if (key == GLFW_KEY_C && action != GLFW_RELEASE) theWindow->panelIzq += 5.0f * signo;
	if (key == GLFW_KEY_V && action != GLFW_RELEASE) theWindow->panelDer += 5.0f * signo;

	//Satelite: instrumento de arriba (Q), gira sobre si mismo para apuntar a otro lado
	if (key == GLFW_KEY_Q && action != GLFW_RELEASE) theWindow->instrumento += 5.0f * signo;

	//Satelite: torre de abajo (B), se inclina desde la base pero sin meterse al cuerpo, de -60 a 60
	if (key == GLFW_KEY_B && action != GLFW_RELEASE)
	{
		theWindow->torre += 3.0f * signo;
		if (theWindow->torre > 60.0f) theWindow->torre = 60.0f;
		if (theWindow->torre < -60.0f) theWindow->torre = -60.0f;
	}

	//Satelite: antena (E), se inclina desde su base de -45 a 45
	if (key == GLFW_KEY_E && action != GLFW_RELEASE)
	{
		theWindow->antena += 3.0f * signo;
		if (theWindow->antena > 45.0f) theWindow->antena = 45.0f;
		if (theWindow->antena < -45.0f) theWindow->antena = -45.0f;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) theWindow->keys[key] = true;
		else if (action == GLFW_RELEASE) theWindow->keys[key] = false;
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
