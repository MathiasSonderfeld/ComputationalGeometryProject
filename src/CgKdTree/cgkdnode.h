#ifndef CG_KD_NODE_H
#define CG_KD_NODE_H

#include <vector>
#include "cgkdaxis.h"

class CgKdNode
{
public:
    explicit CgKdNode(int pointIndex);
    CgKdNode(CgKdAxis axis, float splitValue, std::vector<int> onPlaneIndices = {});
    ~CgKdNode();

    [[nodiscard]] bool isLeaf() const;
    [[nodiscard]] int getPointIndex() const;
    [[nodiscard]] CgKdAxis getSplitAxis() const;
    [[nodiscard]] float getSplitValue() const;
    [[nodiscard]] const std::vector<int>& getOnPlaneIndices() const;
    [[nodiscard]] CgKdNode* getLeft() const;
    [[nodiscard]] CgKdNode* getRight() const;
    void setLeft(CgKdNode* left);
    void setRight(CgKdNode* right);

private:
    int m_point_index;
    CgKdAxis m_split_axis;
    float m_split_value;
    std::vector<int> m_on_plane_indices;
    CgKdNode* m_left;
    CgKdNode* m_right;
};

inline bool     CgKdNode::isLeaf()              const { return m_point_index != -1; }
inline int      CgKdNode::getPointIndex()        const { return m_point_index; }
inline CgKdAxis CgKdNode::getSplitAxis()         const { return m_split_axis; }
inline float    CgKdNode::getSplitValue()        const { return m_split_value; }
inline const std::vector<int>& CgKdNode::getOnPlaneIndices() const { return m_on_plane_indices; }
inline CgKdNode* CgKdNode::getLeft()             const { return m_left; }
inline CgKdNode* CgKdNode::getRight()            const { return m_right; }
inline void CgKdNode::setLeft(CgKdNode* left)          { m_left  = left; }
inline void CgKdNode::setRight(CgKdNode* right)        { m_right = right; }

#endif // CG_KD_NODE_H
