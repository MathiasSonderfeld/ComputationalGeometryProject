#ifndef CG_IMAGEVIEWER_H
#define CG_IMAGEVIEWER_H

#include <string>

#include "cgbasegui.h"
#include "cgimage.h"

class CgImageViewer : public CgBaseGui{


public:
    
    CgImageViewer(std::string current_path);
    CgImageViewer();
    ~CgImageViewer();
    
    void showGUI(int windowsize_x,int windowsize_y);
    void initGUI();
    void renderObjects(){};
    
private:
    
    void createTestImageView();
    void createAufgabenTabBar();
    void createAufgabe1Tab();
    void createAufgabe2Tab();
    void createAufgabe3Tab();
    void createAufgabe4Tab();
    void createAufgabe5Tab();
    
    // variables for gui
    bool show_demo_window;
    std::string gui_current_path;
    CgImage my_image;
    
    // variables for imageview
    std::string imageview_filename;
    
    // variables for Aufgabe 1
    ImVec4 a1_cross_color;
    int a1_linewidth;
};

#endif
