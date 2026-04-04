#include "cgglframebuffer.h"
#include <iostream>

void CgGLFrameBuffer::create_buffers(const GLuint width, const GLuint height)
{
  mWidth = width;
  mHeight = height;

  if (mFBO)
  {
    delete_buffers();
  }

  glGenFramebuffers(1, &mFBO);

  glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
  glGenTextures(1, &mTexId);

  glBindTexture(GL_TEXTURE_2D, mTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexId, 0);


  glGenTextures(1, &mDepthId);
  glBindTexture(GL_TEXTURE_2D, mDepthId);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, mWidth, mHeight);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthId, 0);

  GLenum buffers[4] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(mTexId, buffers);


  unbind();
}

void CgGLFrameBuffer::delete_buffers()
{
  if (mFBO)
  {
    glDeleteFramebuffers(GL_FRAMEBUFFER, &mFBO);
    glDeleteTextures(1, &mTexId);
    glDeleteTextures(1, &mDepthId);
    mTexId = 0;
    mDepthId = 0;
  }
}

void CgGLFrameBuffer::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
  glViewport(0, 0, mWidth, mHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CgGLFrameBuffer::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint CgGLFrameBuffer::get_texture() const
{
  return mTexId;
}
