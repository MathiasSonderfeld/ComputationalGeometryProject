#include "cgkdnode.h"

CgKdNode::CgKdNode(CgKdAxis axis, float splitValue, std::vector<int> indices)
    : m_split_axis(axis)
    , m_split_value(splitValue)
    , m_on_plane_indices(std::move(indices))
    , m_left(nullptr)
    , m_right(nullptr)
{}

CgKdNode::~CgKdNode()
{
    delete m_left;
    delete m_right;
}
