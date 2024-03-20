#pragma once

// GLEW
#include <GL/glew.h>

// My includes
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{
public:
    void clear() const;

    void draw(const unsigned int& drawMode, const VertexArray& va, const IndexBuffer& ib, Shader& shader) const;
    void draw(const unsigned int& drawMode, const int& firstVertex, const unsigned int& count, const VertexArray& va, Shader& shader) const;
	void draw(const unsigned int& drawMode, const int& firstVertex, const unsigned int& count, const VertexArray& va, Shader& shader, unsigned int& texId) const;
};