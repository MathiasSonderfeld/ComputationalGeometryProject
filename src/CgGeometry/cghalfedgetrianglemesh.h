#ifndef CGHALFEDGETRIANGLEMESH_H
#define CGHALFEDGETRIANGLEMESH_H

#include "cgbasehalfedgetrianglemesh.h"
#include "cghalfedgeprimitives.h"

#include <vector>
#include <glm/glm.hpp>
#include <string>

class CgHalfEdgeTriangleMesh : public CgBaseHalfEdgeTriangleMesh {
public:
    CgHalfEdgeTriangleMesh();
    CgHalfEdgeTriangleMesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norm, std::vector<unsigned int>& idx);

    ~CgHalfEdgeTriangleMesh();

    //inherited from CgBaseRenderableObject
    ObjectType getType() const;
    unsigned int getID() const;
    glm::vec3 getColor() const;


    //inherited from CgBaseHalfEdgeTriangleMesh

    const std::vector<CgBaseHeFace*>& getFaces() const;

    //own stuff
    const glm::vec3 getCenter() const;

private:
    std::vector<CgBaseHeFace*> m_faces;
    std::vector<CgBaseHeVert*> m_verts;
    std::vector<CgBaseHeEdge*> m_edges;


    const ObjectType m_type;
    const unsigned int m_id;

    // if no material is used
    glm::vec3 m_color{};
};


inline ObjectType CgHalfEdgeTriangleMesh::getType() const {
    return m_type;
}

inline unsigned int CgHalfEdgeTriangleMesh::getID() const {
    return m_id;
}

inline glm::vec3 CgHalfEdgeTriangleMesh::getColor() const {
    return m_color;
}


#endif // CGHALFEDGETRIANGLEMESH_H
