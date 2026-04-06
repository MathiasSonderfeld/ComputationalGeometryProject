#ifndef CG_HALF_EDGE_TRIANGLE_MESH_H
#define CG_HALF_EDGE_TRIANGLE_MESH_H

#include <unordered_map>

#include "cgbasehalfedgetrianglemesh.h"
#include "cghalfedgeprimitives.h"

#include <vector>
#include <glm/glm.hpp>


struct PairHash {
    size_t operator()(const std::pair<int,int>& p) const {
        const size_t h1 = std::hash<int>()(p.first);
        const size_t h2 = std::hash<int>()(p.second);
        return h1 ^ h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
    }
};

class CgHalfEdgeTriangleMesh : public CgBaseHalfEdgeTriangleMesh
{
public:
    CgHalfEdgeTriangleMesh(const std::vector<glm::vec3>& vertices,std::vector<glm::vec3>& normals, const std::vector<unsigned int>& face_indices);

    ~CgHalfEdgeTriangleMesh() override;

    //inherited from CgBaseRenderableObject
    ObjectType getType() const override;
    unsigned int getID() const override;
    glm::vec3 getColor() const override;

    
    //inherited from CgBaseHalfEdgeTriangleMesh

    const std::vector<CgBaseHeFace*>& getFaces() const override;

    //own stuff
    const glm::vec3 getCenter() const;


private:
    CgHeVert* createVertex(int index, const std::vector<glm::vec3>& vertices, std::unordered_map<int, CgHeVert*>& halfEdgeVertices);
    CgHeEdge* createEdge(const std::pair<int, int>& edge_vertices, std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash>& halfEdges, std::unordered_map<int, CgHeVert*>& halfEdgeVertices);
    std::vector<CgBaseHeFace*> m_faces;
    std::vector<CgBaseHeVert*> m_vertices;
    std::vector<CgBaseHeEdge*> m_edges;


    const ObjectType m_type;
    const unsigned int m_id;

    // if no material is used
    glm::vec3 m_color{1,0,0};
    
};


inline ObjectType  CgHalfEdgeTriangleMesh::getType() const {return m_type;}
inline unsigned int CgHalfEdgeTriangleMesh::getID() const {return m_id;}
inline glm::vec3 CgHalfEdgeTriangleMesh::getColor() const {return m_color;}


#endif // CG_HALF_EDGE_TRIANGLE_MESH_H
