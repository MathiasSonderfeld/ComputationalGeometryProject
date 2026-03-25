#ifndef CGBASETRIANGLEFAN_H
#define CGBASETRIANGLEFAN_H


#include <vector>
#include <glm/glm.hpp>
#include "cgbaserenderableobject.h"

class CgBaseTriangleFan : public CgBaseRenderableObject
{
public:

    virtual ~CgBaseTriangleFan(){};
    virtual const std::vector<glm::vec3>& getVertices()     const =0;
};



#endif // CGBASETRIANGLEFAN_H
