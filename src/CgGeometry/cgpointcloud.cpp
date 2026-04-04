#include "cgpointcloud.h"

extern int getUniqueId();

CgPointCloud::CgPointCloud() :
    m_type(PointCloud),
    m_id(getUniqueId()),
    m_color(glm::vec3(0.0, 1.0, 0.0))
{
}

CgPointCloud::CgPointCloud(std::vector<glm::vec3>& vertices) :
    m_type(PointCloud),
    m_id(getUniqueId()),
    m_color(glm::vec3(0.0, 1.0, 0.0))
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();

    for (auto& vert : vertices)
    {
        m_vertices.emplace_back(vert);

        // for testing purposes
        // comment if not desired
        if (vert.x > 0)
            m_vertex_colors.emplace_back(0.0, 1.0, 0.0);
        else
            m_vertex_colors.emplace_back(0.0, 0.0, 1.0);
    }
}

CgPointCloud::~CgPointCloud()
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_splat_dir_x.clear();
    m_splat_dir_y.clear();
}


const std::vector<glm::vec3>& CgPointCloud::getVertices() const
{
    return m_vertices;
}

const std::vector<glm::vec3>& CgPointCloud::getVertexNormals() const
{
    return m_vertex_normals;
}

const std::vector<glm::vec3>& CgPointCloud::getVertexColors() const
{
    return m_vertex_colors;
}


const std::vector<glm::vec3>& CgPointCloud::getSplatDirX() const
{
    return m_splat_dir_x;
}

const std::vector<glm::vec3>& CgPointCloud::getSplatDirY() const
{
    return m_splat_dir_y;
}

const glm::vec3 CgPointCloud::getClosestPoint(const glm::vec3& origin, const glm::vec3& dir,
                                              const double maxDistance) const
{
    // to be implemented
    return glm::vec3(0.0, 0.0, 0.0);
}
