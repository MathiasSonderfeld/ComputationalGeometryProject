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

        std::pair<CgHeEdge*, CgHeEdge*> pair = createEdge({v0i, v1i}, halfEdges, halfEdgeVertices);
        CgHeEdge* edgeOneForward = pair.first;
        CgHeEdge* edgeOneBackward = pair.second;

        pair = createEdge({v1i, v2i}, halfEdges, halfEdgeVertices);
        CgHeEdge* edgeTwoForward = pair.first;
        CgHeEdge* edgeTwoBackward = pair.second;


        pair = createEdge({v2i, v0i}, halfEdges, halfEdgeVertices);
        CgHeEdge* edgeThreeForward = pair.first;
        CgHeEdge* edgeThreeBackward = pair.second;

        edgeOneForward->m_face = face;
        edgeOneBackward->m_face = face;

        edgeOneForward->m_next = edgeTwoForward;
        edgeTwoForward->m_next = edgeThreeForward;
        edgeThreeForward->m_next = edgeOneForward;

        edgeOneBackward->m_next = edgeThreeBackward;
        edgeThreeBackward->m_next = edgeTwoBackward;
        edgeTwoBackward->m_next = edgeOneBackward;

        face->m_edge = edgeOneForward;
        glm::vec3& v0 = normals[v0i];
        glm::vec3& v1 = normals[v1i];
        glm::vec3& v2 = normals[v2i];
        face->m_normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        this->m_faces.push_back(face);
    }
    this->integrityCheck();
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

std::pair<CgHeEdge*, CgHeEdge*> CgHalfEdgeTriangleMesh::createEdge(const std::pair<int, int>& edge_vertices, std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash>& halfEdges, std::unordered_map<int, CgHeVert*>& halfEdgeVertices)
{
    if (halfEdges[edge_vertices] != nullptr)
    {
        return {halfEdges[edge_vertices], halfEdges[edge_vertices]->m_pair};
    }
    CgHeEdge* edge = new CgHeEdge();
    CgHeEdge* reversed_edge = new CgHeEdge();

    edge->setStartIndex(edge_vertices.first);
    edge->setEndIndex(edge_vertices.second);

    reversed_edge->setStartIndex(edge_vertices.second);
    reversed_edge->setEndIndex(edge_vertices.first);

    edge->m_pair = reversed_edge;
    reversed_edge->m_pair = edge;
    halfEdges[edge_vertices] = edge;
    halfEdges[{edge_vertices.second, edge_vertices.first}] = reversed_edge;

    edge->m_vert = halfEdgeVertices[edge_vertices.first];
    edge->m_vert->m_edge = edge;
    reversed_edge->m_vert = halfEdgeVertices[edge_vertices.second];
    reversed_edge->m_vert->m_edge = reversed_edge;

    this->m_edges.push_back(edge);
    this->m_edges.push_back(reversed_edge);
    return {edge, reversed_edge};
}

void CgHalfEdgeTriangleMesh::integrityCheck()
{
    for (int i = 0; i < this->m_faces.size(); ++i)
    {
        const CgBaseHeFace* face = this->m_faces[i];
        if (face->normal() == glm::vec3{})
        {
            std::cout << "face " << i << " normal invalid" << std::endl;
            return;
        }

        //check loop
        const CgBaseHeEdge* edge = face->edge();
        if (edge == nullptr)
        {
            std::cout << "face " << i << " edge null" << std::endl;
            return;
        }
        const CgBaseHeEdge* next_edge = edge->next();
        if (next_edge == nullptr)
        {
            std::cout << "face " << i << " next edge null" << std::endl;
            return;
        }
        const CgBaseHeEdge* next_next_edge = next_edge->next();
        if (next_next_edge == nullptr || next_next_edge->next() != edge)
        {
            std::cout << "face " << i << " edge loop invalid" << std::endl;
        }

        //check reverse loop
        const CgBaseHeEdge* pair_edge = edge->pair();
        if (pair_edge == nullptr)
        {
            std::cout << "face " << i << " edge null" << std::endl;
            return;
        }
        const CgBaseHeEdge* next_pair_edge = pair_edge->next();
        if (next_pair_edge == nullptr)
        {
            std::cout << "face " << i << " next edge null" << std::endl;
            return;
        }
        const CgBaseHeEdge* next_next_pair_edge = next_pair_edge->next();
        if (next_next_pair_edge == nullptr || next_next_pair_edge->next() != pair_edge)
        {
            std::cout << "face " << i << " reverse edge loop invalid" << std::endl;
        }
    }
}


CgHalfEdgeTriangleMesh::~CgHalfEdgeTriangleMesh()
{
    // that's not enough, have to kill Objects as well´
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
