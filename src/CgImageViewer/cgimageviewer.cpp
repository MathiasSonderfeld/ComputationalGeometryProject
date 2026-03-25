#include "cgimageviewer.h"
#include "imgui.h"

CgImageViewer::CgImageViewer()
{
    a1_cross_color = ImVec4(1.f, 0.f, 0.f, 1.00f);
    a1_linewidth=0;
    gui_current_path="";
    show_demo_window=true;
}

CgImageViewer::CgImageViewer(std::string current_path)
{
    gui_current_path=current_path;
    show_demo_window=true;
}


CgImageViewer::~CgImageViewer()
{
    
}

void CgImageViewer::initGUI()
{
    
}

void CgImageViewer::showGUI(int windowsize_x, int windowsize_y)
{
    //Load and show test Image
    ImGui::SetNextWindowPos(ImVec2(10,10));
    ImGui::SetNextWindowSize(ImVec2(windowsize_x/2-20,windowsize_y-20));
    createTestImageView();
   
    if(show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(windowsize_x/2+10,10));
        ImGui::SetNextWindowSize(ImVec2(windowsize_x/2-20,windowsize_y/3));
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(windowsize_x/2+10,10));
        ImGui::SetNextWindowSize(ImVec2(windowsize_x/2-20,windowsize_y-20));
    }
    
    // create gui for all exercises, see mygui.h for details
    createAufgabenTabBar();
    
    // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()!
    // You can browse its code to learn more about Dear ImGui!).
    ImGui::SetNextWindowPos(ImVec2(windowsize_x/2+10,windowsize_y/3+20));
    ImGui::SetNextWindowSize(ImVec2(windowsize_x/2-20,windowsize_y/3*2-30));
    if(show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    ImGui::Render();
}



void CgImageViewer::createTestImageView()
{
    ImGui::Begin("Test Image");
    ImGui::Text("Select Image:");

    const char* items[] = { "./images/Lena.png", "./images/chess.png", "./images/desert.png", "./images/street2.jpg"};
         static int item_current = 0;
         
    ImGui::SameLine();
    if (ImGui::Button("load"))
    {
        imageview_filename = "";
        imageview_filename+=gui_current_path;
        imageview_filename+=(std::string)items[item_current];
        my_image.LoadFromFile(imageview_filename.c_str());
    }

     ImGui::ListBox(" ", &item_current, items, IM_ARRAYSIZE(items), 4);
     ImGui::Image((void*)(intptr_t)my_image.getImageTexture(), ImVec2(my_image.getImageWidth(), my_image.getImageHeight()));
     ImGui::Text("size = %d x %d", my_image.getImageWidth(), my_image.getImageHeight());
     ImGui::End();
}



void CgImageViewer::createAufgabenTabBar()
{
    
    
    
    ImGui::Begin("Wähle Aufgabe");
    
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    ImGui::BeginTabBar("Wähle Aufgabe", tab_bar_flags);
    
        if (ImGui::BeginTabItem("Aufgabe 1"))
        {
            createAufgabe1Tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Aufgabe 2"))
        {
            createAufgabe2Tab();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Aufgabe 3"))
        {
            createAufgabe3Tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Aufgabe 4"))
        {
            createAufgabe4Tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Aufgabe 5"))
        {
            createAufgabe5Tab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    
    
    ImGui::End();
   
}
void CgImageViewer::createAufgabe1Tab()
{
    ImGui::Text("draw a cross with variable color and linewith");
    ImGui::Text("linewidth: ");
    ImGui::SameLine();
    
    if (ImGui::SliderInt(" ", &a1_linewidth, 0, 30))
        my_image.drawCross(a1_cross_color.x*255,a1_cross_color.y*255,a1_cross_color.z*255,a1_linewidth);
    ImGui::Text("cross color: ");
    ImGui::SameLine();
    if (ImGui::ColorEdit3("", (float*)&a1_cross_color))
        my_image.drawCross(a1_cross_color.x*255,a1_cross_color.y*255,a1_cross_color.z*255,a1_linewidth);
    if (ImGui::Button("convert to grayscale"))
        my_image.convertImageToGreyScale();
    
    ImGui::SameLine();
    if (ImGui::Button("reset image"))
        my_image.resetImage();
}

void CgImageViewer::createAufgabe2Tab()
{
    ImGui::Text("Das ist der Tab für Aufgabe 2!\nadd whatever you need");
}
void CgImageViewer::createAufgabe3Tab()
{
    ImGui::Text("Das ist der Tab für Aufgabe 3!\nadd whatever you need");
}
void CgImageViewer::createAufgabe4Tab()
{
    ImGui::Text("Das ist der Tab für Aufgabe 4!\nadd whatever you need");
}
void CgImageViewer::createAufgabe5Tab()
{
    ImGui::Text("Das ist der Tab für Aufgabe 5!\nadd whatever you need");
}
