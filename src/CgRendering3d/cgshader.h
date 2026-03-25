#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class CgShader
    {

    private:
        unsigned int m_id;

        unsigned int get_compiled_shader(const char* shader_code, GLenum type);

    public:
        CgShader() = default;

        // Load a vertex and a fragment shader from file
        bool load(const std::string& vertexshader_file, const std::string& fragmentshader_file);

        void create_shaders();
        
        // Use the program
        void use();

        // Delete the program
        void unload();

        unsigned int get_program_id() { return m_id; }

        void setUniformValue(std::string name,glm::mat4 val);
        void setUniformValue(std::string name,glm::mat3 val);
        void setUniformValue(std::string name,glm::vec3 val);
        void setUniformValue(std::string name,glm::vec4 val);
        void setUniformValue(std::string name,int val);
        void setUniformValue(std::string name,float val);
        
        


    };

#endif
