#ifndef CG_BASE_RENDERABLE_OBJECT_H
#define CG_BASE_RENDERABLE_OBJECT_H

#include <glm/glm.hpp>

typedef enum ObjectType
{
    PointCloud = 0x00000001,
    PointList = 0x00000002,
    TriangleMesh = 0x00000004,
    HalfEdgeTriangleMesh = 0x00000008,
    TriangleFan = 0x00000010,
    //the following are unused, feel free to change
    SomeOtherType2 = 0x00000020,
    SomeOtherType3 = 0x00000040,
    SomeOtherType4 = 0x00000080,
    SomeOtherType5 = 0x000000A0,
    SomeOtherType6 = 0x00000100,
    SomeOtherType7 = 0x00000200,
} ObjectType;


typedef enum LineStyle
{
    CG_POINTS = 0x00000001,
    CG_LINES = 0x00000002,
    CG_LINE_STRIP = 0x00000004,
    CG_LINE_LOOP = 0x00000008,
} LineStyle;


class CgBaseRenderableObject
{
public:
    virtual ~CgBaseRenderableObject() = default;

    // old school runtime type information :-)
    virtual ObjectType getType() const =0;

    //unique object id
    virtual unsigned int getID() const =0;

    //object color
    virtual glm::vec3 getColor() const =0;
};

#endif // CG_BASE_RENDERABLE_OBJECT_H

