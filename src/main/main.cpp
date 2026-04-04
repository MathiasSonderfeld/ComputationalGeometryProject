#define GL_SILENCE_DEPRECATION
#define LOESUNG


#include <algorithm>
#include <string>
#include <iostream>

#include "CgGlfwWindow.h"
#include "cgopenglrenderinggui.h"

int window_size_x = 1280;
int window_size_y = 720;


static int sNextId = 0;
int getUniqueId() { return ++sNextId; }


// Main code
int main(const int argc, char** argv)
{
    std::string current_path = argv[0];
    current_path.erase(current_path.find_last_of('/') + 1);

    if (argc == 3)
    {
        window_size_x = static_cast<int>(strtol(argv[1], nullptr, 10));
        window_size_y = static_cast<int>(strtol(argv[2], nullptr, 10));
    }

    CgGLFWwindow main_window;
    main_window.init(window_size_x, window_size_y);

    // setup Gui for OpenGL Rendering
    CgOpenGLRenderingGui myGui(current_path);
    main_window.setGui(&myGui);

    // setup Gui for Image Processing
    // CgImageViewer myGui(current_path);
    // main_window.setGui((CgBaseGui*)&myGui);

    main_window.show();


    return 0;
}
