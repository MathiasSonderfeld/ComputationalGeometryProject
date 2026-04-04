#include "cgshader.h"
#include "shadercodephong.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

unsigned int CgShader::get_compiled_shader(const char* shader_code, GLenum type)
{
    const unsigned int current_shader_id = glCreateShader(type);

    const GLchar* code[1];
    code[0] = shader_code;

    GLint code_length[1];
    code_length[0] = strlen(shader_code);

    glShaderSource(current_shader_id, 1, code, code_length);
    glCompileShader(current_shader_id);

    GLint result;
    glGetShaderiv(current_shader_id, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        int length;
        glGetShaderiv(current_shader_id, GL_INFO_LOG_LENGTH, &length);

        GLchar* strInfoLog = new GLchar[length + 1];
        glGetShaderInfoLog(current_shader_id, length, &length, strInfoLog);

        std::cout << "Error compiling " << type << " shader: " << strInfoLog << "\n";
        delete[] strInfoLog;
    }

    std::cout << "shader compiled" << std::endl;
    return current_shader_id;
}

void CgShader::create_shaders()
{
    m_id = glCreateProgram();

#if defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const unsigned int vs = get_compiled_shader(vertex_shader_code_150, GL_VERTEX_SHADER);
    const unsigned int fs = get_compiled_shader(fragment_shader_code_150, GL_FRAGMENT_SHADER);
#else
    // GL 3.0 + GLSL 130
    const unsigned int vs = get_compiled_shader(vertex_shader_code_130, GL_VERTEX_SHADER);
    const unsigned int fs = get_compiled_shader(fragment_shader_code_130, GL_FRAGMENT_SHADER);
#endif


    GLint result = 0;
    GLchar log[1024] = {0};

    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);

    glBindAttribLocation(m_id, 0, "pos");
    glBindAttribLocation(m_id, 1, "normal");
    glBindAttribLocation(m_id, 2, "col");


    glLinkProgram(m_id);
    glValidateProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
        std::cout << "Error linking program:\n" << log << '\n';
        return;
    }
}


void CgShader::setUniformValue(const std::string& name, glm::mat4 val) const
{
    const int loc = glGetUniformLocation(m_id, name.c_str());
    glUseProgram(m_id);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void CgShader::setUniformValue(const std::string& name, glm::mat3 val) const
{
    int loc = glGetUniformLocation(m_id, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void CgShader::setUniformValue(const std::string& name, const glm::vec3 val) const
{
    int loc = glGetUniformLocation(m_id, name.c_str());
    glUniform3f(loc, val.x, val.y, val.z);
}

void CgShader::setUniformValue(const std::string& name, const glm::vec4 val) const
{
    int loc = glGetUniformLocation(m_id, name.c_str());
    glUniform4f(loc, val.x, val.y, val.z, val.w);
}

void CgShader::setUniformValue(const std::string& name, const int val) const
{
    int loc = glGetUniformLocation(m_id, name.c_str());
    glUniform1i(loc, val);
}

void CgShader::setUniformValue(const std::string& name, const float val) const
{
    int loc = glGetUniformLocation(m_id, name.c_str());
    glUniform1f(loc, val);
}

void CgShader::use() const
{
    glUseProgram(m_id);
}

void CgShader::unload() const
{
    glDeleteProgram(m_id);
}


