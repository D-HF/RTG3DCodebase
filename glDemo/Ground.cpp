#include "Ground.h"

using namespace std;
using namespace glm;

static float positionArray[] = {
	//x, y, z, w
	//top 4 vertices 
	-10.0f, -1.0, 10.0f, 1.0f, //left top front 0
	-10.0f, -1.0, -10.0f, 1.0f, //left top back 1
	10.0f, -1.0, -10.0f, 1.0f, //right top back 2
	10.0f, -1.0, 10.0f, 1.0f, //right top front 3

	//bottom 4 vertices
	-10.0f, -1.5, 10.0f, 1.0f, //left bottom front 4
	-10.0f, -1.5, -10.0f, 1.0f, //left bottom back 5
	10.0f, -1.5, -10.0f, 1.0f, //right bottom back 6
	10.0f, -1.5, 10.0f, 1.0f, //right bottom front 7

	//front 4 vertices
	-10.0f, -1.0, 10.0f, 1.0f, //left top front 8
	-10.0f, -1.5, 10.0f, 1.0f, //left bottom front 9
	10.0f, -1.5, 10.0f, 1.0f, //right bottom front 10
	10.0f, -1.0, 10.0f, 1.0f, //right top front 11

	//back 4 vertices
	-10.0f, -1.0, -10.0f, 1.0f, //left top back 12
	-10.0f, -1.5, -10.0f, 1.0f, //left bottom back 13
	10.0f, -1.5, -10.0f, 1.0f, //right bottom back 14
	10.0f, -1.0, -10.0f, 1.0f, //right top back 15

	//left 4 vertices
	-10.0f, -1.0, 10.0f, 1.0f, //left top front 16
	-10.0f, -1.5, 10.0f, 1.0f, //left bottom front 17
	-10.0f, -1.5, -10.0f, 1.0f, //left bottom back 18
	-10.0f, -1.0, -10.0f, 1.0f, //left top back 19

	//right 4 vertices
	10.0f, -1.0, 10.0f, 1.0f, //right top front 20
	10.0f, -1.5, 10.0f, 1.0f, //right bottom front 21
	10.0f, -1.5, -10.0f, 1.0f, //right bottom back 22
	10.0f, -1.0, -10.0f, 1.0f, //right top back 23
};

static float colourArray[] = {

	//top 4 vertices
	0.031, 0.2, 0.02f, 1.0f, 
	0.031, 0.2, 0.02f, 1.0f,
	0.031, 0.2, 0.02f, 1.0f,
	0.031, 0.2, 0.02f, 1.0f,

	//bottom 4 vertices
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,

	//front 4 vertices 
	0.031, 0.2, 0.02f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.031, 0.2, 0.02f, 1.0f,

	//back 4 vertices 
	0.031, 0.2, 0.02f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.031, 0.2, 0.02f, 1.0f,

	//left 4 vertices 
	0.031, 0.2, 0.02f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.031, 0.2, 0.02f, 1.0f,

	//right 4 vertices
	0.031, 0.2, 0.02f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.169f, 0.106f, 0.039f, 1.0f,
	0.031, 0.2, 0.02f, 1.0f,
};

static unsigned int indexArray[] = {

	// Top face 
	// //clockwise as viewed from outside
	2, 1, 0,
	3, 2, 0,

	// Bottom face
	5, 6, 4,
	6, 7, 4,

	// Front face
	//0, 4, 3,
	//4, 7, 3,
	8, 9, 11,
	9, 10, 11,

	// Back face
	//2, 6, 1,
	//6, 5, 1,
	15, 14, 12,
	14, 13, 12,

	// Left face
	//0, 1, 5,
	//4, 0, 5,
	16, 19, 18,
	17, 16, 18,

	// Right face
	//3, 7, 2,
	//7, 6, 2,
	20, 21, 23,
	21, 22, 23
};

Ground::Ground(){
	m_numFaces = 6 * 2;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// setup vbo for position attribute
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 96 * sizeof(float), positionArray, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup vbo for colour attribute
	glGenBuffers(1, &m_colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, 96 * sizeof(float), colourArray, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(4);

	// setup vbo for cube) index buffer
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);

}//constructor

Ground::~Ground() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_colourBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
}//deconstructor

void Ground::render() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, (const GLvoid*)0);
}