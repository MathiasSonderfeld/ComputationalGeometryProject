#ifndef CG_TRIANGLEMESH_H
#define CG_TRIANGLEMESH_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "cgbasetrianglemesh.h"


class CgTriangleMesh : public CgBaseTriangleMesh
{

public:
    CgTriangleMesh();
    CgTriangleMesh(std::vector<glm::vec3>& vertices,std::vector<glm::vec3>& norm, const std::vector<unsigned int>& idx);
    
    ~CgTriangleMesh();

    //inherited from CgBaseRenderableObject
    ObjectType getType() const;
    unsigned int getID() const;
    glm::vec3 getColor() const;

    //inherited from CgBaseTriangleMesh
    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec3>& getVertexNormals() const;
    const std::vector<glm::vec3>& getVertexColors() const;
    const std::vector<glm::vec2>& getVertexTexCoords() const;

    const std::vector<unsigned int>& getTriangleIndices() const;

    const std::vector<glm::vec3>& getFaceNormals() const;
    const std::vector<glm::vec3>& getFaceColors() const;

    // given a ray with origin and dir: check if an intersection exists
    bool hasIntersection (const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const;

protected:

  
    
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_vertex_normals;
    std::vector<glm::vec3> m_vertex_colors;
    std::vector<glm::vec2> m_tex_coords;

    std::vector<unsigned int>  m_triangle_indices;

    std::vector<glm::vec3> m_face_normals;
    std::vector<glm::vec3> m_face_colors;

    const ObjectType m_type;
    const unsigned int m_id;
    
    // if no material is used
    glm::vec3 m_color;

private:
    
  
    
    
};


inline ObjectType  CgTriangleMesh::getType() const {return m_type;}
inline unsigned int CgTriangleMesh::getID() const {return m_id;}
inline glm::vec3 CgTriangleMesh::getColor() const {return m_color;}

#endif // CGEXAMPLETRIANGLE_H
