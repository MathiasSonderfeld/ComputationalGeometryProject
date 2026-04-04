#ifndef CG_IMAGE_H
#define CG_IMAGE_H

#include <GLFW/glfw3.h>

class CgImage
{
public:
  CgImage();
  ~CgImage();

public:
  bool LoadFromFile(const char* filename);
  int getImageWidth() const;
  int getImageHeight() const;
  GLuint& getImageTexture();

  void setIntensity(int pos_x, int pos_y, int r, int g, int b);
  void setIntensity(int pos_x, int pos_y, int intensity);
  static int getIntensity(int pos_x, int pos_y);

  void drawCross(int r, int g, int b, int linewidth);
  void resetImage();
  void convertImageToGreyScale();

private:
  // hold the image data, can be manipulated, call createTexture() after manipulation
  unsigned char* m_image_data;
  //unsigned char* m_orig_image_data;

  int m_orig_channels;

  GLuint m_image_texture{};
  int m_image_width;
  int m_image_height;
  int m_channels{};

private:
  // Simple helper function to convert unsigned char* into OpenGL Texture
  void createTexture();

  void storeOriginalImage();
  void deleteImage();
  void deleteOrigImage();
  // Aufgabe 1
};

#endif
