#ifndef CG_BASE_HALF_EDGE_PRIMITIVES_H
#define CG_BASE_HALF_EDGE_PRIMITIVES_H
#include <glm/glm.hpp>

class CgBaseHeVert;
class CgBaseHeEdge;

class CgBaseHeFace
{
public:
    virtual ~CgBaseHeFace() = default;

    virtual const CgBaseHeEdge* edge() const =0;
    virtual const glm::vec3 normal() const =0;
    virtual void setNormal(glm::vec3);
};

class CgBaseHeEdge
{
public:
    virtual ~CgBaseHeEdge() = default;

    virtual const CgBaseHeEdge* next() const =0;
    virtual const CgBaseHeEdge* pair() const =0;
    virtual const CgBaseHeVert* vert() const =0;
    virtual const CgBaseHeFace* face() const =0;
};


class CgBaseHeVert
{
public:
    virtual ~CgBaseHeVert() = default;

    virtual const CgBaseHeEdge* edge() const =0;
    virtual const glm::vec3 position() const =0;
    virtual const glm::vec3 color() const =0;
};


#endif // CG_BASE_HALF_EDGE_PRIMITIVES_H
