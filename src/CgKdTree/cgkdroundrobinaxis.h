#ifndef CG_KD_ROUND_ROBIN_AXIS_H
#define CG_KD_ROUND_ROBIN_AXIS_H

#include "cgkdaxisstrategy.h"

// classic KD-tree axis selection: cycles X -> Y -> Z -> X ... by depth
class CgKdRoundRobinAxis : public CgKdAxisStrategy
{
public:
    [[nodiscard]] CgKdAxis chooseAxis(const std::vector<glm::vec3>& points,
                                      const std::vector<int>& indices,
                                      int depth) const override;
};

#endif // CG_KD_ROUND_ROBIN_AXIS_H
