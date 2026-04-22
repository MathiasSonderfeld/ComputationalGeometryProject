#include "cghalfedgetrianglemesh.h"

#include <iostream>
#include <ostream>
#include <set>

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
    this->consistencyCheck();
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

void CgHalfEdgeTriangleMesh::consistencyCheck() {
    std::cout << "Consistency check" << std::endl;
    int missingPairs = 0;
    for (const auto & edge : this->m_edges) {
        if (edge != edge->getNext()->getNext()->getNext()) {
            std::cerr << "Inconsistent face loop" << std::endl;
        }
        if (edge->getPair() == nullptr) missingPairs++;
        if (edge->getPair() != nullptr) {
            if (edge->getPair()->getPair() != edge) {
                std::cerr << "Inconsistent edge pair" << std::endl;
            }
            if (edge->getPair() == edge) {
                std::cerr << "Inconsistent edge pair reference to self" << std::endl;
            }
            if (edge->getPair()->getVert() == edge->getVert()) {
                std::cerr << "Inconsistent edge pair vertices" << std::endl;
            }
            if (edge->getFace() == edge->getPair()->getFace()) {
                std::cerr << "pair face inconsistency" << std::endl;
            }
            if (edge->getVert() != edge->getPair()->getNext()->getVert()) {
                std::cerr << "pair next vert inconsistent with edge vert" << std::endl;
            }
            if (edge->getNext()->getVert() != edge->getPair()->getVert()) {
                std::cerr << "pair vert inconsistent with edge next vert" << std::endl;
            }
        }
    }
    std::cout << "Edges without pair: " << missingPairs << " / " << m_edges.size() << std::endl;

    for (const auto & vertex : this->m_vertices) {
        if (vertex->edge() == nullptr) {
            std::cerr << "undefined vertex edge reference" << std::endl;
        }
        if (vertex->edge() != nullptr && vertex->edge()->vert() != vertex) {
            std::cerr << "Inconsistent vertex edge reference" << std::endl;
        }
    }
    for (const auto & face : this->m_faces) {
        if (face->edge() == nullptr) {
            std::cerr << "undefined face edge reference" << std::endl;
        }
        if (face->edge() != nullptr && face->edge()->face() != face) {
            std::cerr << "Inconsistent face edge reference" << std::endl;
        }
    }

    int V = m_vertices.size();
    int E = m_edges.size() / 2;
    int F = m_faces.size();
    std::cout << "V=" << V << " E=" << E << " F=" << F
              << " => Euler: " << (V - E + F) << " (erwartet: 2)" << std::endl;

    // Für Dreiecksnetze gilt immer: 3F = 2E (innen), also m_edges.size() == 3 * m_faces.size()
    if (m_edges.size() != 3 * m_faces.size()) {
        std::cerr << "Edge/Face count mismatch: " << m_edges.size()
                  << " edges, " << m_faces.size() << " faces" << std::endl;
    }


}


void CgHalfEdgeTriangleMesh::subdivide() {
    const int edgeCount = m_edges.size();
    const int vertexCount = m_vertices.size();
    std::set<CgHeEdge*> set;

    for (int i = 0; i < edgeCount; i++) {
        CgHeEdge* current = m_edges[i];
        if (set.find(current) != set.end()) {
            continue;
        }
        const CgHeVert* start = current->getVert();
        CgHeEdge* next = current->getNext();
        glm::vec3 v0 = start->position();
        glm::vec3 v1 = next->getVert()->position();
        glm::vec3 newVertPos;
        if (current->getPair() == nullptr) {
            newVertPos = (v0 + v1) / 2.0f;
        }
        else {
            CgHeEdge* next_next = next->getNext();
            glm::vec3 v2 = next_next->getVert()->position();
            CgHeEdge* pair_next_next = current->getPair()->getNext()->getNext();
            glm::vec3 v3 = pair_next_next->getVert()->position();
            glm::vec3 outer = v2+v3;
            outer *= 1.0f/8.0f;
            glm::vec3 inner = v0+v1;
            inner *= 3.0f/8.0f;
            newVertPos = inner + outer;
        }

        CgHeVert* newVert = new CgHeVert();
        newVert->setPosition(newVertPos);
        newVert->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
        CgHeEdge* newEdge = new CgHeEdge();
        newEdge->setVert(newVert);
        newVert->setEdge(newEdge);
        newEdge->setNext(next);
        current->setNext(newEdge);
        set.insert(current);
        m_vertices.push_back(newVert);
        m_edges.push_back(newEdge);
        if (current->getPair() != nullptr) {
            CgHeEdge* pair = current->getPair();
            CgHeEdge* pairNext = pair->getNext();
            CgHeEdge* newPairEdge = new CgHeEdge();
            newPairEdge->setVert(newVert);
            newPairEdge->setNext(pairNext);
            pair->setNext(newPairEdge);

            newEdge->setPair(pair);
            pair->setPair(newEdge);
            current->setPair(newPairEdge);
            newPairEdge->setPair(current);
            set.insert(pair);
            m_edges.push_back(newPairEdge);
        }
    }

    const int faceCount = m_faces.size();
    for (int i = 0; i < faceCount; i++) {
        CgHeFace* startFace = dynamic_cast<CgHeFace*>(m_faces[i]);
        CgHeEdge* AM1 = startFace->getEdge();
        CgHeEdge* M1B = AM1->getNext();
        CgHeEdge* BM2 = M1B->getNext();
        CgHeEdge* M2C = BM2->getNext();
        CgHeEdge* CM3 = M2C->getNext();
        CgHeEdge* M3A = CM3->getNext();

        CgHeVert* M1 = M1B->getVert();
        CgHeVert* M2 = M2C->getVert();
        CgHeVert* M3 = M3A->getVert();

        CgHeEdge* M1M2 = new CgHeEdge();
        M1M2->setVert(M1);
        CgHeEdge* M2M1 = new CgHeEdge();
        M2M1->setVert(M2);
        M1M2->setPair(M2M1);
        M2M1->setPair(M1M2);

        CgHeEdge* M2M3 = new CgHeEdge();
        M2M3->setVert(M2);
        CgHeEdge* M3M2 = new CgHeEdge();
        M3M2->setVert(M3);
        M2M3->setPair(M3M2);
        M3M2->setPair(M2M3);

        CgHeEdge* M3M1 = new CgHeEdge();
        M3M1->setVert(M3);
        CgHeEdge* M1M3 = new CgHeEdge();
        M1M3->setVert(M1);
        M3M1->setPair(M1M3);
        M1M3->setPair(M3M1);

        CgHeFace* A_M1_M3 = startFace;
        CgHeFace* B_M2_M1 = new CgHeFace();
        CgHeFace* C_M3_M2 = new CgHeFace();
        CgHeFace* M1_M2_M3 = new CgHeFace();

        AM1->setNext(M1M3);
        M1M3->setNext(M3A);
        M3A->setNext(AM1);
        AM1->setFace(A_M1_M3);
        M1M3->setFace(A_M1_M3);
        M3A->setFace(A_M1_M3);

        B_M2_M1->setEdge(M1B);
        M1B->setNext(BM2);
        BM2->setNext(M2M1);
        M2M1->setNext(M1B);
        M1B->setFace(B_M2_M1);
        M2M1->setFace(B_M2_M1);
        BM2->setFace(B_M2_M1);

        C_M3_M2->setEdge(M2C);
        M2C->setNext(CM3);
        CM3->setNext(M3M2);
        M3M2->setNext(M2C);
        M2C->setFace(C_M3_M2);
        M3M2->setFace(C_M3_M2);
        CM3->setFace(C_M3_M2);

        M1_M2_M3->setEdge(M3M1);
        M3M1->setNext(M1M2);
        M1M2->setNext(M2M3);
        M2M3->setNext(M3M1);
        M3M1->setFace(M1_M2_M3);
        M1M2->setFace(M1_M2_M3);
        M2M3->setFace(M1_M2_M3);

        this->m_edges.push_back(M1M2);
        this->m_edges.push_back(M2M1);
        this->m_edges.push_back(M2M3);
        this->m_edges.push_back(M3M2);
        this->m_edges.push_back(M3M1);
        this->m_edges.push_back(M1M3);

        this->m_faces.push_back(B_M2_M1);
        this->m_faces.push_back(C_M3_M2);
        this->m_faces.push_back(M1_M2_M3);
    }

    glm::vec3 newPositions[vertexCount];
    for (int i = 0; i < vertexCount; ++i) {
        newPositions[i] = calculateNewVerticePosition(dynamic_cast<CgHeVert*>(m_vertices[i]));
    }
    for (int i = 0; i < vertexCount; ++i) {
        dynamic_cast<CgHeVert*>(m_vertices[i])->setPosition(newPositions[i]);
    }
    this->consistencyCheck();
    this->calculateNormals();
}

glm::vec3 CgHalfEdgeTriangleMesh::calculateNewVerticePosition(CgHeVert *vertex) {
    std::vector<glm::vec3> vertices;
    CgHeEdge* start = vertex->getEdge();
    CgHeEdge* boundary_prev = nullptr;
    CgHeEdge* boundary_next = nullptr;
    bool boundary = false;

    CgHeEdge* edge = start;
    do {
        vertices.push_back(edge->getNext()->getVert()->position());
        if (edge->getPair() == nullptr) {
            vertices.clear();
            boundary_next = edge;
            boundary = true;
            break;
        }
        edge = edge->getPair()->getNext();
    }
    while (edge != start);

    // only true if we didnt hit a boundary
    if (boundary) {
        edge = start;
        do {
            edge = edge->getNext()->getNext();
            if (edge->getPair() == nullptr) {
                boundary_prev = edge;
                break;
            }
            edge = edge->getPair();
        }
        while (edge != start);
    }

    if (boundary) {
        if (boundary_prev == nullptr) {
            std::cerr << "Boundary vertex without two boundary edges" << std::endl;
            return glm::vec3(0.0f);
        }
        glm::vec3 prev_pos = boundary_prev->getNext()->getVert()->position();
        glm::vec3 next_pos = boundary_next->getNext()->getVert()->position();
        glm::vec3 current = vertex->position();
        glm::vec3 newPos = current * 3.0f/4.0f + prev_pos * 1.0f/8.0f + next_pos * 1.0f/8.0f;
        return newPos;
    }

    const float beta = calculateBeta(vertices.size());
    glm::vec3 sum(0.0f);
    for (auto vertice : vertices) {
        sum += vertice;
    }
    sum *= beta;
    sum += (1-vertices.size() * beta) * vertex->position();
    return sum;
}

float CgHalfEdgeTriangleMesh::calculateBeta(const int size) {
    const float inner_cos = cos(2.0f * M_PI / size);
    const float inner_bracket = 3.0f/8.0f + 1.0f/4.0f * inner_cos;
    const float inner_square = inner_bracket * inner_bracket;
    const float outer_bracket = 5.0f/8.0f - inner_square;
    const float beta = 1.0f/size * outer_bracket;
    return beta;
}


