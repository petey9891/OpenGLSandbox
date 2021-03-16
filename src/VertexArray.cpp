#include "VertexArray.h"
#include "Debug.h"

VertexArray::VertexArray() {
    GLCall(glGenVertexArrays(1, &this->m_RendererID));
}

VertexArray::~VertexArray() {
    GLCall(glDeleteVertexArrays(1, &this->m_RendererID));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    this->bind();
    vb.bind();
    const std::vector<VertexBufferElement> elements = layout.getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size() ; i++) {
        const VertexBufferElement element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized,
                                      layout.getStride(), INT2VOIDP(offset)));
        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
}

void VertexArray::bind() const {
    GLCall(glBindVertexArray(this->m_RendererID));
}

void VertexArray::unbind() const {
    GLCall(glBindVertexArray(0));
}