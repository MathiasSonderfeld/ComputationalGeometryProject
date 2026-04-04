#include "cgtrianglemesh.h"
#include "tiny_obj_loader.h"


extern int getUniqueId();

CgTriangleMesh::CgTriangleMesh() :
    m_type(TriangleMesh),
    m_id(getUniqueId()),
    m_color(glm::vec3(0.0, 1.0, 0.0))
{
}

CgTriangleMesh::CgTriangleMesh(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& norm,
                               const std::vector<unsigned int>& idx) :
    m_type(TriangleMesh),
    m_id(getUniqueId()),
    m_color(glm::vec3(0.0, 1.0, 0.0))
{
    m_vertices.clear();
    m_triangle_indices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();

    for (glm::vec3& vertice : vertices)
        m_vertices.emplace_back(vertice);

    for (glm::vec3& i : norm)
        m_vertex_normals.emplace_back(i);

    for (int i = 0; i < norm.size(); i++)
        m_vertex_normals[i] = glm::normalize(m_vertex_normals[i]);

    for (unsigned int i : idx)
        m_triangle_indices.push_back(i);

    // set same color for every vertex
    for (int i = 0; i < vertices.size(); i++)
        m_vertex_colors.emplace_back(1.0, 0.0, 0.0);
}

CgTriangleMesh::~CgTriangleMesh()
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_tex_coords.clear();
    m_triangle_indices.clear();
    m_face_normals.clear();
    m_face_colors.clear();
}


bool CgTriangleMesh::hasIntersection(const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const
{
    // not yet implemented
    return false;
}


const std::vector<glm::vec3>& CgTriangleMesh::getVertices() const
{
    return m_vertices;
}

const std::vector<glm::vec3>& CgTriangleMesh::getVertexNormals() const
{
    return m_vertex_normals;
}

const std::vector<glm::vec3>& CgTriangleMesh::getVertexColors() const
{
    return m_vertex_colors;
}

const std::vector<glm::vec2>& CgTriangleMesh::getVertexTexCoords() const
{
    return m_tex_coords;
}

const std::vector<unsigned int>& CgTriangleMesh::getTriangleIndices() const
{
    return m_triangle_indices;
}

const std::vector<glm::vec3>& CgTriangleMesh::getFaceNormals() const
{
    return m_face_normals;
}

const std::vector<glm::vec3>& CgTriangleMesh::getFaceColors() const
{
    return m_face_colors;
}
