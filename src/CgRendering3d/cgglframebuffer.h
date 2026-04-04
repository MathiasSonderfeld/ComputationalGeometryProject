#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H


#include <GL/glew.h>

class CgGLFrameBuffer
{
public:
    CgGLFrameBuffer() = default;

    void create_buffers(GLuint width, GLuint height);

    void delete_buffers();

    void bind() const;

    static void unbind();

    GLuint get_texture() const;

protected:
    GLuint mFBO = 0;
    GLuint mTexId = 0;
    GLuint mDepthId = 0;
    GLuint mWidth = 0;
    GLuint mHeight = 0;
};

#endif
