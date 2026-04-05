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

int CgHeVert::index() const
{
    return m_index;
}

void CgHeVert::setIndex(const int index)
{
    this->m_index = index;
}

int CgHeEdge::startIndex() const
{
    return m_start_index;
}

int CgHeEdge::endIndex() const
{
    return m_end_index;
}

void CgHeEdge::setStartIndex(const int start_index)
{
    this->m_start_index = start_index;
}

void CgHeEdge::setEndIndex(const int end_index)
{
    this->m_end_index = end_index;
}

int CgHeFace::index1() const
{
    return m_index1;
}

int CgHeFace::index2() const
{
    return m_index2;
}

int CgHeFace::index3() const
{
    return m_index3;
}

void CgHeFace::setIndex1(const int index1)
{
    this->m_index1 =index1;
}

void CgHeFace::setIndex2(const int index2)
{
    this->m_index2 = index2;
}

void CgHeFace::setIndex3(const int index3)
{
    this->m_index3 = index3;
}