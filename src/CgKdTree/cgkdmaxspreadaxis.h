#ifndef CG_KD_MAX_SPREAD_AXIS_H
#define CG_KD_MAX_SPREAD_AXIS_H

#include "cgkdaxisstrategy.h"

// adaptive axis selection: picks the axis with the largest point spread
// (extent) at each node. produces more cube-like cells, which tends to
// give better nearest-neighbour query performance. may pick the same axis
// on consecutive levels
class CgKdMaxSpreadAxis : public CgKdAxisStrategy
{
public:
    [[nodiscard]] CgKdAxis chooseAxis(const std::vector<glm::vec3>& points,
                                      const std::vector<int>& indices,
                                      int depth) const override;
};

#endif // CG_KD_MAX_SPREAD_AXIS_H
