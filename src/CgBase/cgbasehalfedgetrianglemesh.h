#ifndef CG_BASE_HALF_EDGE_TRIANGLE_MESH_H
#define CG_BASE_HALF_EDGE_TRIANGLE_MESH_H


#include <vector>
#include "cgbaserenderableobject.h"
#include "cgbasehalfedgeprimitives.h"

class CgBaseHalfEdgeTriangleMesh : public CgBaseRenderableObject
{
public:
    ~CgBaseHalfEdgeTriangleMesh() override = default;
    virtual const std::vector<CgBaseHeFace*>& getFaces() const =0;
    virtual const std::vector<CgBaseHeVert*>& getVertices() const =0;
};

#endif // CG_BASE_HALF_EDGE_TRIANGLE_MESH_H
