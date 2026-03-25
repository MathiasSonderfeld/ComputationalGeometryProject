#ifndef CGEIGENDECOMPOSITION3X3_H
#define CGEIGENDECOMPOSITION3X3_H

#include "glm/glm.hpp"


class CgEigenDecomposition3x3
{
public:
  CgEigenDecomposition3x3(glm::mat3 arg);

  glm::mat3 getEigenvectors();
  glm::vec3 getEigenvalues();


private:

    glm::mat3 m_eigenvectors;
    glm::vec3 m_eigenvalues;

};

#endif // CGEIGENDECOMPOSITION3X3_H
