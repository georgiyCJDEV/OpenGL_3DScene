// My includes
#include "GLDebug.h"
#include "VertexArray.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::unbind() const
{
    GLCall(glBindVertexArray(0));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    bind();
	vb.bind();

    const auto& elements = layout.getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(
            i,                   
            element.count,       
            element.type,        
            element.normalized,  
            layout.getStride(i),                 
            NULL 
        ));
    }

    vb.unbind();
    unbind();
}

void VertexArray::addBuffer(const VertexBuffer* vb, const VertexBufferLayout& layout)
{
    bind();

    const auto& elements = layout.getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        vb[i].bind();

        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(
            i,                   
            element.count,       
            element.type,        
            element.normalized,  
            layout.getStride(i),                  
            NULL
        ));

        vb[i].unbind();
    }

    unbind();
}
