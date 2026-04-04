#include "cgeigendecomposition3x3.h"
#include "./Eigen/Eigenvalues"
#include "glm/glm.hpp"
#include <iostream>

using namespace Eigen;

CgEigenDecomposition3x3::CgEigenDecomposition3x3(glm::mat3 arg)
{
    Matrix3d A;
    A(0, 0) = arg[0][0];
    A(0, 1) = arg[0][1];
    A(0, 2) = arg[0][2];
    A(1, 0) = arg[1][0];
    A(1, 1) = arg[1][1];
    A(1, 2) = arg[1][2];
    A(2, 0) = arg[2][0];
    A(2, 1) = arg[2][1];
    A(2, 2) = arg[2][2];

    EigenSolver<Matrix3d> es;
    es.compute(A, true);


    MatrixXcd V = es.eigenvectors();
    VectorXcd res = es.eigenvalues();
    std::cout << "The eigenvectors of A are: " << V.transpose() << std::endl;
    std::cout << "The eigenvalues of A are: " << res.transpose() << std::endl;

    m_eigenvectors[0][0] = V(0, 0).real();
    m_eigenvectors[0][1] = V(0, 1).real();
    m_eigenvectors[0][2] = V(0, 2).real();
    m_eigenvectors[1][0] = V(1, 0).real();
    m_eigenvectors[1][1] = V(1, 1).real();
    m_eigenvectors[1][2] = V(1, 2).real();
    m_eigenvectors[2][0] = V(2, 0).real();
    m_eigenvectors[2][1] = V(2, 1).real();
    m_eigenvectors[2][2] = V(2, 2).real();

    m_eigenvalues[0] = res(0).real();
    m_eigenvalues[1] = res(1).real();
    m_eigenvalues[2] = res(2).real();
}


glm::mat3 CgEigenDecomposition3x3::getEigenvectors() const
{
    return m_eigenvectors;
}

glm::vec3 CgEigenDecomposition3x3::getEigenvalues() const
{
    return m_eigenvalues;
}
