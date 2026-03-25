#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

class CgGLFrameBuffer
  {
  public:
      
      CgGLFrameBuffer() : mFBO { 0 }, mDepthId {0}
          {}

    void create_buffers(GLuint width, GLuint height) ;

    void delete_buffers() ;

    void bind() ;

    void unbind() ;
    
    GLuint get_texture() ;
      
      
      
  protected:
      GLuint mFBO = 0;
      GLuint mTexId = 0;
      GLuint mDepthId = 0;
      GLuint mWidth = 0;
      GLuint mHeight = 0;
      
};

#endif
