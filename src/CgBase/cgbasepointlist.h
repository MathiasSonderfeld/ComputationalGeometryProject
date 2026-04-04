#ifndef CG_BASE_POINT_LIST_H
#define CG_BASE_POINT_LIST_H


#include <vector>
#include <glm/glm.hpp>
#include "cgbaserenderableobject.h"

class CgBasePointList : public CgBaseRenderableObject
{
public:
    ~ CgBasePointList() override = default;

    virtual const std::vector<glm::vec3>& getVertices() const =0;
    virtual const int getLineWidth() const =0;
    virtual const LineStyle getLineStyle() const =0;
    virtual void setLineStyle(LineStyle newVal) =0;
};

#endif
