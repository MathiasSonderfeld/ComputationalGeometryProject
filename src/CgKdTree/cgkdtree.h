#ifndef CG_KD_TREE_H
#define CG_KD_TREE_H

#include <queue>
#include <stdexcept>
#include <utility>
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

    [[nodiscard]] std::vector<int> knn(int queryIdx, int k) const;

private:
    using KnnPQ = std::priority_queue<std::pair<float, int>>;

    CgKdNode* build(const std::vector<int>& indices, int depth);
    [[nodiscard]] CgKdAxis chooseSplitAxis(const std::vector<int>& indices) const;

    void addToPQ(int idx, int queryIdx, int k, KnnPQ& pq) const;
    void searchNode(const CgKdNode* node, int queryIdx, int k, KnnPQ& pq) const;
    void searchSubtree(const CgKdNode* node, int queryIdx, int k, KnnPQ& pq) const;
    [[nodiscard]] float distToSplitPlane(const CgKdNode* node, int queryIdx) const;

    CgKdNode* m_root;
    const std::vector<glm::vec3>& m_points;
    CgKdSplitStrategy* m_strategy;
};

#endif // CG_KD_TREE_H
