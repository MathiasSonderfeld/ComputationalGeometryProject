#include "cgkdroundrobinaxis.h"

CgKdAxis CgKdRoundRobinAxis::chooseAxis(const std::vector<glm::vec3>& /*points*/,
                                        const std::vector<int>& /*indices*/,
                                        const int depth) const {
    switch (depth % 3) {
        case 0:  return CgKdAxis::X;
        case 1:  return CgKdAxis::Y;
        default: return CgKdAxis::Z;
    }
}
