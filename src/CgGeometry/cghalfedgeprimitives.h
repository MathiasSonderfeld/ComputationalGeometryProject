#ifndef CG_HALF_EDGE_PRIMITIVES_H
#define CG_HALF_EDGE_PRIMITIVES_H

#include <glm/glm.hpp>
#include "cgbasehalfedgeprimitives.h"

//forward declaration
class CgHeVert;
class CgHeEdge;

class CgHeFace : public CgBaseHeFace
{
public:
    CgHeFace();
    ~CgHeFace() override;

    const CgBaseHeEdge* edge() const override;
    const glm::vec3 normal() const override;

    CgHeEdge* m_edge;

    glm::vec3 m_normal{};
};


class CgHeVert : public CgBaseHeVert
{
public:
    CgHeVert();
    ~CgHeVert() override;

    const CgBaseHeEdge* edge() const override;
    const glm::vec3 position() const override;
    const glm::vec3 color() const override;


    CgHeEdge* m_edge;
    glm::vec3 m_position{};
    glm::vec3 m_color{};
};

class CgHeEdge : public CgBaseHeEdge
{
public:
    CgHeEdge();
    ~CgHeEdge() override;

    const CgBaseHeEdge* next() const override;
    const CgBaseHeEdge* pair() const override;
    const CgBaseHeVert* vert() const override;
    const CgBaseHeFace* face() const override;


    CgHeEdge* m_next;
    CgHeEdge* m_pair;
    CgHeVert* m_vert;
    CgHeFace* m_face;
};


#endif // CG_HALF_EDGE_PRIMITIVES_H
