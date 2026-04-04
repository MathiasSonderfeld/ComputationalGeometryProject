#ifndef CG_GLFW_WINDOW_H
#define CG_GLFW_WINDOW_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#if defined(IMGUI_IMPL_OPENGL_ES2)
//#include <GLES2/gl2.h>
//#endif


#include "imgui.h"

class CgBaseGui;


class CgGLFWwindow
{
public:
    CgGLFWwindow();
    ~CgGLFWwindow();

    void init(int, int);
    void setGui(CgBaseGui* gui);
    void show() const;

private:
    static void glfw_error_callback(int error, const char* description);
    static void glfw_window_size_callback(GLFWwindow* window, int size_x, int size_y);


    GLFWwindow* window{};
    ImVec4 clear_color;
    CgBaseGui* myGui{};
};

#endif

