#include "cgpointlist.h"
#include <iostream>



extern int getUniqueId();

CgPointList::CgPointList():
m_linewidth(1),
m_line_style(CG_POINTS),
m_type(PointList),
m_id(getUniqueId())
{
    m_color = glm::vec3(0.0,0.0,1.0);
    initTestGeometry();
}

CgPointList::CgPointList(std::vector<glm::vec3>& verts):
m_linewidth(1),
m_line_style(CG_POINTS),
m_type(PointList),
m_id(getUniqueId())
{
    m_color = glm::vec3(0.0,0.0,1.0);
    m_vertices.clear();
    
    for(int i=0;i<verts.size();i++)
        m_vertices.push_back(glm::vec3(verts[i]));
}

CgPointList::~CgPointList()
{
    m_vertices.clear();
}

void CgPointList::initTestGeometry()
{
    m_vertices.push_back(glm::vec3(-0.5,0.0,0.0));
    m_vertices.push_back(glm::vec3(0.0,-0.5,0.0));
    m_vertices.push_back(glm::vec3(0.0,0.5,0.0));
    m_vertices.push_back(glm::vec3(0.5,0.0,0.0));
}


const std::vector<glm::vec3>& CgPointList::getVertices() const
{
    return m_vertices;
}


const int CgPointList::getLineWidth() const
{
    return m_linewidth;
}

void CgPointList::setLineWidth(int newVal)
{
    m_linewidth=newVal;
}

void CgPointList::setLineStyle(LineStyle newVal)
{
    m_line_style=newVal;
}
