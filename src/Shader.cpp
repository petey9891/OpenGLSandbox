#include "Renderer.h"
#include "Shader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& filepath): m_RendererID(0), m_FilePath(filepath) {
    // printf("filepath: %s\n", filepath.c_str());
    ShaderProgramSource source = this->parseShader(filepath);

    printf("\nVertex Shader:\n%s\n", source.vertexSource.c_str());
    printf("\nFragment Shader:\n%s\n\n", source.fragmentSource.c_str());

    this->m_RendererID = this->createShader(source.vertexSource, source.fragmentSource);

    GLCall(glUseProgram(this->m_RendererID));
}

Shader::~Shader() {
    GLCall(glDeleteProgram(this->m_RendererID));
}

void Shader::bind() const {
    GLCall(glUseProgram(this->m_RendererID));
}

void Shader::unbind() const {
    GLCall(glUseProgram(0));
}

int Shader::getUniformLocation(const std::string& name) {
    if (this->m_UniformLocationCache.find(name) != this->m_UniformLocationCache.end())
        return this->m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(this->m_RendererID, name.c_str()));
    if (location == -1)
        printf("No active uniform variable with name %s found\n", name.c_str());

    this->m_UniformLocationCache[name] = location;

    return location;
}

void Shader::setUniform1f(const std::string& name, float value) {
    GLCall(glUniform1f(this->getUniformLocation(name), value));
}

void Shader::setUniform4f(const std::string& name, float v1, float v2, float v3, float v4) {
    GLCall(glUniform4f(this->getUniformLocation(name), v1, v2, v3, v4));
}

void Shader::setUniform1fv(const std::string& name, float count, const float* value) {
    GLCall(glUniform1fv(this->getUniformLocation(name), count, value));
}

enum ShaderType
{
    NONE = -1, VERTEX = 0, FRAGMENT = 1
};

struct ShaderProgramSource Shader::parseShader(const std::string& filepath) {

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    struct ShaderProgramSource sps = { ss[0].str(), ss[1].str() };
    return sps;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    std::string shaderType = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    printf("%s shader compile status: %d\n", shaderType.c_str(), result);
    if ( result == GL_FALSE )
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*) alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        printf("Failed to compile %s shader\n", shaderType.c_str());
        printf("%s\n", message);

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    // create a shader program
    unsigned int program = glCreateProgram();
    unsigned int vs = this->compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = this->compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));

    GLint program_linked;

    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &program_linked));
    std::cout << "Program link status: " << program_linked << std::endl;
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        GLCall(glGetProgramInfoLog(program, 1024, &log_length, message));
        std::cout << "Failed to link program" << std::endl;
        std::cout << message << std::endl;
    }

    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}