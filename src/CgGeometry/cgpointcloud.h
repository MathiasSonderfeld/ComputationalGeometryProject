#ifndef CG_POINTCLOUD_H
#define CG_POINTCLOUD_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "cgbasepointcloud.h"


class CgPointCloud : public CgBasePointCloud
{

public:
    
    // constructors ans destructor
    CgPointCloud();
    CgPointCloud(std::vector<glm::vec3>& verts);
    ~CgPointCloud();

    //inherited from CgBaseRenderableObject
    ObjectType getType() const;
    unsigned int getID() const;
    glm::vec3 getColor() const;

    //inherited from CgBasePointCloud
    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec3>& getVertexNormals() const;
    const std::vector<glm::vec3>& getVertexColors() const;
    
    // given a ray with origin and dir: get closest Point
    // to be implemented by students
    const glm::vec3 getClosestPoint (const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const;

    // for splatting two axes for the ellipse are needed for each vertex
    const std::vector<glm::vec3>& getSplatDirX() const;
    const std::vector<glm::vec3>& getSplatDirY() const;
    

protected:
    
    // store point data given by constructor
    std::vector<glm::vec3> m_vertices;
    
    // not set by constructor, assume point data only.
    // to be constructed by students during analysis of pointcloud
    std::vector<glm::vec3> m_vertex_normals;
    std::vector<glm::vec3> m_splatdir_x;
    std::vector<glm::vec3> m_splatdir_y;
    
    // vertex colors can be used to mark certain regions (e.g. nearest neighbors)
    // by a different color, to be construced by students
    std::vector<glm::vec3> m_vertex_colors;
    
    // store id and type as provided by CgBaseRenderableObject
    const ObjectType m_type;
    const unsigned int m_id;
    
    // if no material is used (lighting off) and no per vertex color available use this color for complete object
    glm::vec3 m_color;

private:
    
  
    
    
};

// inline for simple getter methods
inline ObjectType  CgPointCloud::getType() const {return m_type;}
inline unsigned int CgPointCloud::getID() const {return m_id;}
inline glm::vec3 CgPointCloud::getColor() const {return m_color;}

#endif // CGEXAMPLETRIANGLE_H
