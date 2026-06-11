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

// the polynomial and all its partials up to second order, evaluated at (u,v).
// negative exponents are guarded so a term never contributes to a derivative it
// has differentiated away (e.g. the constant term drops out of every derivative)
namespace {
struct PolyDerivs { float f, fu, fv, fuu, fuv, fvv; };

PolyDerivs evalDerivs(const CgMlsSurface& s, const float u, const float v) {
    PolyDerivs d{0, 0, 0, 0, 0, 0};
    for (size_t k = 0; k < s.coefficients.size(); ++k) {
        const int a = s.terms[k].first;
        const int b = s.terms[k].second;
        const float c = s.coefficients[k];
        const float ua = std::pow(u, a);
        const float vb = std::pow(v, b);

        d.f += c * ua * vb;
        if (a >= 1) d.fu  += c * a * std::pow(u, a - 1) * vb;
        if (b >= 1) d.fv  += c * b * ua * std::pow(v, b - 1);
        if (a >= 2) d.fuu += c * a * (a - 1) * std::pow(u, a - 2) * vb;
        if (a >= 1 && b >= 1) d.fuv += c * a * b * std::pow(u, a - 1) * std::pow(v, b - 1);
        if (b >= 2) d.fvv += c * b * (b - 1) * ua * std::pow(v, b - 2);
    }
    return d;
}
}

glm::vec3 CgMlsSurface::projectOrthogonal() const {
    if (!valid())
        return origin;

    // give up once Newton leaves the sample hull (plus a margin) — outside it the
    // polynomial extrapolates wildly and the foot point is meaningless
    glm::vec2 boundsMin, boundsMax;
    paramBounds(boundsMin, boundsMax);
    const glm::vec2 slack = 0.5f * (boundsMax - boundsMin) + glm::vec2(1e-4f);
    const glm::vec2 lo = boundsMin - slack;
    const glm::vec2 hi = boundsMax + slack;

    // start at the vertical projection's parameter (origin sits at (0,0))
    float u = 0.0f, v = 0.0f;
    constexpr int maxIters = 20;
    constexpr float tol = 1e-7f;
    bool converged = false;

    for (int it = 0; it < maxIters; ++it) {
        const PolyDerivs d = evalDerivs(*this, u, v);

        // residual of the orthogonality system (local frame is orthonormal, so
        // the connection vector is (u, v, P) and the tangents are (1,0,Pu),(0,1,Pv))
        const float g1 = u + d.f * d.fu;
        const float g2 = v + d.f * d.fv;
        if (g1 * g1 + g2 * g2 < tol * tol) {
            converged = true;
            break;
        }

        // symmetric 2x2 Jacobian J of (g1,g2) w.r.t. (u,v)
        const float j00 = 1.0f + d.fu * d.fu + d.f * d.fuu;
        const float j01 = d.fu * d.fv + d.f * d.fuv;
        const float j11 = 1.0f + d.fv * d.fv + d.f * d.fvv;
        const float det = j00 * j11 - j01 * j01;
        if (std::abs(det) < 1e-12f)
            return positionAt(0.0f, 0.0f); // singular Jacobian -> vertical fallback

        // Newton step [u,v] -= J^-1 [g1,g2]  (2x2 inverse via the determinant)
        u -= (j11 * g1 - j01 * g2) / det;
        v -= (j00 * g2 - j01 * g1) / det;

        if (u < lo.x || u > hi.x || v < lo.y || v > hi.y)
            return positionAt(0.0f, 0.0f); // wandered out of the sample region
    }

    return converged ? positionAt(u, v) : positionAt(0.0f, 0.0f);
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
