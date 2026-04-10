#ifndef CG_HALF_EDGE_PRIMITIVES_H
#define CG_HALF_EDGE_PRIMITIVES_H

#include <glm/glm.hpp>
#include "cgbasehalfedgeprimitives.h"

//forward declaration
class CgHeVert;
class CgHeEdge;

class CgHeFace : public CgBaseHeFace {
public:
    CgHeFace();
    ~CgHeFace() override;

    const CgBaseHeEdge* edge() const override;
    const glm::vec3 center() const override;
    const glm::vec3 normal() const override;
    void setNormal(glm::vec3) override;
    int index1() const override;
    int index2() const override;
    int index3() const override;
    void setIndex1(int) override;
    void setIndex2(int) override;
    void setIndex3(int) override;

    CgHeEdge* m_edge;
    glm::vec3 m_normal{};
    int m_index1 = -1;
    int m_index2 = -1;
    int m_index3 = -1;
};


class CgHeVert : public CgBaseHeVert {
public:
    CgHeVert();
    ~CgHeVert() override;

    const CgBaseHeEdge* edge() const override;
    const glm::vec3 position() const override;
    const glm::vec3 color() const override;
    const glm::vec3 normal() const override;
    void setNormal(glm::vec3) override;
    int index() const override;
    void setIndex(int) override;


    CgHeEdge* m_edge;
    glm::vec3 m_position{};
    glm::vec3 m_color{};
    glm::vec3 m_normal{};
    int m_index = -1;
};

class CgHeEdge : public CgBaseHeEdge {
public:
    CgHeEdge();
    ~CgHeEdge() override;

    const CgBaseHeEdge* next() const override;
    const CgBaseHeEdge* pair() const override;
    const CgBaseHeVert* vert() const override;
    const CgBaseHeFace* face() const override;
    int startIndex() const override;
    int endIndex() const override;
    void setStartIndex(int) override;
    void setEndIndex(int) override;

    CgHeEdge* m_next;
    CgHeEdge* m_pair;
    CgHeVert* m_vert;
    CgHeFace* m_face;
    int m_start_index = -1;
    int m_end_index = -1;
};


#endif // CG_HALF_EDGE_PRIMITIVES_H
