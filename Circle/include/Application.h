#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <iostream>
#include "Object3D.h"
#include "Plane.h"
#include <stack>
#include "GLFW\glfw3.h"


class Application {
public:

	static const int WIDTH = 1024;
	static const int HEIGHT = 1024;
	static const int RGB = 3;
	static const int RGBA = 4;

	Application();
	~Application();

	void init();
	void display();
	void reshape(int w, int h);
	void keyboard(int key, int scancode, int action, int mods);
	void update();
	void swapbuffers();
	void cursor_position(double xpos, double ypos);
	void putPixel(GLubyte *buffer, int x, int y, char R, char G, char B, char A);
	void putPixel(int x, int y, char R, char G, char B, char A);
	void draw();
	void setColor(int r, int g, int b, int a);
	void moveTo(int X, int Y);
	void lineTo(int X1, int Y1);
	void Circle(int Cx, int Cy, int R);
	void PutPixel(int x, int y);
	void Line(int X0, int Y0, int X1, int Y1);

	std::stack<glm::mat4> mStack;
	glm::mat4 mProjectionMatrix, mTransform;
	glm::vec3 vEye;
	GLFWwindow* window;
	unsigned char Buffers[2][WIDTH*HEIGHT*RGBA];

private:	
	GLuint texturesID[2], pboID[2];
	GLuint shaderID;
	GLuint VAO, VBO;
	GLuint sampler;
	GLuint uTransform;
	GLubyte* _screenBuffer;
	int _currentBuffer, 
		_nextBuffer;

	glm::vec3 myLightPosition;
	GLuint uMyLightPosition[2];	

	Plane oPlane;
	float fTime;
	void initTextures();
	void processPBO();
	void initPBOs();
	void updatePixels(GLubyte *buffer);
	int _drawMode,
		shader;
	bool moveLight;

	int R;
	int G;
	int B;
	int A;

	int XF;
	int YF;

	void fakeBuffers();
};

#endif //__APPLICATION_H__