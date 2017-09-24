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

std::vector<Vec3> v4Final;

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
	Start();
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

void Application::Circle(int Cx, int Cy, int R)
{
	setColor(rand() % 255, rand() % 255, rand() % 255, 0);


	int x = 0, y = R, dE = (2*x) + 3, dsE = 2*(x - y) + 5;
	int d = 1 - R;
	int dx = 1, dy = 1;
	int z = dx - (R << 1);

	while (x <= y)
	{
		PutPixel(Cx + x, y + Cy);
		PutPixel(Cx + y, -x + Cy);
		PutPixel(Cx - x, -y + Cy);
		PutPixel(Cx - y, x + Cy);
		PutPixel(Cx + y, x + Cy);
		PutPixel(Cx - x, y + Cy);
		PutPixel(Cx - y, -x + Cy);
		PutPixel(Cx + x, -y + Cy);

		if (d < 0)
		{
			d += (2 * x) + 3;
		}
		else
		{
			d += 2 * (x - y) + 5;
			--y;
		}
		++x;
	}
}

void Application::PutPixel(int x, int y)
{
	putPixel(x, y, R, G, B, A);
}

void Application::lineTo(int X1, int Y1)
{
	Line(XF, YF, X1, Y1);
}

void Application::putPixel(int x, int y, char R, char G, char B, char A)
{
	putPixel(_screenBuffer, x, y, R, G, B, A);
}

void Application::Line(int X0, int Y0, int X1, int Y1)
{
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
		while (x <= X1) {
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
	moveTo(X1, Y1);
}

void Application::clearScreen()
{
	for (int i = 0; i < WIDTH * HEIGHT * RGBA; i++)
		_screenBuffer[i] = 0;
}

void Application::Start()
{
	/*moveTo(500, 500);

	v4 = CuboGen();*/
}

void Application::update()
{
	/*int angulo = 0;
	float rad = 0;
	angulo = angulo + 1 % 360;
	rad += 2 + angulo * PI / 180.0f;
	Vec4 V4;

	mRT = mRot.RotationY(rad);

	v4Final.clear();
	for (Vec4& i : v4)
	{
		V4 = i *= mRT;
		v4Final.push_back(v4);
	}*/

}

void Application::draw()
{
	clearScreen();
	setColor(0, 0, 255, 255);

}

Vec3 Application::puntoMedio(Vec3 o, Vec3 p)
{
	Vec3 result;
	result.x = (o.x + p.x) / 2.0f;
	result.y = (o.y + p.y) / 2.0f;
	result.z = (o.z + p.z) / 2.0f;
	return result;
}

std::vector<Vec3> Application::genGeometry(int vertNum, int radius)
{
	std::vector<Vec3> vector;
	float radiusOffset = 360 / vertNum;
	for (float i = 0; i < 360; i += radiusOffset)
	{
		float x = XF + (radius*cos(i * (3.1416 / 180)));
		float y = YF + (radius*sin(i * (3.1416 / 180)));
		Vec3 vec;
		vec.x = x;
		vec.y = y;
		vec.z = 0;
		vector.push_back(vec);
	}
	//moveTo(vertices[0].v[0], vertices[0].v[1]);
	//for (int i = 1; i < vertices.size() - 1; ++i)
	//	lineTo(vertices[1].v[0], vertices[i].v[1]);
	//lineTo(vertices[0].v[0], vertices[0].v[1]);

	return vector;
}

Vec4::Vec4(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
	w = 1;
}

Vec4::Vec4()
{
}

std::vector<Vec4> Application::CuboGen()
{
	std::vector<Vec4> cuboGen;

	Vec4 a = Vec4(-100, -100, 100);
	Vec4 b = Vec4(100, -100, 100);
	Vec4 c = Vec4(100, 100, 100);
	Vec4 d = Vec4(-100, 100, 100);
	Vec4 e = Vec4(-100, -100, -100);
	Vec4 f = Vec4(100, -100, -100);
	Vec4 g = Vec4(100, 100, -100);
	Vec4 h = Vec4(-100, 100, -100);

	cuboGen.push_back(a);
	cuboGen.push_back(b);
	cuboGen.push_back(c);
	cuboGen.push_back(d);
	cuboGen.push_back(e);
	cuboGen.push_back(f);
	cuboGen.push_back(g);
	cuboGen.push_back(h);
	
	return cuboGen;
}

void Application::Sierpinski(Vec3 a, Vec3 b, Vec3 c, int nivel)
{
	if (nivel == 0)
	{
		verticesA.push_back(a);
		verticesA.push_back(b);
		verticesA.push_back(c);
	}

	else
	{
		Vec3 ab, bc, ca;
		ab = puntoMedio(a, b);
		bc = puntoMedio(b, c);
		ca = puntoMedio(c, a);

		Sierpinski(a, ab, ca, nivel - 1);
		Sierpinski(ab, b, bc, nivel - 1);
		Sierpinski(ca, bc, c, nivel - 1);
	}
}

Matrix3 Application::MultiplicacionMatriz(Matrix3 R, Matrix3 T)
{
	Matrix3 finalMatrix;
	finalMatrix.matx[0].x = (R.matx[0].x * T.matx[0].x) + (R.matx[0].y * T.matx[1].x) + (R.matx[0].z * T.matx[2].x);
	finalMatrix.matx[0].y = (R.matx[0].x * T.matx[0].y) + (R.matx[0].y * T.matx[1].y) + (R.matx[0].z * T.matx[2].y);
	finalMatrix.matx[0].z = (R.matx[0].x * T.matx[0].z) + (R.matx[0].y * T.matx[1].z) + (R.matx[0].z * T.matx[2].z);

	finalMatrix.matx[1].x = (R.matx[1].x * T.matx[0].x) + (R.matx[1].y * T.matx[1].x) + (R.matx[1].z * T.matx[2].x);
	finalMatrix.matx[1].y = (R.matx[1].x * T.matx[0].y) + (R.matx[1].y * T.matx[1].y) + (R.matx[1].z * T.matx[2].y);
	finalMatrix.matx[1].z = (R.matx[1].x * T.matx[0].z) + (R.matx[1].y * T.matx[1].z) + (R.matx[1].z * T.matx[2].z);

	finalMatrix.matx[2].x = (R.matx[2].x * T.matx[0].x) + (R.matx[2].y * T.matx[1].x) + (R.matx[2].z * T.matx[2].x);
	finalMatrix.matx[2].y = (R.matx[2].x * T.matx[0].y) + (R.matx[2].y * T.matx[1].y) + (R.matx[2].z * T.matx[2].y);
	finalMatrix.matx[2].z = (R.matx[2].x * T.matx[0].z) + (R.matx[2].y * T.matx[1].z) + (R.matx[2].z * T.matx[2].z);

	return finalMatrix;
}

Matrix3 Matrix3::Translation(const float &x, const float &y)
{
	Matrix3 t;

	t.matx[0].x = 1;
	t.matx[0].y = 0;
	t.matx[0].z = x;
	t.matx[1].x = 0;
	t.matx[1].y = 1;
	t.matx[1].z = y;
	t.matx[2].x = 0;
	t.matx[2].y = 0;
	t.matx[2].z = 1;
	return Matrix3();
}

Vec3 & Vec3::operator*=(const Matrix3 & right)
{
	this->x = this->x * right.matx[0].x + (this->y * right.matx[0].y + (this->z * right.matx[0].z));
	this->y = this->x * right.matx[1].x + (this->y * right.matx[1].y + (this->z * right.matx[1].z));
	this->z = this->x * right.matx[2].x + (this->y * right.matx[2].y + (this->z * right.matx[2].z));

	return *this;
	// TODO: insert return statement here
}

Matrix3 & Matrix3::operator*(const Matrix3 & right)
{
	return Application::MultiplicacionMatriz(*this, right);
	// TODO: insert return statement here
}

Matrix4 Application::MultiplicacionMatriz4(Matrix4 R, Matrix4 T)
{
	Matrix4 finalMatrix4;
	finalMatrix4.matx[0].x = (R.matx[0].x * T.matx[0].x) + (R.matx[0].y * T.matx[1].x) + (R.matx[0].z * T.matx[2].x);
	finalMatrix4.matx[0].y = (R.matx[0].x * T.matx[0].y) + (R.matx[0].y * T.matx[1].y) + (R.matx[0].z * T.matx[2].y);
	finalMatrix4.matx[0].z = (R.matx[0].x * T.matx[0].z) + (R.matx[0].y * T.matx[1].z) + (R.matx[0].z * T.matx[2].z);
	finalMatrix4.matx[0].w = (R.matx[0].x * T.matx[0].w) + (R.matx[0].y * T.matx[1].w) + (R.matx[0].z * T.matx[2].w);

	finalMatrix4.matx[1].x = (R.matx[1].x * T.matx[0].x) + (R.matx[1].y * T.matx[1].x) + (R.matx[1].z * T.matx[2].x);
	finalMatrix4.matx[1].y = (R.matx[1].x * T.matx[0].y) + (R.matx[1].y * T.matx[1].y) + (R.matx[1].z * T.matx[2].y);
	finalMatrix4.matx[1].z = (R.matx[1].x * T.matx[0].z) + (R.matx[1].y * T.matx[1].z) + (R.matx[1].z * T.matx[2].z);
	finalMatrix4.matx[1].w = (R.matx[1].x * T.matx[0].w) + (R.matx[1].y * T.matx[1].w) + (R.matx[1].z * T.matx[2].w);

	finalMatrix4.matx[2].x = (R.matx[2].x * T.matx[0].x) + (R.matx[2].y * T.matx[1].x) + (R.matx[2].z * T.matx[2].x);
	finalMatrix4.matx[2].y = (R.matx[2].x * T.matx[0].y) + (R.matx[2].y * T.matx[1].y) + (R.matx[2].z * T.matx[2].y);
	finalMatrix4.matx[2].z = (R.matx[2].x * T.matx[0].z) + (R.matx[2].y * T.matx[1].z) + (R.matx[2].z * T.matx[2].z);
	finalMatrix4.matx[2].w = (R.matx[2].x * T.matx[0].w) + (R.matx[2].y * T.matx[1].w) + (R.matx[2].z * T.matx[2].w);

	finalMatrix4.matx[3].x = (R.matx[3].x * T.matx[0].x) + (R.matx[3].y * T.matx[1].x) + (R.matx[3].z * T.matx[2].x);
	finalMatrix4.matx[3].y = (R.matx[3].x * T.matx[0].y) + (R.matx[3].y * T.matx[1].y) + (R.matx[3].z * T.matx[2].y);
	finalMatrix4.matx[3].z = (R.matx[3].x * T.matx[0].z) + (R.matx[3].y * T.matx[1].z) + (R.matx[3].z * T.matx[2].z);
	finalMatrix4.matx[3].w = (R.matx[3].x * T.matx[0].w) + (R.matx[3].y * T.matx[1].w) + (R.matx[3].z * T.matx[2].w);

	return finalMatrix4;
}

Matrix4 Matrix4::RotationX(const float & angle)
{
	Matrix4 r;

	r.matx[0].x = 1;
	r.matx[0].y = 0;
	r.matx[0].z = 0;
	r.matx[0].w = 0;

	r.matx[1].x = 0;
	r.matx[1].y = cos(degtorad * angle);
	r.matx[1].z = -sin(degtorad * angle);
	r.matx[1].w = 0;

	r.matx[2].x = 0;
	r.matx[2].y = sin(degtorad * angle);
	r.matx[2].z = cos(degtorad * angle);
	r.matx[2].w = 0;

	r.matx[3].x = 0;
	r.matx[3].y = 0;
	r.matx[3].z = 0;
	r.matx[3].w = 1;

	return r;
}

Matrix4 Matrix4::RotationY(const float & angle)
{
	Matrix4 r;

	r.matx[0].x = cos(degtorad * angle);
	r.matx[0].y = 0;
	r.matx[0].z = sin(degtorad * angle);
	r.matx[0].w = 0;

	r.matx[1].x = 0;
	r.matx[1].y = 1;
	r.matx[1].z = 0;
	r.matx[1].w = 0;

	r.matx[2].x = sin(degtorad * angle);
	r.matx[2].y = 0;
	r.matx[2].z = cos(degtorad * angle);
	r.matx[2].w = 0;

	r.matx[3].x = 0;
	r.matx[3].y = 0;
	r.matx[3].z = 0;
	r.matx[3].w = 1;

	return r;
}

Matrix4 Matrix4::RotationZ(const float & angle)
{
	Matrix4 r;

	r.matx[0].x = cos(degtorad * angle);
	r.matx[0].y = -sin(degtorad * angle);
	r.matx[0].z = 0;
	r.matx[0].w = 0;

	r.matx[1].x = sin(degtorad * angle);
	r.matx[1].y = cos(degtorad * angle);;
	r.matx[1].z = 0;
	r.matx[1].w = 0;

	r.matx[2].x = 0;
	r.matx[2].y = 0;
	r.matx[2].z = 1;
	r.matx[2].w = 0;

	r.matx[3].x = 0;
	r.matx[3].y = 0;
	r.matx[3].z = 0;
	r.matx[3].w = 1;

	return r;
}

Matrix4 Matrix4::Translation(const float & x, const float & y, const float & z)
{
	Matrix4 t;

	t.matx[0].x = 1;
	t.matx[0].y = 0;
	t.matx[0].z = 0;
	t.matx[0].w = x;

	t.matx[1].x = 0;
	t.matx[1].y = 1;
	t.matx[1].z = 0;
	t.matx[1].w = y;

	t.matx[2].x = 0;
	t.matx[2].y = 0;
	t.matx[2].z = 1;
	t.matx[2].w = z;

	t.matx[3].x = 0;
	t.matx[3].y = 0;
	t.matx[3].z = 0;
	t.matx[3].w = 1;
 
	return Matrix4();
}

Vec4 & Vec4::operator*=(const Matrix4 & right)
{
	this->x = this->x * right.matx[0].x + (this->y * right.matx[0].y + (this->z * right.matx[0].z));
	this->y = this->x * right.matx[1].x + (this->y * right.matx[1].y + (this->z * right.matx[1].z));
	this->z = this->x * right.matx[2].x + (this->y * right.matx[2].y + (this->z * right.matx[2].z));
	this->w = this->x * right.matx[3].x + (this->y * right.matx[3].y + (this->z * right.matx[3].z));

	return *this;
	// TODO: insert return statement here
}

Matrix4 & Matrix4::operator*(const Matrix4 & right)
{
	return Application::MultiplicacionMatriz4(*this, right);
	// TODO: insert return statement here
}






