#ifndef CG_BASE_TRIANGLE_FAN_H
#define CG_BASE_TRIANGLE_FAN_H


#include <vector>
#include <glm/glm.hpp>
#include "cgbaserenderableobject.h"

class CgBaseTriangleFan : public CgBaseRenderableObject
{
public:
    ~CgBaseTriangleFan() override = default;
    virtual const std::vector<glm::vec3>& getVertices() const =0;
};

#endif // CG_BASE_TRIANGLE_FAN_H
