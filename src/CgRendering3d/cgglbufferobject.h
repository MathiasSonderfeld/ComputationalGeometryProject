#ifndef CG_GL_BUFFER_OBJECT
#define CG_GL_BUFFER_OBJECT

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vertexholder.h"
#include <vector>

class CgBaseRenderableObject;


class CgGLBufferObject {
    
    
public:

    CgGLBufferObject();
    ~CgGLBufferObject();
    
    void renderObject(CgBaseRenderableObject* obj);
   
    
    void create_vertex_index_buffers(const std::vector<VertexHolder>& vertices,
                                     std::vector<unsigned int>& indices);
    void create_vertex_buffers(const std::vector<VertexHolder>& vertices);
   
    
private:
    
    GLuint mVBO;
    GLuint mVAO;
    GLuint mIBO;
    
};


#endif
