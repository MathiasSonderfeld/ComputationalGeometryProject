#define GL_SILENCE_DEPRECATION
#define LOESUNG


#include <algorithm>
#include <string>
#include <iostream>
#include <cstdlib>
#include <stdio.h>

int window_size_x = 1280;
int window_size_y = 720;




#include "cgglfwwindow.h"
#include "cgopenglrenderinggui.h"



static int sNextId = 0;
int getUniqueId() { return ++sNextId; }


// Main code
int main(int argc, char** argv)
{
    std::string current_path = (std::string)argv[0];
    current_path.erase(current_path.find_last_of("/")+1);
    
    if(argc==3)
    {
        window_size_x = atoi(argv[1]);
        window_size_y = atoi(argv[2]);
    }
    
    CgGLFWwindow mainwindow;
    mainwindow.init(window_size_x,window_size_y);

    // setup Gui for OpenGL Rendering
    CgOpenGLRenderingGui myGui(current_path);
    mainwindow.setGui((CgBaseGui*)&myGui);
    
    // setup Gui for Image Processing
    // CgImageViewer myGui(current_path);
    // mainwindow.setGui((CgBaseGui*)&myGui);

    mainwindow.show();

   

    return 0;
}
