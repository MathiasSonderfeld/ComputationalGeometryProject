#include "cgglbufferobject.h"
#include <vector>
#include <glm/glm.hpp>
#include "cgbaserenderableobject.h"
#include "cgbasetrianglemesh.h"
#include "cgbasehalfedgetrianglemesh.h"
#include "cgbasepointcloud.h"
#include "cgbasepointlist.h"
#include "cgbasetrianglefan.h"


CgGLBufferObject::CgGLBufferObject()
= default;


CgGLBufferObject::~CgGLBufferObject()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &mIBO);
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}


void CgGLBufferObject::renderObject(CgBaseRenderableObject* obj) const
{
    if (obj->getType() == TriangleMesh)
    {
        const unsigned int isize = dynamic_cast<CgBaseTriangleMesh*>(obj)->getTriangleIndices().size();
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, isize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    if (obj->getType() == HalfEdgeTriangleMesh)
    {
        const unsigned int isize = 3 * dynamic_cast<CgBaseHalfEdgeTriangleMesh*>(obj)->getFaces().size();
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, isize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    if (obj->getType() == PointCloud)
    {
        const unsigned int size = dynamic_cast<CgBasePointCloud*>(obj)->getVertices().size();

        glBindVertexArray(mVAO);
        glDrawArrays(GL_POINTS, 0, size);
        glBindVertexArray(0);
    }

    if (obj->getType() == PointList)
    {
        const CgBasePointList* pointlist = dynamic_cast<CgBasePointList*>(obj);


        glEnableClientState(GL_VERTEX_ARRAY);
        glPointSize(2);
        glBindVertexArray(mVAO);

        switch (pointlist->getLineStyle())
        {
        case CG_LINE_STRIP:
            glDrawArrays(GL_LINE_STRIP, 0, pointlist->getVertices().size());
            break;
        case CG_POINTS:
            glDrawArrays(GL_POINTS, 0, pointlist->getVertices().size());
            break;
        case CG_LINES:
            glDrawArrays(GL_LINES, 0, pointlist->getVertices().size());
            break;
        case CG_LINE_LOOP:
            glDrawArrays(GL_LINE_LOOP, 0, pointlist->getVertices().size());
            break;
        default:
            break;
        }


        glBindVertexArray(0);
        glDisableClientState(GL_VERTEX_ARRAY);
    }


    if (obj->getType() == TriangleFan)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        const unsigned int size = dynamic_cast<CgBaseTriangleFan*>(obj)->getVertices().size();
        glBindVertexArray(mVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, size);
        glBindVertexArray(0);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}


void CgGLBufferObject::create_vertex_index_buffers(const std::vector<VertexHolder>& vertices,
                                                   const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &mVAO);

    glGenBuffers(1, &mIBO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexHolder), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexHolder),
                          reinterpret_cast<void*>(offsetof(VertexHolder, mPos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexHolder),
                          reinterpret_cast<void*>(offsetof(VertexHolder, mNormal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexHolder),
                          reinterpret_cast<void*>(offsetof(VertexHolder, mColor)));

    glBindVertexArray(0);
}


void CgGLBufferObject::create_vertex_buffers(const std::vector<VertexHolder>& vertices)
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexHolder), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexHolder),
                          reinterpret_cast<void*>(offsetof(VertexHolder, mPos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexHolder),
                          reinterpret_cast<void*>(offsetof(VertexHolder, mNormal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexHolder),
                          reinterpret_cast<void*>(offsetof(VertexHolder, mColor)));

    glBindVertexArray(0);
}


