#ifndef CG_TRIANGLEFAN_H
#define CG_TRIANGLEFAN_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "cgbasetrianglefan.h"


class CgTriangleFan : public CgBaseTriangleFan
{

public:
    CgTriangleFan();
    CgTriangleFan(std::vector<glm::vec3>& verts);
    
    ~CgTriangleFan();

    //inherited from CgBaseRenderableObject
    ObjectType getType() const;
    unsigned int getID() const;
    glm::vec3 getColor() const;

    // inherited from CgBaseTriangleFan
    // expects center vertex and all other vertices ordered counterclockwise
    // in one list
    const std::vector<glm::vec3>& getVertices() const;
   
   

protected:

    std::vector<glm::vec3> m_vertices;
    const ObjectType m_type;
    const unsigned int m_id;
    
    // if no material is used
    glm::vec3 m_color;

private:
    
  
};


inline ObjectType  CgTriangleFan::getType() const {return m_type;}
inline unsigned int CgTriangleFan::getID() const {return m_id;}
inline glm::vec3 CgTriangleFan::getColor() const {return m_color;}

#endif // CG_TRIANGLEFAN_H
