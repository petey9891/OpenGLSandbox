  
#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
    std::string vertexSource;
    std::string fragmentSource;
};

class Shader {
    public: 
        unsigned int m_RendererID;
    private:
        std::string m_FilePath;
        std::unordered_map<std::string, int> m_UniformLocationCache;

    public:
        Shader(const std::string& filepath);
        ~Shader();

        void bind() const;
        void unbind() const;

        void setUniform1f(const std::string& name, float value);
        void setUniform4f(const std::string& name, float v1, float v2, float v3, float v4);
        void setUniform1fv(const std::string& name, float count, const float* value);


    private:
        int getUniformLocation(const std::string& name);
        struct ShaderProgramSource parseShader(const std::string& filepath);
        unsigned int compileShader(unsigned int type, const std::string& source);
        unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);

};