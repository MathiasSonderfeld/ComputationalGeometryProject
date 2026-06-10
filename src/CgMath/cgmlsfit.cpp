#include "cgmlsfit.h"
#include "./Eigen/Dense"
#include <cmath>
#include <limits>

using namespace Eigen;

// all monomial exponent pairs (a,b) with a+b <= degree, grouped by total degree.
// count = (degree+1)(degree+2)/2. design matrix and evaluate() share this order.
static std::vector<std::pair<int, int>> makeTerms(const int degree) {
    std::vector<std::pair<int, int>> terms;
    for (int total = 0; total <= degree; ++total)
        for (int a = total; a >= 0; --a)
            terms.emplace_back(a, total - a);
    return terms;
}

float CgMlsSurface::evaluate(const float u, const float v) const {
    float height = 0.0f;
    for (size_t k = 0; k < coefficients.size(); ++k)
        height += coefficients[k] * std::pow(u, terms[k].first) * std::pow(v, terms[k].second);
    return height;
}

glm::vec3 CgMlsSurface::positionAt(const float u, const float v) const {
    return origin + u * tangentU + v * tangentV + evaluate(u, v) * normal;
}

void CgMlsSurface::paramBounds(glm::vec2& outMin, glm::vec2& outMax) const {
    outMin = glm::vec2(std::numeric_limits<float>::max());
    outMax = glm::vec2(-std::numeric_limits<float>::max());
    for (const auto& p: params) {
        outMin = glm::min(outMin, p);
        outMax = glm::max(outMax, p);
    }
}

CgMlsSurface fitMlsSurface(const glm::vec3& center, const glm::vec3& normal,
                           const std::vector<glm::vec3>& samples, const int degree) {
    CgMlsSurface surface;
    surface.origin = center;
    surface.normal = glm::normalize(normal);
    surface.degree = degree;
    surface.terms = makeTerms(degree);

    // tangent frame orthogonal to the normal (same construction as the splat code)
    const glm::vec3 helper = (std::abs(surface.normal.x) < 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    surface.tangentU = glm::normalize(glm::cross(helper, surface.normal));
    surface.tangentV = glm::cross(surface.normal, surface.tangentU);

    const int numSamples = static_cast<int>(samples.size());
    const int numTerms = static_cast<int>(surface.terms.size());
    if (numSamples == 0)
        return surface; // invalid: no coefficients

    // bandwidth for the Gaussian weights = distance of the farthest sample, so
    // the outermost neighbour still contributes (weight e^-1) while close ones dominate
    float maxDistSq = 0.0f;
    for (const auto& s: samples)
        maxDistSq = std::max(maxDistSq, glm::dot(s - center, s - center));
    const float bandwidthSq = std::max(maxDistSq, 1e-12f);

    // build the weighted least-squares system A c = b:
    //   row i  = polynomial basis evaluated at the sample's (u,v), scaled by sqrt(weight)
    //   b(i)   = the sample's height along the normal, scaled by sqrt(weight)
    MatrixXd A(numSamples, numTerms);
    VectorXd b(numSamples);
    surface.params.reserve(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        const glm::vec3 offset = samples[i] - center;
        const float u = glm::dot(offset, surface.tangentU);
        const float v = glm::dot(offset, surface.tangentV);
        const float height = glm::dot(offset, surface.normal);
        surface.params.emplace_back(u, v);

        const float weight = std::exp(-glm::dot(offset, offset) / bandwidthSq);
        const double sqrtW = std::sqrt(weight);

        for (int k = 0; k < numTerms; ++k)
            A(i, k) = sqrtW * std::pow(static_cast<double>(u), surface.terms[k].first)
                            * std::pow(static_cast<double>(v), surface.terms[k].second);
        b(i) = sqrtW * height;
    }

    // least-squares solution via SVD (see calculateSingularValueDecomposition for
    // the underlying Moore-Penrose pseudo-inverse). solve() handles the
    // overdetermined and rank-deficient cases gracefully.
    const JacobiSVD<MatrixXd> svd(A, ComputeThinU | ComputeThinV);
    const VectorXd coeffs = svd.solve(b);

    surface.coefficients.resize(numTerms);
    for (int k = 0; k < numTerms; ++k)
        surface.coefficients[k] = static_cast<float>(coeffs(k));

    return surface;
}
