#ifndef CG_POINT_CLOUD_H
#define CG_POINT_CLOUD_H

#include <vector>
#include <glm/glm.hpp>
#include "cgbasepointcloud.h"

class CgTriangleMesh;

class CgPointCloud : public CgBasePointCloud
{
public:
    // constructors and destructor
    CgPointCloud();
    explicit CgPointCloud(std::vector<glm::vec3>& vertices);
    ~CgPointCloud() override;

    //inherited from CgBaseRenderableObject
    ObjectType getType() const override;
    unsigned int getID() const override;
    glm::vec3 getColor() const override;

    //inherited from CgBasePointCloud
    const std::vector<glm::vec3>& getVertices() const override;
    const std::vector<glm::vec3>& getVertexNormals() const override;
    const std::vector<glm::vec3>& getVertexColors() const override;

    // given a ray with origin and dir: get closest Point
    // to be implemented by students
    const glm::vec3 getClosestPoint(const glm::vec3& origin, const glm::vec3& dir, double maxDistance) const override;

    // for splatting two axes for the ellipse are needed for each vertex
    const std::vector<glm::vec3>& getSplatDirX() const override;
    const std::vector<glm::vec3>& getSplatDirY() const override;

    // naive O(n) k-nearest neighbours — iterates all vertices, uses a max-heap of size k
    std::vector<int> kNearestNeighboursSimple(const glm::vec3& queryPoint, int k) const;

    // PCA normal estimation: fits a local plane to m_k nearest neighbours of each vertex
    void calculateNormals();

    // consistent normal orientation via MST propagation (Hoppe et al. 1992)
    // requires calculateNormals() to have been called first
    void orientNormals();

    int getK() const { return m_k; }
    void setK(int k) { m_k = k; }

    // generates a triangle mesh with one disc splat per vertex, oriented along the vertex normal
    CgTriangleMesh* generateSplatMesh(float radius, int segments = 8) const;

protected:
    // store point data given by constructor
    std::vector<glm::vec3> m_vertices;

    // not set by constructor, assume point data only.
    // to be constructed by students during analysis of point cloud
    std::vector<glm::vec3> m_vertex_normals;
    std::vector<glm::vec3> m_splat_dir_x;
    std::vector<glm::vec3> m_splat_dir_y;

    // vertex colors can be used to mark certain regions (e.g. nearest neighbors)
    // by a different color, to be constructed by students
    mutable std::vector<glm::vec3> m_vertex_colors;

    int m_k{};

    // store id and type as provided by CgBaseRenderableObject
    const ObjectType m_type;
    const unsigned int m_id;

    // if no material is used (lighting off) and no per vertex color available use this color for complete object
    glm::vec3 m_color;
};

// inline for simple getter methods
inline ObjectType CgPointCloud::getType() const { return m_type; }
inline unsigned int CgPointCloud::getID() const { return m_id; }
inline glm::vec3 CgPointCloud::getColor() const { return m_color; }

#endif // CG_POINT_CLOUD_H
