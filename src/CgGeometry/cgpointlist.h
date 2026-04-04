#ifndef CG_POINT_LIST_H
#define CG_POINT_LIST_H

#include <vector>
#include <glm/glm.hpp>
#include "cgbasepointlist.h"


class CgPointList : public CgBasePointList
{

public:
    CgPointList();
    explicit CgPointList(std::vector<glm::vec3>& vertices);
    
    ~CgPointList() override;

    //inherited from CgBaseRenderableObject
    ObjectType getType() const override;
    unsigned int getID() const override;
    glm::vec3 getColor() const override;
   
    void initTestGeometry();

    //inherited from CgBasePointList
    const std::vector<glm::vec3>& getVertices() const override;
    const int getLineWidth() const override;
    
    void setLineStyle(LineStyle newVal) override;
    const LineStyle getLineStyle() const override;
  
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

