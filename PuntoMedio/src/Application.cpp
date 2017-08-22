//

//  HelloCube

//

//  Created by Pedro  Cervantes Pintor on 11/Oct/14.

//
#define GLM_FORCE_RADIANS
#include "Application.h"
#include <iostream>
#include <vector>

#include "GL\glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GLFW\glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderFuncs.h"
#include "Plane.h"

#define PI  3.1516


Application::Application() : vEye(0.0f, 50.0f, 100.0f),
_drawMode(0),
shader(0),
_currentBuffer(0),
_nextBuffer(1)

{
}

Application::~Application()
{
	glDeleteTextures(2, texturesID);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, pboID);
}

void Application::initTextures()
{
	int img_width, img_height;
	int channels;

	glGenTextures(2, texturesID);
	//Textura 0
	glBindTexture(GL_TEXTURE_2D, texturesID[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffers[0]);
	//glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img_width, img_height, GL_RGB, GL_UNSIGNED_BYTE, img);

	//Sin promediar
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Textura 1
	glBindTexture(GL_TEXTURE_2D, texturesID[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffers[1]);
	//glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img_width, img_height, GL_RGB, GL_UNSIGNED_BYTE, img);

	//Sin promediar
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);


}

void Application::initPBOs()
{
	// create 2 pixel buffer objects, you need to delete them when program exits.
	// glBufferData with NULL pointer reserves only memory space.
	glGenBuffers(2, pboID);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, WIDTH * HEIGHT * RGBA, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, WIDTH * HEIGHT * RGBA, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Application::init()
{
	fakeBuffers();
	initTextures();
	initPBOs();
	GLfloat vertices[]
	{
		//triangulo 0


		-1.0f, -1.0f, 0.0f, 1.0f,

		1.0f, -1.0f, 0.0f, 1.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,

		//triangulo2
		//-1.0f, 1.0f, 0.0f, 1.0f,

		//1.0f, -1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 0.0f, 1.0f,
	};
	GLfloat texCoords[]
	{
		0.0f, 1.0f, 0.0f, 0.0f,  //TEX
		1.0f, 1.0f, 0.0f, 0.0f,//TEX
		0.0f, 0.0f, 0.0f, 0.0f, //TEX

		1.0f, 0.0f, 0.0f, 0.0f//TEX

							  //0.0f, 1.0f, 0.0f, 0.0f,//TEX
							  //1.0f, 0.0f, 0.0f, 0.0f,//TEX
	};

	vEye = glm::vec3(0.0f, 50.0f, 50.0f);

	std::string sVertex, sFragment;
	sVertex = loadTextFile("shaders/passThru.v");
	sFragment = loadTextFile("shaders/passThru.f");
	InitializeProgram(shaderID, sVertex, sFragment);

	uTransform = glGetUniformLocation(shaderID, "mTransform");
	sampler = glGetUniformLocation(shaderID, "theTexture");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(vertices)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}


void Application::display()
{
	processPBO();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shaderID);

	mProjectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
	mTransform = glm::mat4(1.0f) * mProjectionMatrix;

	glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mTransform));

	//pasamos parametros de textura
	glBindTexture(GL_TEXTURE_2D, texturesID[_currentBuffer]);
	glUniform1i(sampler, _currentBuffer);
	glActiveTexture(_currentBuffer == 0 ? GL_TEXTURE0 : GL_TEXTURE1);
	//glActiveTexture(GL_TEXTURE0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	swapbuffers();
}

void Application::processPBO()
{
	// bind the texture and PBO
	glBindTexture(GL_TEXTURE_2D, texturesID[_currentBuffer]); //solo se necesita una textura
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[_currentBuffer]);

	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, 0);


	// bind PBO to update texture source
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[_nextBuffer]);

	// Note that glMapBufferARB() causes sync issue.
	// If GPU is working with this buffer, glMapBufferARB() will wait(stall)
	// until GPU to finish its job. To avoid waiting (idle), you can call
	// first glBufferDataARB() with NULL pointer before glMapBufferARB().
	// If you do that, the previous data in PBO will be discarded and
	// glMapBufferARB() returns a new allocated pointer immediately
	// even if GPU is still working with the previous data.
	glBufferData(GL_PIXEL_UNPACK_BUFFER, WIDTH * HEIGHT * RGBA, 0, GL_STREAM_DRAW);

	// map the buffer object into client's memory

	_screenBuffer = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
	if (_screenBuffer)
	{
		// update data directly on the mapped buffer
		//updatePixels(ptr);
		draw();
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release the mapped buffer
	}

	// it is good idea to release PBOs with ID 0 after use.
	// Once bound with 0, all pixel operations are back to normal ways.
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void Application::reshape(int w, int h)
{
	//glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//mProjectionMatrix = glm::perspective(45.0f, (float)w / (float)h, 1.0f, 10000.f);
}


void Application::keyboard(int key, int scancode, int action, int mods)
{
	const float inc = 1.0;
	switch (action)
	{
	case GLFW_PRESS:
		switch (key)
		{

		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
		case GLFW_KEY_T: _currentBuffer = 1 - _currentBuffer; break;
		case GLFW_KEY_P:
			_drawMode = ++_drawMode % 3;
			switch (_drawMode)
			{
			case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
			case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
			case 2:	glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_LINE);
				break;
			}
		}
		//break;

	} //switch (action)
}


void Application::update()
{
}


void Application::cursor_position(double xpos, double ypos)
{
	int width, heigth;

	glfwGetWindowSize(window, &width, &heigth);
}


void Application::fakeBuffers()
{
	for (int i = 0; i < WIDTH*HEIGHT*RGBA; i += 4)
	{
		Buffers[1][i] = 255;
		Buffers[1][i + 1] = 0;
		Buffers[1][i + 2] = 0;
		Buffers[1][i + 3] = 255;

		Buffers[0][i] = 0;
		Buffers[0][i + 1] = 255;
		Buffers[0][i + 2] = 0;
		Buffers[0][i + 3] = 255;
	}
}

void Application::swapbuffers()
{
	_currentBuffer = 1 - _currentBuffer;
	_nextBuffer = 1 - _currentBuffer;
}


void Application::putPixel(GLubyte *buffer, int x, int y, char R, char G, char B, char A)
{
	int offset = (x + (y * WIDTH)) * RGBA;
	if (offset < 0 || offset > WIDTH * HEIGHT *RGBA)
		return;

	buffer[offset] = R;
	buffer[offset + 1] = G;
	buffer[offset + 2] = B;
	buffer[offset + 3] = A;

}

void Application::putPixel(int x, int y, char R, char G, char B, char A)
{
	putPixel(_screenBuffer, x, y, R, G, B, A);
}

int CreateHash(int dx, int dy)
{
	int ret = 0;
	dx >= 0 ? ++ret : --ret;
	dy >= 0 ? ret += 2 : ret -= 2;
	ret = abs(dx) >= abs(dy) ? ret << 1 : ret << 2;
	return ret;
}


void Application::setColor(int r, int g, int b, int a)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

void Application::moveTo(int X, int Y)
{
	XF = X;
	YF = Y;
}

void Application::lineTo(int X1, int Y1)
{
	Line(XF, YF, X1, Y1);
}

void Application::Line(int X0, int Y0, int X1, int Y1)
{
	//Codigo Linea Marcos

	//int dy = Y1 - Y0;
	//int dx = X1 - X0;
	//int E = 2 * dy;
	//int NE = 2 * (dy - dx);
	//int d = 2 * dy - dx;
	//putPixel(X0, Y0, R, G, B, A);
	//int x = X0;
	//int y = Y0;

	//while (x <= X1)
	//{
	//	if (d > 0)
	//	{
	//		++y;
	//		d += NE;
	//	}
	//	else
	//		d += E;

	//	putPixel(x, y, R, G, B, A);
	//	putPixel(y, x, R, G, B, A);
	//	putPixel(-x, -y, R, G, B, A);
	//	putPixel(-y, -x, R, G, B, A);
	//	++x;
	//}

	// CODIGO MALAGON //

	int dy = Y1 - Y0,
		dx = X1 - X0,
		x = X0 + 1,//256 
		y = Y0,
		caseHash = CreateHash(dx, dy);
	dx = abs(dx);
	dy = abs(dy);
	switch (caseHash)
	{
	case 6:
	{// dx > dy 
		int E = 2 * (dy),
			nE = 2 * (dy - dx),
			d = 2 * dy - dx;//0
		while (x != X1) {
			(d > 0) ? y++, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			++x;
		}
		break;
	}
	case -2: // dx > -dy
	{
		int E = 2 * (dy),
			nE = 2 * (dy - dx),
			d = 2 * dy - dx;//0
		while (x != X1) {
			(d > 0) ? y--, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			++x;
		}
		break;
	}

	case -4: // -dy > dx
	{
		int E = 2 * (dx),
			nE = 2 * (dx - dy),
			d = 2 * dx - dy;//0
		while (y != Y1) {
			(d > 0) ? x++, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			--y;
		}
		break;
	}
	case -12: //-dy > -dx
	{
		int E = 2 * (dx),
			nE = 2 * (dx - dy),
			d = 2 * dx - dy;//0
		while (y != Y1) {
			(d > 0) ? x--, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			--y;
		}
		break;
	}

	case -6: //-dx > -dy
	{
		int E = 2 * (dy),
			nE = 2 * (dy - dx),
			d = 2 * dy - dx;//0
		while (x != X1) {
			(d > 0) ? y--, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			--x;
		}
		break;
	}
	case 2: //-dx> dy
	{
		int E = 2 * (dy),
			nE = 2 * (dy - dx),
			d = 2 * dy - dx;//0
		while (x != X1) {
			(d > 0) ? y++, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			--x;
		}
		break;
	}
	case 4: //dy > -dx
	{
		int E = 2 * (dx),
			nE = 2 * (dx - dy),
			d = 2 * dx - dy;//0
		while (y != Y1) {
			(d > 0) ? --x, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			++y;
		}
		break;
	}
	case 12: //dy > dx
	{
		int E = 2 * (dx),
			nE = 2 * (dx - dy),
			d = 2 * dx - dy;//0
		while (y != Y1) {
			(d > 0) ? ++x, d += nE : d += E;
			putPixel(x, y, R, G, B, A);
			++y;
		}
		break;
	}
	}
}

//////////////////////////////////////////////////////////////////////
void Application::draw()
{

	//for (int i  = 0; i < HEIGHT; ++i)
	/*for (int j = 0; j < WIDTH; ++j)
	{
		putPixel(j, 10, 255, 255, 0, 255);
	}
*/

// CODIGO DE LA LINEA MARCOS //

int x = 0, y = 0, X0 = 250, Y0 = 250;
int angulo;

for (angulo = 0; angulo < 360; ++angulo)
{
	x = (100 * (float)cos(angulo * PI / 180)) + XF;
	y = (100 * (float)sin(angulo * PI / 180)) + YF;
	setColor(255, 0, 0, 0);
	moveTo(500, 500);
	/*if (angulo <= 45 && angulo >= 0)
		Line(XF, YF, x, y);
	else
		Line(x, y, XF, YF);*/
}
lineTo(600, 500);



// CODIGO LINEA MALAGON //

	/*int x0 = 255,
		y0 = 255,
		x1 = 200,
		y1 = 270;
	int numRand = rand();
	moveTo(500, 500);
	setColor(0, 255, 0, 0);
	lineTo(600, 500);
	for (int i = 360; i--;)
	{
		setColor(255, 0, 0, 0);
		Line(XF, YF, (x1 * cos(i*(PI / 180))) + XF, (x1 * sin(i*(PI / 180))) + YF);
	}

	moveTo(0, 0);
	setColor(0, 255, 255, 0);*/
}