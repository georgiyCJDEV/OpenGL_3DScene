#pragma once

// STD includes
#include <vector>
#include <assert.h>

// GLEW
#include "GL/glew.h"

struct VertexBufferElement 
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	unsigned int stride;

	static unsigned int getSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return 4;

		case GL_UNSIGNED_INT:
			return 4;

		case GL_UNSIGNED_BYTE:
			return 1;
		}
		assert(false);
		return 0;
	}
};

class VertexBufferLayout
{

private:
	std::vector<VertexBufferElement> m_Elements;

public:
	VertexBufferLayout() {}

	template<typename T>
	void push(unsigned int count) // В функцию передан необрабатываемый тип данных
	{
		assert(false);
	}
	
	template<>
	void push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE, VertexBufferElement::getSizeOfType(GL_FLOAT) * count });
	}

	template<>
	void push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE, VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT) * count });
	}

	template<>
	void push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE, VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE) * count });
	}

	inline const std::vector<VertexBufferElement> getElements() const { return m_Elements; }
	inline unsigned int getStride(int index) const { return m_Elements[index].stride; }
};