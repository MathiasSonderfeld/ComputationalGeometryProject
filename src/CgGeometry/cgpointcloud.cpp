#include "cgpointcloud.h"
#include <iostream>


extern int getUniqueId();

CgPointCloud::CgPointCloud():
m_type(PointCloud),
m_id(getUniqueId()),
m_color(glm::vec3(0.0,1.0,0.0))
{
    
}

CgPointCloud::CgPointCloud(std::vector<glm::vec3>& verts):
m_type(PointCloud),
m_id(getUniqueId()),
m_color(glm::vec3(0.0,1.0,0.0))
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    
    for(int i=0;i<verts.size();i++)
    {
        m_vertices.push_back(glm::vec3(verts[i]));
        
        // for testiung purposes
        // comment if not desired
        if(verts[i].x>0)
            m_vertex_colors.push_back(glm::vec3(0.0,1.0,0.0));
        else
            m_vertex_colors.push_back(glm::vec3(0.0,0.0,1.0));
    }
             
}

CgPointCloud::~CgPointCloud()
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_splatdir_x.clear();
    m_splatdir_y.clear(); 
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
    return m_splatdir_x;
}

const std::vector<glm::vec3>& CgPointCloud::getSplatDirY() const
{
    return m_splatdir_y;
}

const glm::vec3 CgPointCloud::getClosestPoint (const glm::vec3& origin, const glm::vec3& dir, const double maxDistance) const
{
    // to be implemented
    return glm::vec3(0.0,0.0,0.0);
}
