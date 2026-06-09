#include "cgkdmeansplit.h"

float CgKdMeanSplit::computeSplitValue(const std::vector<glm::vec3>& points, const std::vector<int>& indices, CgKdAxis axis) const{
    const int axisIdx = static_cast<int>(axis);
    float sum = 0.0f;
    for (const int i : indices)
        sum += points[i][axisIdx];
    return sum / static_cast<float>(indices.size());
}
