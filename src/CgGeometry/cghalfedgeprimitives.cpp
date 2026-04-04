#include "cghalfedgeprimitives.h"

CgHeFace::CgHeFace()
{
    m_edge=nullptr;
}

CgHeVert::CgHeVert()
{
    m_edge=nullptr;
}

CgHeEdge::CgHeEdge()
{
    m_next=nullptr;
    m_pair=nullptr;
    m_vert=nullptr;
    m_face=nullptr;
}

CgHeEdge::~CgHeEdge()
= default;

CgHeVert::~CgHeVert()
= default;

CgHeFace::~CgHeFace()
= default;

const CgBaseHeEdge* CgHeFace::edge() const
{
    return static_cast<CgBaseHeEdge*>(m_edge);
}

const glm::vec3 CgHeFace::normal() const
{
    return m_normal;
}

const CgBaseHeVert* CgHeEdge::vert() const
{
    return static_cast<CgBaseHeVert*>(m_vert);
}

const CgBaseHeEdge* CgHeEdge::next() const
{
    return static_cast<CgBaseHeEdge*>(m_next);
}

const CgBaseHeEdge* CgHeEdge::pair() const
{
    return static_cast<CgBaseHeEdge*>(m_pair);
}

const CgBaseHeFace* CgHeEdge::face() const
{
    return static_cast<CgBaseHeFace*>(m_face);
}


const CgBaseHeEdge* CgHeVert::edge() const
{
    return static_cast<CgBaseHeEdge*>(m_edge);
}

const glm::vec3 CgHeVert::position() const
{
    return m_position;
}

const glm::vec3 CgHeVert::color() const
{
    return m_color;
}
