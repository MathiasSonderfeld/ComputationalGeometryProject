#ifndef FRAMEBUFFER_SHADER
#define FRAMEBUFFER_SHADER

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "cgglbufferobject.h"
#include "cgtrackball.h"


class CgBaseRenderableObject;
class CgBaseTriangleMesh;
class CgGLFrameBuffer;
class CgShader;
class CgOpenGLRenderingGui;
class CgBufferObjectHolder;

class CgFrameBufferShader
{

private:
    void create_triangle();
    unsigned int get_compiled_shader(GLuint program, const char* shader_code, GLenum type);
    void create_shaders();
    void create_framebuffer();
    void bind();
    void unbind();
    void rescale(float width, float height);
    
    // x,y mjust be normalized into [-1,1]
    void calculatePickRayInWorldCoordinates(double x, double y);
    void calculateMovement(double x, double y);
    
public:

    CgFrameBufferShader();
    ~CgFrameBufferShader();
    
   
    void init();
    void show();
    void setOpenGLRenderingGui(CgOpenGLRenderingGui*);
    
    void initObject(CgBaseRenderableObject* obj);
    void removeObject(CgBaseRenderableObject* obj);
    void renderObject(CgBaseRenderableObject*);
    
   
    void setPolygonMode(int mode);
    void setShadeMode(int mode);
    void setLightingMode(bool mode);
 
    void setModelViewMatrix(glm::mat4 val);
    void setLookAtMatrix(glm::mat4 val);
    void setProjectionMatrix(glm::mat4 val);
    void setCustomRotationMatrix(glm::mat4 val);
   
  

private:
    
    int m_polygon_mode;
    int m_shade_mode;
    bool m_lighting_mode;
  
    
    glm::mat4 m_modelview_matrix;
    glm::mat4 m_lookat_matrix;
    glm::mat4 m_projection_matrix;
    glm::mat4 m_global_scale_matrix;
    glm::mat4 m_custom_rotation_matrix;
    
    std::map<unsigned int,CgGLBufferObject*> m_gl_buffer_objects;
    
    CgGLFrameBuffer* m_framebuffer;
    ImVec2 m_pos; // position of texture in window --> needed for mouse position calc.
    CgShader* m_shaderprogram;
    CgTrackball* m_trackball;
    CgOpenGLRenderingGui* m_controller;
    
    GLuint my_image_texture ;
    
    void handleMouseEvents();
    void handleKeyEvents();
    
    ImVec2 m_old_mouse_pos;
   
};

#endif
