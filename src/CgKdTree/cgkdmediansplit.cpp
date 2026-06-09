#include "cgkdmediansplit.h"
#include <algorithm>

float CgKdMedianSplit::computeSplitValue(const std::vector<glm::vec3>& points, const std::vector<int>& indices, CgKdAxis axis) const{
    const int axisIdx = static_cast<int>(axis);
    std::vector<int> sorted = indices;
    std::sort(sorted.begin(), sorted.end(), [&](const int a, const int b) {
        return points[a][axisIdx] < points[b][axisIdx];
    });
    const size_t n = sorted.size();
    if (n % 2 == 0) {
        return (points[sorted[n / 2 - 1]][axisIdx] + points[sorted[n / 2]][axisIdx]) / 2.0f;
    }
    return points[sorted[n / 2]][axisIdx];
}
