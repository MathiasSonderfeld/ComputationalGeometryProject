#ifndef CG_BASE_GUI_H
#define CG_BASE_GUI_H


class CgBaseGui
{
public:
    virtual ~CgBaseGui() = default;

    virtual void showGUI(int windowSizeX, int windowSizeY) =0;
    virtual void initGUI() =0;
    virtual void renderObjects() =0;
};
#endif
