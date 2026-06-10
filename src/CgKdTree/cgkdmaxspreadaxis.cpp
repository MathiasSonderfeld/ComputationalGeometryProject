#include "cgkdmaxspreadaxis.h"

CgKdAxis CgKdMaxSpreadAxis::chooseAxis(const std::vector<glm::vec3>& points,
                                       const std::vector<int>& indices,
                                       const int /*depth*/) const {
    glm::vec3 minPt = points[indices[0]];
    glm::vec3 maxPt = points[indices[0]];
    for (const int i : indices) {
        minPt = glm::min(minPt, points[i]);
        maxPt = glm::max(maxPt, points[i]);
    }
    const glm::vec3 extent = maxPt - minPt;
    if (extent.x >= extent.y && extent.x >= extent.z) {
        return CgKdAxis::X;
    }
    if (extent.y >= extent.z) {
        return CgKdAxis::Y;
    }
    return CgKdAxis::Z;
}
