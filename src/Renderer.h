#pragma once

// #include <GL/glew.h>

#include "Debug.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {
    public:
        void clear() const;
        void drawArrays(const VertexArray& va, const Shader& shader) const;
        void drawElements(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};