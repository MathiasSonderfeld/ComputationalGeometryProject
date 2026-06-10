#include "cgkdtree.h"
#include "cgkdnode.h"
#include "cgpointcloud.h"
#include <algorithm>
#include <cmath>

CgKdTree::CgKdTree(const CgPointCloud &cloud, CgKdSplitStrategy *strategy) : m_root(nullptr),
                                                                             m_points(cloud.getVertices()),
                                                                             m_strategy(strategy
                                                                                 ? strategy
                                                                                 : new CgKdMedianSplit()) {
    std::vector<int> indices(m_points.size());
    for (int i = 0; i < static_cast<int>(m_points.size()); ++i)
        indices[i] = i;
    m_root = build(indices, 0);
}

CgKdTree::~CgKdTree() {
    delete m_root;
    delete m_strategy;
}

CgKdAxis CgKdTree::chooseSplitAxis(const std::vector<int> &indices) const {
    glm::vec3 minPt = m_points[indices[0]];
    glm::vec3 maxPt = m_points[indices[0]];
    for (const int i: indices) {
        minPt = glm::min(minPt, m_points[i]);
        maxPt = glm::max(maxPt, m_points[i]);
    }
    const glm::vec3 extent = maxPt - minPt;
    if (extent.x >= extent.y && extent.x >= extent.z) {
        return CgKdAxis::X;
    }
    if (extent.y >= extent.z) {
        return CgKdAxis::Y;
    }
    return CgKdAxis::Z;
}

CgKdNode *CgKdTree::build(const std::vector<int> &indices, const int depth) {
    if (indices.empty()) return nullptr;

    CgKdAxis axis = chooseSplitAxis(indices);
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
    const CgKdNode *far  = (queryCoord < node->getSplitValue()) ? node->getRight() : node->getLeft();

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
    searchSubtree(current->getLeft(),  queryIdx, k, pq);
    searchSubtree(current->getRight(), queryIdx, k, pq);

    for (int i = static_cast<int>(path.size()) - 2; i >= 0; --i) {
        const CgKdNode *ancestor   = path[i];
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
