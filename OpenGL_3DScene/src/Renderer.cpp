// STD includes
#include <iostream>

// My includes
#include "Renderer.h"
#include "GLDebug.h"

void Renderer::clear() const
{
    GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::draw(const unsigned int& drawMode, const VertexArray& va, const IndexBuffer& ib, Shader& shader ) const
{
	va.bind();
	ib.bind();

	shader.bind();

	GLCall(glDrawElements(drawMode, ib.getCount(), GL_UNSIGNED_INT, nullptr));
	
	ib.unbind();
	va.unbind();
}

void Renderer::draw(const unsigned int& drawMode, const VertexArray& va, const IndexBuffer& ib, Shader& shader, unsigned int& texId) const
{
	shader.bind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	va.bind();
	ib.bind();

	GLCall(glDrawElements(drawMode, ib.getCount(), GL_UNSIGNED_INT, nullptr));

	ib.unbind();
	va.unbind();
}

void Renderer::draw(const unsigned int& drawMode, const int& firstVertex, const unsigned int& count, const VertexArray& va, Shader& shader) const
{
	shader.bind();

    va.bind();

    GLCall(glDrawArrays(drawMode, firstVertex, count));

	va.unbind();
}

void Renderer::draw(const unsigned int& drawMode, const int& firstVertex, const unsigned int& count, const VertexArray& va, Shader& shader, unsigned int& texId) const
{
	shader.bind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	va.bind();

	GLCall(glDrawArrays(drawMode, firstVertex, count));

	va.unbind();
}

