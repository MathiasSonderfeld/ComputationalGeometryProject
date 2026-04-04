#include "cgtrianglefan.h"
#include "tiny_obj_loader.h"


extern int getUniqueId();

CgTriangleFan::CgTriangleFan() :
    m_type(TriangleFan),
    m_id(getUniqueId()),
    m_color(glm::vec3(1.0, 0.0, 0.0))
{
}

CgTriangleFan::CgTriangleFan(std::vector<glm::vec3>& vertices) :
    m_type(TriangleFan),
    m_id(getUniqueId()),
    m_color(glm::vec3(1.0, 0.0, 0.0))
{
    m_vertices.clear();

    for (auto v : vertices)
        m_vertices.push_back(v);
}

CgTriangleFan::~CgTriangleFan()
{
    m_vertices.clear();
}

const std::vector<glm::vec3>& CgTriangleFan::getVertices() const
{
    return m_vertices;
}

