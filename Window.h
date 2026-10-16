#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getroverX() { return roverX; } //cuanto se ha movido el rover con las teclas Y y U
	GLfloat getavionX() { return avionX; } //cuanto se ha movido el avion hacia adelante o atras con las flechas derecha e izquierda
	int getcolorFaro() { return colorFaro; } //0 rojo, 1 amarillo, 2 verde, 3 cian, 4 azul, 5 magenta (tecla C)
	bool getholocronPrendido() { return holocronPrendido; } //tecla H
	bool getpinzaPrendida() { return pinzaPrendida; } //tecla P
	bool getnaveEnergia() { return naveEnergia; } //tecla N: luz puntual y spot de la nave
	bool getmotor1() { return motor1; } //tecla 1: motor derecho (flama y su luz)
	bool getmotor2() { return motor2; } //tecla 2: motor izquierdo (flama y su luz)
	GLfloat getavionY() { return avionY; } //cuanto ha subido o bajado el avion con las flechas arriba y abajo
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat roverX;
	GLfloat avionX;
	GLfloat avionY;
	int colorFaro;
	bool holocronPrendido;
	bool pinzaPrendida;
	bool naveEnergia;
	bool motor1;
	bool motor2;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

