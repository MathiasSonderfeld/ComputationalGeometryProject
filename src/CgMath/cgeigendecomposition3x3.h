#ifndef CG_EIGEN_DECOMPOSITION_3X3_H
#define CG_EIGEN_DECOMPOSITION_3X3_H

#include "glm/glm.hpp"


class CgEigenDecomposition3x3
{
public:
    CgEigenDecomposition3x3(glm::mat3 arg);

    glm::mat3 getEigenvectors() const;
    glm::vec3 getEigenvalues() const;

private:
    glm::mat3 m_eigenvectors{};
    glm::vec3 m_eigenvalues{};
};

#endif // CG_EIGEN_DECOMPOSITION_3X3_H
