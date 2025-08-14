#pragma once

#include "core.h"

class Ground {

private:

	GLuint				m_numFaces = 0;
	GLuint				m_vao = 0;

	GLuint				m_vertexBuffer;
	GLuint				m_colourBuffer;
	GLuint				m_indexBuffer;

public:

	Ground();
	~Ground();

	void render();
};