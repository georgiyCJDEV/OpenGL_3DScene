// STD includes
#include <iostream>

// GLEW
#include "GL/glew.h"

// My includes
#include "GLDebug.h"

void GLClearError()
{
	/* ���� ����� ����������� ���� ������ �� ������ */
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	/* ���� ����� ����������� ���� error �� ����� ����� 0 */
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ")" <<
			function << " " << file << ":" <<
			line << std::endl;
		return false;
	}
	return true;
}