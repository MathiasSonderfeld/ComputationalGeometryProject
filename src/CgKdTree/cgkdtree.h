#ifndef CG_KD_TREE_H
#define CG_KD_TREE_H

#include <vector>
#include <glm/glm.hpp>
#include "cgkdmediansplit.h"

class CgPointCloud;
class CgKdNode;

class CgKdTree
{
public:
    explicit CgKdTree(const CgPointCloud& cloud, CgKdSplitStrategy* strategy = nullptr);
    ~CgKdTree();

private:
    CgKdNode* build(std::vector<int> indices, int depth);
    [[nodiscard]] CgKdAxis chooseSplitAxis(const std::vector<int>& indices) const;

    CgKdNode* m_root;
    std::vector<glm::vec3> m_points;
    CgKdSplitStrategy* m_strategy;
    CgKdMedianSplit m_default_strategy;
};

#endif // CG_KD_TREE_H
