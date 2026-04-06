#include "cghalfedgetrianglemesh.h"

#include <iostream>
#include <ostream>

extern int getUniqueId();

CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, const std::vector<unsigned int>& face_indices) :
    m_type(HalfEdgeTriangleMesh),
    m_id(getUniqueId())
{

    std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash> halfEdges;
    std::unordered_map<int, CgHeVert*> halfEdgeVertices;

    for (int i = 0; i < face_indices.size(); i+=3)
    {
        int v0i = face_indices[i];
        int v1i = face_indices[i+1];
        int v2i = face_indices[i+2];
        createVertex(v0i, vertices, halfEdgeVertices);
        createVertex(v1i, vertices, halfEdgeVertices);
        createVertex(v2i, vertices, halfEdgeVertices);

        CgHeFace* face = new CgHeFace();
        face->setIndex1(v0i);
        face->setIndex2(v1i);
        face->setIndex3(v2i);

        CgHeEdge* edgeOne = createEdge({v0i, v1i}, halfEdges, halfEdgeVertices);
        CgHeEdge* edgeTwo = createEdge({v1i, v2i}, halfEdges, halfEdgeVertices);
        CgHeEdge* edgeThree = createEdge({v2i, v0i}, halfEdges, halfEdgeVertices);

        edgeOne->m_face = face;
        edgeTwo->m_face = face;
        edgeThree->m_face = face;
        face->m_edge = edgeOne;

        edgeOne->m_next = edgeTwo;
        edgeTwo->m_next = edgeThree;
        edgeThree->m_next = edgeOne;

        this->m_faces.push_back(face);
    }
}

CgHeVert* CgHalfEdgeTriangleMesh::createVertex(const int index, const std::vector<glm::vec3>& vertices, std::unordered_map<int, CgHeVert*>& halfEdgeVertices)
{
    if (halfEdgeVertices[index] == nullptr)
    {
        halfEdgeVertices[index] = new CgHeVert();
        halfEdgeVertices[index]->setIndex(index);
        halfEdgeVertices[index]->m_color = glm::vec3(1, 0, 0);
        halfEdgeVertices[index]->m_position = vertices[index];
        this->m_vertices.push_back(halfEdgeVertices[index]);
    }
    return halfEdgeVertices[index];
}

CgHeEdge* CgHalfEdgeTriangleMesh::createEdge(const std::pair<int, int>& edge_vertices, std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash>& halfEdges, std::unordered_map<int, CgHeVert*>& halfEdgeVertices)
{
    CgHeEdge* edge = new CgHeEdge();
    edge->setStartIndex(edge_vertices.first);
    edge->setEndIndex(edge_vertices.second);

    halfEdges[edge_vertices] = edge;

    auto pair_edge_it = halfEdges.find({edge_vertices.second, edge_vertices.first});
    if (pair_edge_it != nullptr)
    {
        edge->m_pair = pair_edge_it->second;
        pair_edge_it->second->m_pair = edge;
    }
    edge->m_vert = halfEdgeVertices[edge_vertices.first];
    edge->m_vert->m_edge = edge;
    this->m_edges.push_back(edge);
    return edge;
}

CgHalfEdgeTriangleMesh::~CgHalfEdgeTriangleMesh()
{
    // that's not enough, have to kill Objects as well´
    for (const auto m_face : m_faces)
    {
        delete m_face;
    }
    for (const auto m_edge : m_edges) {
        delete m_edge;
    }
    for (const auto m_vert : m_vertices) {
        delete m_vert;
    }

    m_faces.clear();
    m_edges.clear();
    m_vertices.clear();
}

const std::vector<CgBaseHeFace*>& CgHalfEdgeTriangleMesh::getFaces() const
{
    return m_faces;
}

const glm::vec3 CgHalfEdgeTriangleMesh::getCenter() const
{
    glm::vec3 center(0.);
    for (const auto m_vert : m_vertices)
    {
        center += m_vert->position();
    }
    center /= static_cast<double>(m_vertices.size());
    return center;
}
