#ifndef CG_GLFW_WINDOW_H
#define CG_GLFW_WINDOW_H


// ReSharper disable once CppUnusedIncludeDirective
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

class CgBaseGui;


class CgGlfwWindow
{
public:
    CgGlfwWindow();
    ~CgGlfwWindow();

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

