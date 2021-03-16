#pragma once

#include <vector>
// #include <GL/glew.h>
#include "Debug.h"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type) {
        switch (type)
        {
            case GL_FLOAT         : return sizeof(GLfloat);
            case GL_UNSIGNED_INT  : return sizeof(GLuint);
            case GL_UNSIGNED_BYTE : return sizeof(GLbyte);
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout {
    private:
        unsigned int m_Stride;
        std::vector<VertexBufferElement> m_Elements;

    public:
        VertexBufferLayout():
            m_Stride(0) { }

        void addFloat(unsigned int count)        { push(GL_FLOAT, count, GL_FALSE);        }
        void addUnsignedInt(unsigned int count)  { push(GL_UNSIGNED_INT, count, GL_FALSE); }
        void addUnsignedByte(unsigned int count) { push(GL_UNSIGNED_BYTE, count, GL_TRUE); }

        inline const std::vector<VertexBufferElement> getElements() const { return m_Elements; };
        inline unsigned int getStride() const { return m_Stride; };

    private:
        void push(unsigned int type, unsigned int count, unsigned char normalized)
        {
            struct VertexBufferElement vbe = {type, count, normalized};
            m_Elements.push_back(vbe);
            m_Stride += count * VertexBufferElement::getSizeOfType(type);
        };
};
