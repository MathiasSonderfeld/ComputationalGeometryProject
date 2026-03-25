#ifndef CGBASE_POINTLIST_H
#define CGBASE_POINTLIST_H


#include <vector>
#include <glm/glm.hpp>
#include "cgbaserenderableobject.h"

class CgBasePointList : public CgBaseRenderableObject
{
public:

    virtual ~ CgBasePointList(){};


    virtual const std::vector<glm::vec3>& getVertices()     const =0;
    virtual const int getLineWidth() const =0;
    virtual const LineStyle getLineStyle() const=0;
    virtual void setLineStyle(LineStyle newVal)=0;
};



#endif
