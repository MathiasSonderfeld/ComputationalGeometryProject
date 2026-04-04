#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class CgShader
{
private:
    unsigned int m_id;

    static unsigned int get_compiled_shader(const char* shader_code, GLenum type);

public:
    CgShader() = default;

    // Load a vertex and a fragment shader from file
    bool load(const std::string& vertex_shader_file, const std::string& fragment_shader_file);

    void create_shaders();

    // Use the program
    void use() const;

    // Delete the program
    void unload() const;

    unsigned int get_program_id() const { return m_id; }

    void setUniformValue(const std::string& name, glm::mat4 val) const;
    void setUniformValue(const std::string& name, glm::mat3 val) const;
    void setUniformValue(const std::string& name, glm::vec3 val) const;
    void setUniformValue(const std::string& name, glm::vec4 val) const;
    void setUniformValue(const std::string& name, int val) const;
    void setUniformValue(const std::string& name, float val) const;
};

#endif
