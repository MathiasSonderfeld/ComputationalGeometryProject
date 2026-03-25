#ifndef CG_SPHERE_H
#define CG_SPHERE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "cgtrianglemesh.h"


class CgSphere : public CgTriangleMesh
{

public:
    CgSphere();
    CgSphere(glm::vec3 center, double radius);
    
    ~CgSphere();
    
    glm::vec3 getCenter();
    void setCenter(glm::vec3 c);

    //overload triangular calculation
    bool hasIntersection (const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const;
    
private:
      
    void createTrianglesForSphere(int sectors, int rings);
    
    double m_radius;
    glm::vec3 m_center;
   
    
};



#endif // CGEXAMPLETRIANGLE_H

