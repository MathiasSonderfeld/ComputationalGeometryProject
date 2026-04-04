#ifndef CG_TRIANGLE_FAN_H
#define CG_TRIANGLE_FAN_H

#include <vector>
#include <glm/glm.hpp>
#include "cgbasetrianglefan.h"


class CgTriangleFan : public CgBaseTriangleFan
{
public:
    CgTriangleFan();
    CgTriangleFan(std::vector<glm::vec3>& vertices);

    ~CgTriangleFan() override;

    //inherited from CgBaseRenderableObject
    ObjectType getType() const override;
    unsigned int getID() const override;
    glm::vec3 getColor() const override;

    // inherited from CgBaseTriangleFan
    // expects center vertex and all other vertices ordered counterclockwise
    // in one list
    const std::vector<glm::vec3>& getVertices() const override;

protected:
    std::vector<glm::vec3> m_vertices;
    const ObjectType m_type;
    const unsigned int m_id;

    // if no material is used
    glm::vec3 m_color;
};


inline ObjectType CgTriangleFan::getType() const { return m_type; }
inline unsigned int CgTriangleFan::getID() const { return m_id; }
inline glm::vec3 CgTriangleFan::getColor() const { return m_color; }

#endif // CG_TRIANGLE_FAN_H
