#ifndef CG_KD_MEDIAN_SPLIT_H
#define CG_KD_MEDIAN_SPLIT_H

#include "cgkdsplitstrategy.h"

class CgKdMedianSplit : public CgKdSplitStrategy
{
public:
    [[nodiscard]] float computeSplitValue(const std::vector<glm::vec3>& points,
                            const std::vector<int>& indices,
                            CgKdAxis axis) const override;
};

#endif // CG_KD_MEDIAN_SPLIT_H
