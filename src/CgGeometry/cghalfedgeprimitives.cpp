#include "cghalfedgeprimitives.h"

CgHeFace::CgHeFace()
{
    m_edge=NULL;
}

CgHeVert::CgHeVert()
{
    m_edge=NULL;
}

CgHeEdge::CgHeEdge()
{
    m_next=NULL;
    m_pair=NULL;
    m_vert=NULL;
    m_face=NULL;
}

CgHeEdge::~CgHeEdge()
{

}

CgHeVert::~CgHeVert()
{

}

CgHeFace::~CgHeFace()
{

}

const CgBaseHeEdge* CgHeFace::edge() const
{
    return (CgBaseHeEdge*) m_edge;
}

const glm::vec3 CgHeFace::normal() const
{
    return m_normal;
}

const CgBaseHeVert* CgHeEdge::vert() const
{
    return (CgBaseHeVert*) m_vert;
}

const CgBaseHeEdge* CgHeEdge::next() const
{
    return (CgBaseHeEdge*) m_next;
}

const CgBaseHeEdge* CgHeEdge::pair() const
{
    return (CgBaseHeEdge*) m_pair;
}

const CgBaseHeFace* CgHeEdge::face() const
{
    return (CgBaseHeFace*) m_face;
}


const CgBaseHeEdge* CgHeVert::edge() const
{
    return (CgBaseHeEdge*) m_edge;
}

const glm::vec3 CgHeVert::position() const
{
    return m_position;
}

const glm::vec3 CgHeVert::color() const
{
    return m_color;
}
