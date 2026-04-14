#include "cghalfedgetrianglemesh.h"

#include <iostream>
#include <ostream>

extern int getUniqueId();

CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh(
    const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, const std::vector<unsigned int>& face_indices) : m_type(HalfEdgeTriangleMesh),
    m_id(getUniqueId()) {
    std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash> halfEdges;
    std::unordered_map<int, CgHeVert*> halfEdgeVertices;

    for (int i = 0; i < face_indices.size(); i += 3) {
        int v0i = face_indices[i];
        int v1i = face_indices[i + 1];
        int v2i = face_indices[i + 2];
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

        edgeOne->setFace(face);
        edgeTwo->setFace(face);
        edgeThree->setFace(face);
        face->setEdge(edgeOne);

        edgeOne->setNext(edgeTwo);
        edgeTwo->setNext(edgeThree);
        edgeThree->setNext(edgeOne);

        this->m_faces.push_back(face);
    }
    this->calculateNormals();
}

CgHeVert* CgHalfEdgeTriangleMesh::createVertex(const int index, const std::vector<glm::vec3>& vertices, std::unordered_map<int, CgHeVert*>& halfEdgeVertices) {
    if (halfEdgeVertices[index] == nullptr) {
        halfEdgeVertices[index] = new CgHeVert();
        halfEdgeVertices[index]->setIndex(index);
        halfEdgeVertices[index]->setColor(glm::vec3(1, 0, 0));
        halfEdgeVertices[index]->setPosition(vertices[index]);
        this->m_vertices.push_back(halfEdgeVertices[index]);
    }
    return halfEdgeVertices[index];
}

CgHeEdge* CgHalfEdgeTriangleMesh::createEdge(
    const std::pair<int, int>& edge_vertices, std::unordered_map<std::pair<int, int>, CgHeEdge*, PairHash>& halfEdges, std::unordered_map<int, CgHeVert*>& halfEdgeVertices) {
    CgHeEdge* edge = new CgHeEdge();
    edge->setStartIndex(edge_vertices.first);
    edge->setEndIndex(edge_vertices.second);

    halfEdges[edge_vertices] = edge;

    auto pair_edge_it = halfEdges.find({edge_vertices.second, edge_vertices.first});
    if (pair_edge_it != halfEdges.end()) {
        edge->setPair(pair_edge_it->second);
        pair_edge_it->second->setPair(edge);
    }
    edge->setVert(halfEdgeVertices[edge_vertices.first]);
    halfEdgeVertices[edge_vertices.first]->setEdge(edge);
    this->m_edges.push_back(edge);
    return edge;
}

void CgHalfEdgeTriangleMesh::calculateNormals() const {
    for (const auto m_face : m_faces) {
        glm::vec3 v1 = m_face->edge()->vert()->position();
        glm::vec3 v2 = m_face->edge()->next()->vert()->position();
        glm::vec3 v3 = m_face->edge()->next()->next()->vert()->position();
        const glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        m_face->setNormal(normal);
    }

    for (CgBaseHeVert* vertex : m_vertices) {
        const glm::vec3 normal = glm::normalize(calculateVertexNormal(vertex));
        vertex->setNormal(normal);
    }
}

glm::vec3 CgHalfEdgeTriangleMesh::calculateVertexNormal(const CgBaseHeVert* vertex) {
    glm::vec3 normal(0.0f);
    const CgBaseHeEdge* start = vertex->edge();

    if (start == nullptr)
        return normal;

    const CgBaseHeEdge* edge = start;

    do {
        if (edge->face() != nullptr)
            normal += edge->face()->normal();

        if (edge->pair() == nullptr)
            break; // Boundary

        edge = edge->pair()->next();
    }
    while (edge != start);

    // only true if we didnt hit a boundary
    if (edge != start) {
        edge = start;
        do {
            edge = edge->next()->next();
            if (edge->pair() == nullptr)
                break;

            edge = edge->pair();
            if (edge->face() != nullptr)
                normal += edge->face()->normal();
        }
        while (edge != start);
    }
    return normal;
}

CgHalfEdgeTriangleMesh::~CgHalfEdgeTriangleMesh() {
    // that's not enough, have to kill Objects as well´
    for (const auto m_face : m_faces) {
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

const std::vector<CgBaseHeFace*>& CgHalfEdgeTriangleMesh::getFaces() const {
    return m_faces;
}

const std::vector<CgBaseHeVert*>& CgHalfEdgeTriangleMesh::getVertices() const {
    return m_vertices;
}

glm::vec3 CgHalfEdgeTriangleMesh::getCenter() const {
    glm::vec3 center(0.);
    for (const auto m_vert : m_vertices) {
        center += m_vert->position();
    }
    center /= static_cast<double>(m_vertices.size());
    return center;
}


void CgHalfEdgeTriangleMesh::subdivide() {
    const int edgeCount = m_edges.size();
    for (int i = 0; i < edgeCount; i++) {
        CgHeEdge* edge = m_edges[i];
        const CgHeVert* start = edge->getVert();
        //check if pair exists and was already split - if the next edge also refers to start as pair
        if (edge->getPair() != nullptr && edge->getPair()->getNext()->getPair() == edge) {
            CgHeEdge* pair = edge->getPair();
            CgHeEdge* pairSecond = pair->getNext();
            CgHeVert* newVert = edge->getPair()->getNext()->getVert();
            CgHeEdge* newEdge = new CgHeEdge();
            newEdge->setFace(edge->getFace());
            newEdge->setPair(pairSecond);
            newEdge->setVert(newVert);
            pairSecond->setPair(newEdge);
            newEdge->setNext(edge->getNext());
            edge->setNext(newEdge);
            m_edges.push_back(newEdge);
        }
        else {
            const CgHeEdge* next = edge->getNext();
            const glm::vec3 newVertPos = (next->getVert()->position() + start->position()) / 2.0f;
            CgHeVert* newVert = new CgHeVert();
            newVert->setPosition(newVertPos);
            newVert->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
            CgHeEdge* newEdge = new CgHeEdge();
            newEdge->setPair(edge->getPair());
            newEdge->setVert(newVert);
            newVert->setEdge(newEdge);
            newEdge->setNext(edge->getNext());
            edge->setNext(newEdge);
            m_vertices.push_back(newVert);
            m_edges.push_back(newEdge);
        }
    }

    const int faceCount = m_faces.size();
    for (int i = 0; i < faceCount; i++) {
        CgHeFace* startFace = dynamic_cast<CgHeFace*>(m_faces[i]);
        CgHeEdge* edgeOneFirst = startFace->getEdge();
        CgHeEdge* edgeOneSecond = edgeOneFirst->getNext();
        CgHeEdge* edgeTwoFirst = edgeOneSecond->getNext();
        CgHeEdge* edgeTwoSecond = edgeTwoFirst->getNext();
        CgHeEdge* edgeThreeFirst = edgeTwoSecond->getNext();
        CgHeEdge* edgeThreeSecond = edgeThreeFirst->getNext();



        CgHeVert* v12 = edgeOneSecond->getVert();
        CgHeVert* v23 = edgeTwoSecond->getVert();
        CgHeVert* v31 = edgeThreeSecond->getVert();

        CgHeEdge* v12v23 = new CgHeEdge();
        v12v23->setVert(v12);
        CgHeEdge* v23v12 = new CgHeEdge();
        v23v12->setVert(v23);
        v12v23->setPair(v23v12);
        v23v12->setPair(v12v23);

        CgHeEdge* v23v31 = new CgHeEdge();
        v23v31->setVert(v23);
        CgHeEdge* v31v23 = new CgHeEdge();
        v31v23->setVert(v31);
        v23v31->setPair(v31v23);
        v31v23->setPair(v23v31);

        CgHeEdge* v31v12 = new CgHeEdge();
        v31v12->setVert(v31);
        CgHeEdge* v12v31 = new CgHeEdge();
        v12v31->setVert(v12);
        v31v12->setPair(v12v31);
        v12v31->setPair(v31v12);

        CgHeFace* newFaceOne = new CgHeFace();
        CgHeFace* newFaceTwo = new CgHeFace();
        CgHeFace* newFaceThree = new CgHeFace();

        edgeOneFirst->setNext(v12v31);
        v12v31->setNext(edgeThreeSecond);
        edgeThreeSecond->setNext(edgeOneFirst);
        edgeOneFirst->setFace(startFace);
        v12v31->setFace(startFace);
        edgeThreeSecond->setFace(startFace);

        newFaceOne->setEdge(edgeOneSecond);
        edgeOneSecond->setNext(edgeTwoFirst);
        edgeTwoFirst->setNext(v23v12);
        v23v12->setNext(edgeOneSecond);
        edgeOneSecond->setFace(newFaceOne);
        v23v12->setFace(newFaceOne);
        edgeTwoFirst->setFace(newFaceOne);

        newFaceTwo->setEdge(edgeTwoSecond);
        edgeTwoSecond->setNext(edgeThreeFirst);
        edgeThreeFirst->setNext(v31v23);
        v31v23->setNext(edgeTwoSecond);
        edgeTwoSecond->setFace(newFaceTwo);
        v31v23->setFace(newFaceTwo);
        edgeThreeFirst->setFace(newFaceTwo);

        newFaceThree->setEdge(v31v12);
        v31v12->setNext(v12v23);
        v12v23->setNext(v23v31);
        v23v31->setNext(v31v12);
        v31v12->setFace(newFaceThree);
        v12v23->setFace(newFaceThree);
        v23v31->setFace(newFaceThree);

        this->m_edges.push_back(v12v23);
        this->m_edges.push_back(v23v12);
        this->m_edges.push_back(v23v31);
        this->m_edges.push_back(v31v23);
        this->m_edges.push_back(v31v12);
        this->m_edges.push_back(v12v31);

        this->m_faces.push_back(newFaceOne);
        this->m_faces.push_back(newFaceTwo);
        this->m_faces.push_back(newFaceThree);
    }

    this->calculateNormals();
}

