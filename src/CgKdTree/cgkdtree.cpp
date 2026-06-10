#include "cgkdtree.h"
#include "cgkdnode.h"
#include "cgkdroundrobinaxis.h"
#include "cgpointcloud.h"
#include <algorithm>
#include <cmath>

CgKdTree::CgKdTree(const CgPointCloud &cloud, CgKdSplitStrategy *strategy,
                   CgKdAxisStrategy *axisStrategy) : m_root(nullptr),
                                                     m_points(cloud.getVertices()),
                                                     m_strategy(strategy
                                                                    ? strategy
                                                                    : new CgKdMedianSplit()),
                                                     m_axis_strategy(axisStrategy
                                                                         ? axisStrategy
                                                                         : new CgKdRoundRobinAxis()) {
    std::vector<int> indices(m_points.size());
    for (int i = 0; i < static_cast<int>(m_points.size()); ++i)
        indices[i] = i;
    m_root = build(indices, 0);
}

CgKdTree::~CgKdTree() {
    delete m_root;
    delete m_strategy;
    delete m_axis_strategy;
}

CgKdNode *CgKdTree::build(const std::vector<int> &indices, const int depth) {
    if (indices.empty()) return nullptr;

    CgKdAxis axis = m_axis_strategy->chooseAxis(m_points, indices, depth);
    const float splitValue = m_strategy->computeSplitValue(m_points, indices, axis);

    const int axisIdx = static_cast<int>(axis);
    std::vector<int> left, right, onPlane;
    for (int i: indices) {
        const float coord = m_points[i][axisIdx];
        if (coord < splitValue)
            left.push_back(i);
        else if (coord > splitValue)
            right.push_back(i);
        else
            onPlane.push_back(i);
    }

    CgKdNode *node = new CgKdNode(axis, splitValue, std::move(onPlane));
    node->setLeft(build(left, depth + 1));
    node->setRight(build(right, depth + 1));
    return node;
}


void CgKdTree::addToPQ(int idx, int queryIdx, int k, KnnPQ &pq) const {
    if (idx == queryIdx) return;
    float dist = glm::distance(m_points[idx], m_points[queryIdx]);
    if (static_cast<int>(pq.size()) < k) {
        pq.emplace(dist, idx);
    } else if (dist < pq.top().first) {
        pq.pop();
        pq.emplace(dist, idx);
    }
}

void CgKdTree::searchNode(const CgKdNode *node, const int queryIdx, const int k, KnnPQ &pq) const {
    if (!node) return;
    for (const int idx: node->getOnPlaneIndices())
        addToPQ(idx, queryIdx, k, pq);
}

void CgKdTree::searchSubtree(const CgKdNode *node, const int queryIdx, const int k, KnnPQ &pq) const {
    if (!node) return;

    for (const int idx: node->getOnPlaneIndices())
        addToPQ(idx, queryIdx, k, pq);

    if (node->isLeaf()) return;

    const int axisIdx = static_cast<int>(node->getSplitAxis());
    const float queryCoord = m_points[queryIdx][axisIdx];
    const CgKdNode *near = (queryCoord < node->getSplitValue()) ? node->getLeft() : node->getRight();
    const CgKdNode *far = (queryCoord < node->getSplitValue()) ? node->getRight() : node->getLeft();

    searchSubtree(near, queryIdx, k, pq);

    const float planeDist = distToSplitPlane(node, queryIdx);
    if (static_cast<int>(pq.size()) < k || planeDist <= pq.top().first)
        searchSubtree(far, queryIdx, k, pq);
}

float CgKdTree::distToSplitPlane(const CgKdNode *node, int queryIdx) const {
    const int axisIdx = static_cast<int>(node->getSplitAxis());
    return std::abs(m_points[queryIdx][axisIdx] - node->getSplitValue());
}

std::vector<int> CgKdTree::knn(const int queryIdx, const int k) const {
    std::vector<const CgKdNode *> path;
    const CgKdNode *current = m_root;
    bool found = false;

    while (current != nullptr) {
        path.push_back(current);

        for (const int idx: current->getOnPlaneIndices()) {
            if (idx == queryIdx) {
                found = true;
                break;
            }
        }
        if (found) break;

        if (current->isLeaf()) break;

        const int axisIdx = static_cast<int>(current->getSplitAxis());
        current = (m_points[queryIdx][axisIdx] < current->getSplitValue())
                      ? current->getLeft()
                      : current->getRight();
    }

    if (current == nullptr)
        throw std::runtime_error("CgKdTree::knn: query point not found in tree");

    KnnPQ pq;

    searchNode(current, queryIdx, k, pq);
    searchSubtree(current->getLeft(), queryIdx, k, pq);
    searchSubtree(current->getRight(), queryIdx, k, pq);

    for (int i = static_cast<int>(path.size()) - 2; i >= 0; --i) {
        const CgKdNode *ancestor = path[i];
        const CgKdNode *nextInPath = path[i + 1];
        const CgKdNode *other = (ancestor->getLeft() == nextInPath)
                                    ? ancestor->getRight()
                                    : ancestor->getLeft();

        searchNode(ancestor, queryIdx, k, pq);

        const float planeDist = distToSplitPlane(ancestor, queryIdx);
        if (static_cast<int>(pq.size()) < k || planeDist <= pq.top().first)
            searchSubtree(other, queryIdx, k, pq);
    }

    std::vector<int> result;
    result.reserve(pq.size());
    while (!pq.empty()) {
        result.push_back(pq.top().second);
        pq.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

void CgKdTree::collectSplitRects(const CgKdNode *node, const CgAABB &bounds, const int depth,
                                 std::vector<CgKdSplitRect> &result) {
    if (!node || node->isLeaf()) return;

    const int axisIdx = static_cast<int>(node->getSplitAxis());
    const float sv = node->getSplitValue();

    CgKdSplitRect rect{};
    rect.depth = depth;
    switch (node->getSplitAxis()) {
        case CgKdAxis::X:
            rect.corners[0] = glm::vec3(sv, bounds.min.y, bounds.min.z);
            rect.corners[1] = glm::vec3(sv, bounds.max.y, bounds.min.z);
            rect.corners[2] = glm::vec3(sv, bounds.max.y, bounds.max.z);
            rect.corners[3] = glm::vec3(sv, bounds.min.y, bounds.max.z);
            break;
        case CgKdAxis::Y:
            rect.corners[0] = glm::vec3(bounds.min.x, sv, bounds.min.z);
            rect.corners[1] = glm::vec3(bounds.max.x, sv, bounds.min.z);
            rect.corners[2] = glm::vec3(bounds.max.x, sv, bounds.max.z);
            rect.corners[3] = glm::vec3(bounds.min.x, sv, bounds.max.z);
            break;
        case CgKdAxis::Z:
            rect.corners[0] = glm::vec3(bounds.min.x, bounds.min.y, sv);
            rect.corners[1] = glm::vec3(bounds.max.x, bounds.min.y, sv);
            rect.corners[2] = glm::vec3(bounds.max.x, bounds.max.y, sv);
            rect.corners[3] = glm::vec3(bounds.min.x, bounds.max.y, sv);
            break;
    }
    result.push_back(rect);

    CgAABB leftBounds = bounds;
    leftBounds.max[axisIdx] = sv;
    CgAABB rightBounds = bounds;
    rightBounds.min[axisIdx] = sv;

    collectSplitRects(node->getLeft(), leftBounds, depth + 1, result);
    collectSplitRects(node->getRight(), rightBounds, depth + 1, result);
}

std::vector<CgKdSplitRect> CgKdTree::getSplitPlaneRects(const CgAABB &rootBounds) const {
    std::vector<CgKdSplitRect> result;
    collectSplitRects(m_root, rootBounds, 0, result);
    return result;
}

static float perpendicularDistanceSquaredToLine(const glm::vec3 &p, const glm::vec3 &origin, const glm::vec3 &dir) {
    const glm::vec3 toPoint = p - origin;                         // anchor -> point (hypotenuse)
    const float projectionLength = glm::dot(toPoint, dir);        // its shadow along the line
    // Pythagoras: |toPoint|^2 - projection^2 = perpendicular^2
    return glm::dot(toPoint, toPoint) - projectionLength * projectionLength;
}

static float closestPossibleDistanceToLine(const CgAABB &bounds, const glm::vec3 &origin, const glm::vec3 &dir) {
    const glm::vec3 center = 0.5f * (bounds.min + bounds.max);
    const float radius = 0.5f * glm::length(bounds.max - bounds.min);
    const float centerDist = std::sqrt(std::max(0.0f, perpendicularDistanceSquaredToLine(center, origin, dir)));
    return std::max(0.0f, centerDist - radius);
}

void CgKdTree::searchClosestToLine(const CgKdNode *node, const CgAABB &bounds, const glm::vec3 &origin,
                                   const glm::vec3 &dir, float &bestDistSq, int &bestIdx) const {
    if (!node) return;

    // evaluate this node's own points against the line
    for (const int idx: node->getOnPlaneIndices()) {
        const float distSq = perpendicularDistanceSquaredToLine(m_points[idx], origin, dir);
        if (distSq < bestDistSq) {
            bestDistSq = distSq;
            bestIdx = idx;
        }
    }

    if (node->isLeaf()) return;

    const int axisIdx = static_cast<int>(node->getSplitAxis());
    const float sv = node->getSplitValue();
    CgAABB leftBounds = bounds;
    leftBounds.max[axisIdx] = sv;
    CgAABB rightBounds = bounds;
    rightBounds.min[axisIdx] = sv;

    const float leftBound = closestPossibleDistanceToLine(leftBounds, origin, dir);
    const float rightBound = closestPossibleDistanceToLine(rightBounds, origin, dir);

    // visit the nearer child first (tightens bestDist early), then the farther
    // one only if it could still hold something closer than the current best
    if (leftBound <= rightBound) {
        if (leftBound * leftBound < bestDistSq)
            searchClosestToLine(node->getLeft(), leftBounds, origin, dir, bestDistSq, bestIdx);
        if (rightBound * rightBound < bestDistSq)
            searchClosestToLine(node->getRight(), rightBounds, origin, dir, bestDistSq, bestIdx);
    } else {
        if (rightBound * rightBound < bestDistSq)
            searchClosestToLine(node->getRight(), rightBounds, origin, dir, bestDistSq, bestIdx);
        if (leftBound * leftBound < bestDistSq)
            searchClosestToLine(node->getLeft(), leftBounds, origin, dir, bestDistSq, bestIdx);
    }
}

int CgKdTree::closestToLine(const glm::vec3 &origin, const glm::vec3 &dir,
                            const CgAABB &rootBounds, const double maxDistance) const {
    if (!m_root) return -1;

    const glm::vec3 unitDir = glm::normalize(dir);
    float bestDistSq = static_cast<float>(maxDistance * maxDistance);
    int bestIdx = -1;

    searchClosestToLine(m_root, rootBounds, origin, unitDir, bestDistSq, bestIdx);
    return bestIdx;
}
