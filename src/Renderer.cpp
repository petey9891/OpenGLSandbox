#include "Renderer.h"
#include <iostream>

void Renderer::clear() const {
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::drawArrays(const VertexArray& va, const Shader& shader) const {
    shader.bind();
    va.bind();

    GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 12));
}

void Renderer::drawElements(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    shader.bind();
    va.bind();
    ib.bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}