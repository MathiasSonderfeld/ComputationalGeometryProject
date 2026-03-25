#ifndef CG_POLYLINE_H
#define CG_POLYLINE_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "cgbasepointlist.h"


class CgPointList : public CgBasePointList
{

public:
    CgPointList();
    CgPointList(std::vector<glm::vec3>& verts);
    
    ~CgPointList();

    //inherited from CgBaseRenderableObject
    ObjectType getType() const;
    unsigned int getID() const;
    glm::vec3 getColor() const;
   
    void initTestGeometry();

    //inherited from CgBasePointList
    const std::vector<glm::vec3>& getVertices() const;
    const int getLineWidth() const;
    
    void setLineStyle(LineStyle newVal);
    const LineStyle getLineStyle() const;
  
    void setLineWidth(int);
    

private:

    std::vector<glm::vec3> m_vertices;
    int m_linewidth;
    

    LineStyle m_line_style;

    const ObjectType m_type;
    const unsigned int m_id;

    // if no material is used
    glm::vec3 m_color;
};


inline const LineStyle  CgPointList::getLineStyle() const {return m_line_style;}
inline ObjectType  CgPointList::getType() const {return m_type;}
inline unsigned int CgPointList::getID() const {return m_id;}
inline glm::vec3 CgPointList::getColor() const {return m_color;}

#endif

