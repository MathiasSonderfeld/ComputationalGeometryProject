#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#include "cgimage.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <iostream>


CgImage::CgImage()
{
    m_image_width = 0;
    m_image_height = 0;
    m_image_data = nullptr;
    m_orig_channels = 0;
}

CgImage::~CgImage()
{
    delete [] m_image_data;
}

int CgImage::getImageWidth() const
{
    return m_image_width;
}

int CgImage::getImageHeight() const
{
    return m_image_height;
}

void CgImage::setIntensity(int pos_x, int pos_y, int r, int g, int b)
{
    // to be implemented
}

void CgImage::setIntensity(int pos_x, int pos_y, int intensity)
{
    // to be implemented
}

int CgImage::getIntensity(int pos_x, int pos_y)
{
    // return red value in case of RGB
    // to be implemented
    return 0;
}

void CgImage::convertImageToGreyScale()
{
    // to be implemented
}


void CgImage::drawCross(int r, int g, int b, int linewidth)
{
    // to be implemented
}

void CgImage::storeOriginalImage()
{
    // eine Kopie des aktuellen Bildes merken
    // to be implemented
}

void CgImage::resetImage()
{
    // aktuelles Bild aus der Kopie wiederherstellen ohne neu zu laden
    // to be implemented
}

void CgImage::deleteImage()
{
    // Speicher aufräumen, wird z.B. aufgerufen wenn ein neues Bild geladen wird
    // to be implemented
}

void CgImage::deleteOrigImage()
{
    // Speicher aufräumen, wird z.B. aufgerufen bevor ein aktuelles Bild gemerkt werden soll
    // to be implemented
}


// Simple helper function to load an image into unsigned char* with common settings
bool CgImage::LoadFromFile(const char* filename)
{
    deleteImage();
    deleteOrigImage();

    int orig_num_channels;

    // set desired number of channels
    m_channels = 3;

    // Load from file
    unsigned char* image_data = stbi_load(filename, &m_image_width, &m_image_height, &orig_num_channels, 4);
    if (image_data == nullptr)
        return false;

    m_image_data = new unsigned char[m_image_width * m_image_height * m_channels];


    for (unsigned int i = 0; i < m_image_width * m_image_height; i++)
    {
        m_image_data[m_channels * i] = image_data[4 * i];
        m_image_data[m_channels * i + 1] = image_data[4 * i + 1];
        m_image_data[m_channels * i + 2] = image_data[4 * i + 2];
    }

    stbi_image_free(image_data);

    storeOriginalImage();
    createTexture();

    return true;
}


/********************************************************************
*
*
*   OpenGl stuff - alles, was hier unten steht, muss nicht angefasst werden
*
*
****************************************************************************************************/

GLuint& CgImage::getImageTexture()
{
    return m_image_texture;
}


// OpenGL helper function to convert unsigned char* into OpenGL Texture
// not necessary to unterstand
void CgImage::createTexture()
{
    // Create a OpenGL texture identifier

    glGenTextures(1, &m_image_texture);
    glBindTexture(GL_TEXTURE_2D, m_image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same


    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    switch (m_channels)
    {
    case 1:
        {
            unsigned char* rgb_image_data = new unsigned char[m_image_width * m_image_height * 3];
            for (unsigned int i = 0; i < m_image_width * m_image_height; i++)
            {
                rgb_image_data[3 * i] = m_image_data[i];
                rgb_image_data[3 * i + 1] = m_image_data[i];
                rgb_image_data[3 * i + 2] = m_image_data[i];
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         rgb_image_data);
            delete [] rgb_image_data;
            break;
        }
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     m_image_data);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image_width, m_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     m_image_data);
        break;
    default:
        break;
    }
}
