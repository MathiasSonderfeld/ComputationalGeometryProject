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
    virtual const glm::vec3 center() const =0;
    virtual const glm::vec3 normal() const =0;
    virtual void setNormal(glm::vec3) =0;
    virtual int index1() const =0;
    virtual int index2() const =0;
    virtual int index3() const =0;
    virtual void setIndex1(int) =0;
    virtual void setIndex2(int) =0;
    virtual void setIndex3(int) =0;
};

class CgBaseHeEdge
{
public:
    virtual ~CgBaseHeEdge() = default;

    virtual const CgBaseHeEdge* next() const =0;
    virtual const CgBaseHeEdge* pair() const =0;
    virtual const CgBaseHeVert* vert() const =0;
    virtual const CgBaseHeFace* face() const =0;
    virtual int startIndex() const =0;
    virtual int endIndex() const =0;
    virtual void setStartIndex(int) =0;
    virtual void setEndIndex(int) =0;
};


class CgBaseHeVert
{
public:
    virtual ~CgBaseHeVert() = default;

    virtual const CgBaseHeEdge* edge() const =0;
    virtual const glm::vec3 position() const =0;
    virtual const glm::vec3 color() const =0;
    virtual const glm::vec3 normal() const =0;
    virtual void setNormal(glm::vec3) =0;
    virtual int index() const =0;
    virtual void setIndex(int) =0;
};


#endif // CG_BASE_HALF_EDGE_PRIMITIVES_H
