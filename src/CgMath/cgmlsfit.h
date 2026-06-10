#ifndef CG_MLS_FIT_H
#define CG_MLS_FIT_H

#include <vector>
#include <utility>
#include "glm/glm.hpp"

// A bivariate polynomial height field P(u,v) fitted over the tangent plane of a
// point. The 3D position of a parameter pair is:
//   origin + u*tangentU + v*tangentV + P(u,v)*normal
// This is the local model used for Moving Least Squares smoothing.
struct CgMlsSurface {
    glm::vec3 origin{};
    glm::vec3 normal{};
    glm::vec3 tangentU{};
    glm::vec3 tangentV{};
    int degree = 0;

    std::vector<float> coefficients;          // one coefficient per monomial term
    std::vector<std::pair<int, int>> terms;   // exponents (a,b) of each term: u^a * v^b
    std::vector<glm::vec2> params;            // (u,v) of every sample, for AABB / sampling

    [[nodiscard]] bool valid() const { return !coefficients.empty(); }

    // P(u,v) = sum_k coefficients[k] * u^a_k * v^b_k
    [[nodiscard]] float evaluate(float u, float v) const;

    // lift a parameter pair back into world space
    [[nodiscard]] glm::vec3 positionAt(float u, float v) const;

    // 2D axis-aligned bounds of the stored sample params
    void paramBounds(glm::vec2& outMin, glm::vec2& outMax) const;
};

// Fit a degree-`degree` polynomial to the heights of `samples` over the tangent
// plane at `center` (the selected point's own position should be included among
// the samples). `normal` is the surface normal at `center`.
// Uses a Gaussian-weighted least-squares solve via SVD (the "moving" weighting
// makes closer samples count more).
CgMlsSurface fitMlsSurface(const glm::vec3& center, const glm::vec3& normal,
                           const std::vector<glm::vec3>& samples, int degree);

#endif // CG_MLS_FIT_H
