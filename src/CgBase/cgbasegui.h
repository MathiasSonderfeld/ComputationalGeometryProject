#ifndef CGBASEGUI_H
#define CGBASEGUI_H


class CgBaseGui
{
    
public:

    virtual void showGUI(int windowsize_x,int windowsize_y)=0;
    virtual void initGUI()=0;
    virtual void renderObjects()=0;
};
#endif
