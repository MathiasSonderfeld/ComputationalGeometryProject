#ifndef CGBASE_POINTCLOUD_H
#define CGBASE_POINTCLOUD_H


#include <vector>
#include <glm/glm.hpp>
#include "cgbaserenderableobject.h"

class CgBasePointCloud : public CgBaseRenderableObject
{
public:

    virtual ~ CgBasePointCloud(){};


    virtual const std::vector<glm::vec3>& getVertices()     const =0;
    virtual const std::vector<glm::vec3>& getVertexNormals() const =0;
    virtual const std::vector<glm::vec3>& getVertexColors() const =0;

    // given a ray with origin and dir: get closest Point
    virtual const glm::vec3 getClosestPoint (const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const=0;

    
    // for splatting two axes for the ellipse are needed for each vertex
    virtual const std::vector<glm::vec3>& getSplatDirX() const =0;
    virtual const std::vector<glm::vec3>& getSplatDirY() const =0;
    
    
};



#endif 
