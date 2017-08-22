
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Application.h"

#define GLM_FORCE_RADIANS

#define RESX 1024
#define RESY 768

Application app;

//Se tiene que llamar despues de inicializar la ventana y el OpenGL, pero antes del main loop
void init()
{
	app.init();
}

//Se ejecuta siempre que se necesite actualizar la imagen
void display()
{
	app.display();
}

//Se ejecuta cuando la ventana cambia de tamaño. Parámetros en pixeles.
//(ojo: En otros programas se debe ejecutar glViewport)
void reshape(int w, int h)
{
	app.reshape(w, h);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (window == app.window)
		app.keyboard(key, scancode, action, mods);
}


static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (window == app.window)
		app.cursor_position(xpos, ypos);
}



int main(int argc, char *argv[])
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	app.window = glfwCreateWindow(app.WIDTH, app.HEIGHT, "pixel", NULL, NULL);
	if (!app.window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(app.window);

	glewExperimental = GL_TRUE;
	glewInit();
	app.init();

	// get version info 
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf(" Renderer: %s\n", renderer);
	printf(" OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer 
	//glEnable (GL_DEPTH_TEST); // enable depth-testing 	
	//glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer" 

	glViewport(0, 0, (GLsizei)app.WIDTH, (GLsizei)app.HEIGHT);

	glfwSetCursorPosCallback(app.window, cursor_pos_callback);
	glfwSetKeyCallback(app.window, key_callback);

	while (!glfwWindowShouldClose(app.window))
	{
		/* update other events like input handling */
		glfwPollEvents();

		app.update();
		app.display();
		
		glfwSwapBuffers(app.window);
	}

	/* close GL context and any other GLFW resources */
	glfwTerminate();
	return 0;

}
