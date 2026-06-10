#ifndef CG_KD_AXIS_STRATEGY_H
#define CG_KD_AXIS_STRATEGY_H

#include <vector>
#include <glm/glm.hpp>
#include "cgkdaxis.h"

// strategy for choosing which axis a node splits along.
// receives the node's points, their indices and the node depth;
// concrete strategies use whichever of these they need
class CgKdAxisStrategy
{
public:
    virtual ~CgKdAxisStrategy() = default;
    [[nodiscard]] virtual CgKdAxis chooseAxis(const std::vector<glm::vec3>& points,
                                              const std::vector<int>& indices,
                                              int depth) const = 0;
};

#endif // CG_KD_AXIS_STRATEGY_H
