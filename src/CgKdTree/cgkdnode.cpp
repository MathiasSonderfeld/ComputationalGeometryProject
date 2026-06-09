#include "cgkdnode.h"

CgKdNode::CgKdNode(const int pointIndex):
    m_point_index(pointIndex),
    m_split_axis(CgKdAxis::X),
    m_split_value(0.0f),
    m_left(nullptr),
    m_right(nullptr){}

CgKdNode::CgKdNode(const CgKdAxis axis, const float splitValue, std::vector<int> onPlaneIndices):
    m_point_index(-1),
    m_split_axis(axis),
    m_split_value(splitValue),
    m_on_plane_indices(std::move(onPlaneIndices)),
    m_left(nullptr),
    m_right(nullptr){}


CgKdNode::~CgKdNode(){
    delete m_left;
    delete m_right;
}
