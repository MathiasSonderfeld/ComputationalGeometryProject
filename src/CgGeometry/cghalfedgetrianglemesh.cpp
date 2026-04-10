#include "cghalfedgetrianglemesh.h"


extern int getUniqueId();


CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh() : m_type(HalfEdgeTriangleMesh),
                                                   m_id(getUniqueId()) {
    CgHeFace* nf1 = new CgHeFace();
    CgHeFace* nf2 = new CgHeFace();

    CgHeVert* nv1 = new CgHeVert();
    CgHeVert* nv2 = new CgHeVert();
    CgHeVert* nv3 = new CgHeVert();
    CgHeVert* nv4 = new CgHeVert();

    CgHeEdge* n1 = new CgHeEdge();
    CgHeEdge* n2 = new CgHeEdge();
    CgHeEdge* n3 = new CgHeEdge();
    CgHeEdge* n4 = new CgHeEdge();
    CgHeEdge* n5 = new CgHeEdge();
    CgHeEdge* n6 = new CgHeEdge();


    n1->m_next = n2;
    n2->m_next = n3;
    n3->m_next = n1;
    nf1->m_edge = n1;

    n4->m_next = n5;
    n5->m_next = n6;
    n6->m_next = n4;
    nf2->m_edge = n4;

    nv1->m_position = glm::vec3(0.0, 0.0, 0.0);
    nv2->m_position = glm::vec3(0.5, 0.0, 0.0);
    nv3->m_position = glm::vec3(0.5, 0.5, 0.0);
    nv4->m_position = glm::vec3(0.0, 0.5, 0.0);

    nv1->m_edge = n1;
    nv2->m_edge = n2;
    nv3->m_edge = n6;
    nv4->m_edge = n3;

    n4->m_vert = nv4;
    n5->m_vert = nv2;
    n6->m_vert = nv3;

    n1->m_vert = nv1;
    n2->m_vert = nv2;
    n3->m_vert = nv4;

    n2->m_pair = n4;
    n4->m_pair = n2;

    // store into lists
    m_faces.push_back(nf1);
    m_faces.push_back(nf2);

    m_verts.push_back(nv1);
    m_verts.push_back(nv2);
    m_verts.push_back(nv3);
    m_verts.push_back(nv4);

    m_edges.push_back(n1);
    m_edges.push_back(n2);
    m_edges.push_back(n3);
    m_edges.push_back(n4);
    m_edges.push_back(n5);
    m_edges.push_back(n6);

    //attributes
    nv1->m_color = glm::vec3(0.0, 1.0, 0.0);
    nv2->m_color = glm::vec3(0.0, 1.0, 0.0);
    nv3->m_color = glm::vec3(0.0, 1.0, 0.0);
    nv4->m_color = glm::vec3(0.0, 1.0, 0.0);

    nf1->m_normal = glm::vec3(0.0, 0.0, 1.0);
    nf2->m_normal = glm::vec3(0.0, 0.0, 1.0);
}

CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh(
    std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norm,
    std::vector<unsigned int>& idx) : CgHalfEdgeTriangleMesh() {
    // data ignored at the moment
    // to be implemented .... :-)
    // calling standard constructor instead for simple example
}


CgHalfEdgeTriangleMesh::~CgHalfEdgeTriangleMesh() {
    // thats not enough, have to kill Objects as well´
    m_faces.clear();
    m_edges.clear();
    m_verts.clear();
}

const std::vector<CgBaseHeFace*>& CgHalfEdgeTriangleMesh::getFaces() const {
    return m_faces;
}

const glm::vec3 CgHalfEdgeTriangleMesh::getCenter() const {
    glm::vec3 center(0.);
    for (const auto m_vert : m_verts) {
        center += m_vert->position();
    }
    center /= static_cast<double>(m_verts.size());
    return center;
}
