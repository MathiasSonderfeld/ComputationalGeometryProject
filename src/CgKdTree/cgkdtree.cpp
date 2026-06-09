#include "cgkdtree.h"
#include "cgkdnode.h"
#include "cgpointcloud.h"

CgKdTree::CgKdTree(const CgPointCloud& cloud, CgKdSplitStrategy* strategy):
    m_root(nullptr),
    m_points(cloud.getVertices()),
    m_default_strategy(),
    m_strategy(strategy ? strategy : &m_default_strategy)
{
    std::vector<int> indices(m_points.size());
    for (int i = 0; i < static_cast<int>(m_points.size()); ++i)
        indices[i] = i;
    m_root = build(std::move(indices), 0);
}

CgKdTree::~CgKdTree()
{
    delete m_root;
}

CgKdAxis CgKdTree::chooseSplitAxis(const std::vector<int>& indices) const
{
    glm::vec3 minPt = m_points[indices[0]];
    glm::vec3 maxPt = m_points[indices[0]];
    for (const int i : indices) {
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

CgKdNode* CgKdTree::build(const std::vector<int> &indices, const int depth)
{
    if (indices.empty()) return nullptr;
    if (indices.size() == 1) return new CgKdNode(indices[0]);

    CgKdAxis axis = chooseSplitAxis(indices);
    const float splitValue = m_strategy->computeSplitValue(m_points, indices, axis);

    const int axisIdx = static_cast<int>(axis);
    std::vector<int> left, right, onPlane;
    for (int i : indices) {
        float coord = m_points[i][axisIdx];
        if (coord < splitValue)
            left.push_back(i);
        else if (coord > splitValue)
            right.push_back(i);
        else
            onPlane.push_back(i);
    }

    CgKdNode* node = new CgKdNode(axis, splitValue, std::move(onPlane));
    node->setLeft(build(left,  depth + 1));
    node->setRight(build(right, depth + 1));
    return node;
}
