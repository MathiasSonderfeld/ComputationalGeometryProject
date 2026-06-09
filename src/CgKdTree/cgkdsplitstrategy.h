#ifndef CG_KD_SPLIT_STRATEGY_H
#define CG_KD_SPLIT_STRATEGY_H

#include <vector>
#include <glm/glm.hpp>
#include "cgkdaxis.h"

class CgKdSplitStrategy
{
public:
    virtual ~CgKdSplitStrategy() = default;
    [[nodiscard]] virtual float computeSplitValue(const std::vector<glm::vec3>& points, const std::vector<int>& indices, CgKdAxis axis) const = 0;
};

#endif // CG_KD_SPLIT_STRATEGY_H
