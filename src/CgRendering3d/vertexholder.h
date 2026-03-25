#ifndef VERTEXHOLDER_H
#define VERTEXHOLDER_H

#include <glm/glm.hpp>



class VertexHolder
  {

  public:
    VertexHolder() : mPos(), mNormal() {}
    
    VertexHolder(const glm::vec3& pos)
        : mPos(pos), mNormal(glm::vec3(0.0,0.0,0.0)), mColor(glm::vec3(0.0,0.0,1.0))
      {
          
      }
      
    VertexHolder(const glm::vec3& pos, const glm::vec3& normal)
      : mPos(pos), mNormal(normal),mColor(glm::vec3(0.0,0.0,1.0))
    {
    }
      
      VertexHolder(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& color)
        : mPos(pos), mNormal(normal),mColor(color)
      {
      }

    ~VertexHolder() = default;


    glm::vec3 mPos;
    glm::vec3 mNormal;
    glm::vec3 mColor;
   

    // TODO
    // glm::vec2 mUV;


  };

#endif
