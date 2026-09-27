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
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
	bool* getsKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	//un getter por llanta, mismo esquema de nombres que practica4 (J,K,L,I,M,N)
	GLfloat getarticulacion4() { return articulacion4; } //llanta delantera derecha
	GLfloat getarticulacion5() { return articulacion5; } //llanta delantera izquierda
	GLfloat getarticulacion6() { return articulacion6; } //llanta media derecha
	GLfloat getarticulacion7() { return articulacion7; } //llanta media izquierda
	GLfloat getarticulacion8() { return articulacion8; } //llanta trasera derecha
	GLfloat getarticulacion9() { return articulacion9; } //llanta trasera izquierda

	//Holocron: giro de cada esquina alrededor del centro (teclas 1 a 8)
	GLfloat getesquina1() { return esquina1; }
	GLfloat getesquina2() { return esquina2; }
	GLfloat getesquina3() { return esquina3; }
	GLfloat getesquina4() { return esquina4; }
	GLfloat getesquina5() { return esquina5; }
	GLfloat getesquina6() { return esquina6; }
	GLfloat getesquina7() { return esquina7; }
	GLfloat getesquina8() { return esquina8; }

	//Satelite: movimiento (flechas y O/P) y giro completo (X, Y, Z)
	GLfloat getsatX() { return satX; }
	GLfloat getsatY() { return satY; }
	GLfloat getsatZ() { return satZ; }
	GLfloat getsatGiroX() { return satGiroX; }
	GLfloat getsatGiroY() { return satGiroY; }
	GLfloat getsatGiroZ() { return satGiroZ; }

	//Satelite: partes que giran en su union con el cuerpo (C, V, B, Q, E)
	GLfloat getpanelIzq() { return panelIzq; }
	GLfloat getpanelDer() { return panelDer; }
	GLfloat gettorre() { return torre; }
	GLfloat getinstrumento() { return instrumento; }
	GLfloat getantena() { return antena; }

	~Window();
private:
	GLFWwindow *mainWindow;
	GLint width, height;
	GLfloat articulacion4, articulacion5, articulacion6, articulacion7, articulacion8, articulacion9;
	bool sentido4, sentido5, sentido6, sentido7, sentido8, sentido9; //true=va subiendo, false=va bajando, para rebotar entre -45 y 45
	GLfloat esquina1, esquina2, esquina3, esquina4, esquina5, esquina6, esquina7, esquina8;
	GLfloat satX, satY, satZ, satGiroX, satGiroY, satGiroZ;
	GLfloat panelIzq, panelDer, torre, instrumento, antena;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;
	void createCallbacks();
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};
