#ifndef CGBASEHALFDGEPRIMITIVES_H
#define CGBASEHALFDGEPRIMITIVES_H
#include <glm/glm.hpp>

class CgBaseHeVert;
class CgBaseHeEdge;

class CgBaseHeFace
{
public:
    virtual ~CgBaseHeFace(){};

    virtual const CgBaseHeEdge* edge() const =0;
    virtual const glm::vec3 normal() const =0;

};

class CgBaseHeEdge
{
public:
    virtual ~CgBaseHeEdge(){};

    virtual const CgBaseHeEdge* next() const =0;
    virtual const CgBaseHeEdge* pair() const =0;
    virtual const CgBaseHeVert* vert() const =0;
    virtual const CgBaseHeFace* face() const =0;
};



class CgBaseHeVert
{
public:
    virtual ~CgBaseHeVert(){};

    virtual const CgBaseHeEdge* edge() const =0;
    virtual const glm::vec3 position() const =0;
    virtual const glm::vec3 color() const =0;

};



#endif // CGBASEHALFDGEPRIMITIVES_H
