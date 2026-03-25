#include "cgsphere.h"
#include <glm/ext.hpp>
#include <iostream>

extern int getUniqueId();

CgSphere::CgSphere():CgTriangleMesh()
{
    m_center=glm::vec3(0.0,0.0,0.0);
    m_radius=0.0f;
    
}

CgSphere::CgSphere(glm::vec3 center, double radius):CgTriangleMesh()
{
    m_center=center;
    m_radius=radius;

    createTrianglesForSphere(16, 16);
}


CgSphere::~CgSphere()
{
    
}

glm::vec3 CgSphere::getCenter()
{
    return m_center;
}
void CgSphere::setCenter(glm::vec3 c)
{
    m_center=c;
    createTrianglesForSphere(16, 16);
}

bool CgSphere::hasIntersection (const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const
    {
        glm::vec3 m = origin-m_center;
        float temp = glm::dot(m,m);
        float c = temp-(m_radius*m_radius);
        //if there is at least one real root, there must be an intersection
        if(c<=0.0f) return true;
        float b = glm::dot(dir,m);

        // exit if outside and pointing away
        if(b>=0.0f) return false;
        float disc = b*b-c;

        // neagtive discriminant means missing
        if(disc<0.0f) return false;

        std::cout <<  "intersction found in object: #" << this->getID() << std::endl;
        return true;
    }





void CgSphere::createTrianglesForSphere(int sectors, int rings)
{
    
    float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);
    int r, s;
    
    m_vertices.resize(rings * sectors);
    m_vertex_normals.resize(rings * sectors);
   
    
    std::vector<glm::vec3>::iterator v = m_vertices.begin();
    std::vector<glm::vec3>::iterator n = m_vertex_normals.begin();
    
    for(r = 0; r < rings; r++)
    {
        for(s = 0; s < sectors; s++)
        {
            float const y = sin( -M_PI_2 + M_PI * r * R );
            float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
            float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );
            
            *v=glm::vec3(glm::vec3(x*m_radius,y*m_radius,z*m_radius)+m_center);
            v++;
            
            *n=glm::vec3(x,y,z);
            n++;
        }
    }

  
    m_triangle_indices.resize(rings * sectors * 6);
    
    std::vector<unsigned int>::iterator i = m_triangle_indices.begin();
    for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
        *i++ = r * sectors + s;
        *i++ = (r+1) * sectors + (s+1);
        *i++ = r * sectors + (s+1);
        *i++ = r * sectors + s;
        *i++ = (r+1) * sectors + s;
        *i++ = (r+1) * sectors + (s+1);
       
    }
  
    
}

