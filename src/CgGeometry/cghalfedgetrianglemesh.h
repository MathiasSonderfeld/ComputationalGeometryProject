#ifndef CG_HALF_EDGE_TRIANGLE_MESH_H
#define CG_HALF_EDGE_TRIANGLE_MESH_H

#include <unordered_map>

#include "cgbasehalfedgetrianglemesh.h"
#include "cghalfedgeprimitives.h"
#include "../CgMath/cgmlsfit.h"

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
    const std::vector<CgBaseHeVert*>& getVertices() const override;

    //own stuff
    glm::vec3 getCenter() const;

    void consistencyCheck() const;
    void subdivide();

    // index of the vertex whose position is closest to the picking line, or -1
    int getClosestVertex(const glm::vec3& origin, const glm::vec3& dir, double maxDistance) const;

    // vertices reachable from `index` over at most two edges (the 2-ring)
    std::vector<int> twoRingNeighbours(int index) const;

    // Moving Least Squares fit at vertex `index` over its 2-ring neighbourhood
    // (the vertex itself is included as a sample)
    CgMlsSurface mlsSurfaceAt(int index, int degree) const;

    // move a single vertex onto a position and refresh the vertex normals
    void setVertexPosition(int index, const glm::vec3& position);

    // MLS-smooth every vertex at once (double-buffered: old positions feed all
    // fits, new positions committed together, then normals recomputed)
    void smoothAllMLS(int degree);

    static glm::vec3 calculateNewVerticePosition(CgHeVert *vertex);

    static float calculateBeta(int size);

private:
    CgHeVert* createVertex(int index, const std::vector<glm::vec3>& vertices, std::unordered_map<int, CgHeVert*>& halfEdgeVertices);
    CgHeEdge* createEdge(const std::pair<int, int>& edge_vertices, std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash>& halfEdges, std::unordered_map<int, CgHeVert*>& halfEdgeVertices);
    void calculateNormals() const;
    static glm::vec3 calculateVertexNormal(const CgBaseHeVert* vertex);

    // vertex adjacency derived from the triangle faces (boundary-proof)
    std::vector<std::vector<int>> buildVertexAdjacency() const;

    std::vector<CgBaseHeFace*> m_faces;
    std::vector<CgBaseHeVert*> m_vertices;
    std::vector<CgHeEdge*> m_edges;


    const ObjectType m_type;
    const unsigned int m_id;

    // if no material is used
    glm::vec3 m_color{1,0,0};
    
};


inline ObjectType  CgHalfEdgeTriangleMesh::getType() const {return m_type;}
inline unsigned int CgHalfEdgeTriangleMesh::getID() const {return m_id;}
inline glm::vec3 CgHalfEdgeTriangleMesh::getColor() const {return m_color;}


#endif // CG_HALF_EDGE_TRIANGLE_MESH_H
