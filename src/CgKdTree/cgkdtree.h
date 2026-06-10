#ifndef CG_KD_TREE_H
#define CG_KD_TREE_H

#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include "cgkdmediansplit.h"
#include "cgkdaxisstrategy.h"
#include "cgaabb.h"

class CgPointCloud;
class CgKdNode;

struct CgKdSplitRect {
    glm::vec3 corners[4];
    int depth;
};

class CgKdTree
{
public:
    explicit CgKdTree(const CgPointCloud& cloud,
                      CgKdSplitStrategy* strategy = nullptr,
                      CgKdAxisStrategy* axisStrategy = nullptr);
    ~CgKdTree();

    [[nodiscard]] std::vector<int> knn(int queryIdx, int k) const;
    [[nodiscard]] std::vector<CgKdSplitRect> getSplitPlaneRects(const CgAABB& rootBounds) const;

    // index of the point with the smallest perpendicular distance to the line
    // (origin + t*dir, t unrestricted), found via branch-and-bound over the tree.
    // returns -1 if no point lies within maxDistance of the line.
    // rootBounds is the AABB enclosing all points (the cloud tracks it)
    [[nodiscard]] int closestToLine(const glm::vec3& origin, const glm::vec3& dir,
                                    const CgAABB& rootBounds, double maxDistance) const;

private:
    using KnnPQ = std::priority_queue<std::pair<float, int>>;

    CgKdNode* build(const std::vector<int>& indices, int depth);

    static void collectSplitRects(const CgKdNode* node, const CgAABB &bounds, int depth, std::vector<CgKdSplitRect>& result);

    void addToPQ(int idx, int queryIdx, int k, KnnPQ& pq) const;
    void searchNode(const CgKdNode* node, int queryIdx, int k, KnnPQ& pq) const;
    void searchSubtree(const CgKdNode* node, int queryIdx, int k, KnnPQ& pq) const;
    [[nodiscard]] float distToSplitPlane(const CgKdNode* node, int queryIdx) const;

    void searchClosestToLine(const CgKdNode* node, const CgAABB& bounds,
                             const glm::vec3& origin, const glm::vec3& dir,
                             float& bestDistSq, int& bestIdx) const;

    CgKdNode* m_root;
    const std::vector<glm::vec3>& m_points;
    CgKdSplitStrategy* m_strategy;
    CgKdAxisStrategy* m_axis_strategy;
};

#endif // CG_KD_TREE_H
