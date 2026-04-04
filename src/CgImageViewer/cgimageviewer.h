#ifndef CG_IMAGE_VIEWER_H
#define CG_IMAGE_VIEWER_H

#include <string>
#include "cgbasegui.h"
#include "cgimage.h"
#include "imgui.h"

class CgImageViewer : public CgBaseGui
{
public:
    explicit CgImageViewer(std::string current_path);
    CgImageViewer();
    ~CgImageViewer() override;

    void showGUI(int window_size_x, int window_size_y) override;
    void initGUI() override;

    void renderObjects() override
    {
    };

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
