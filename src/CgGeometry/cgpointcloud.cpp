#include "cgpointcloud.h"
#include "cgtrianglemesh.h"
#include "../CgMath/cgeigendecomposition3x3.h"
#include <queue>
#include <tuple>
#include <functional>
#include <cmath>
#include <utility>

extern int getUniqueId();

CgPointCloud::CgPointCloud() : m_type(PointCloud),
                               m_id(getUniqueId()),
                               m_color(glm::vec3(0.0, 1.0, 0.0)) {
}

CgPointCloud::CgPointCloud(std::vector<glm::vec3> &vertices) : m_type(PointCloud),
                                                               m_id(getUniqueId()),
                                                               m_color(glm::vec3(0.0, 1.0, 0.0)) {
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();

    for (auto &vert: vertices) {
        m_vertices.emplace_back(vert);

        // for testing purposes
        // comment if not desired
        if (vert.x > 0)
            m_vertex_colors.emplace_back(0.0, 1.0, 0.0);
        else
            m_vertex_colors.emplace_back(0.0, 0.0, 1.0);
    }

    m_k = std::max(5, static_cast<int>(std::sqrt(static_cast<float>(m_vertices.size()))));
    calculateNormals();
}

CgPointCloud::~CgPointCloud() {
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_splat_dir_x.clear();
    m_splat_dir_y.clear();
}


const std::vector<glm::vec3> &CgPointCloud::getVertices() const {
    return m_vertices;
}

const std::vector<glm::vec3> &CgPointCloud::getVertexNormals() const {
    return m_vertex_normals;
}

const std::vector<glm::vec3> &CgPointCloud::getVertexColors() const {
    return m_vertex_colors;
}


const std::vector<glm::vec3> &CgPointCloud::getSplatDirX() const {
    return m_splat_dir_x;
}

const std::vector<glm::vec3> &CgPointCloud::getSplatDirY() const {
    return m_splat_dir_y;
}

const glm::vec3 CgPointCloud::getClosestPoint(const glm::vec3 &origin, const glm::vec3 &dir,
                                              const double maxDistance) const {
    // to be implemented
    return glm::vec3(0.0, 0.0, 0.0);
}

void CgPointCloud::calculateNormals() {
    int n = static_cast<int>(m_vertices.size());
    m_vertex_normals.resize(n);

    for (int i = 0; i < n; ++i) {
        std::vector<int> neighbors = kNearestNeighboursSimple(m_vertices[i], m_k);

        // centroid of the local neighbourhood
        glm::vec3 centroid(0.0f);
        for (int idx: neighbors)
            centroid += m_vertices[idx];
        centroid /= static_cast<float>(neighbors.size());

        // covariance matrix: C = Σ (p - centroid) ⊗ (p - centroid)
        // outerProduct(d,d)[col][row] = d[row]*d[col], which is the correct GLM layout
        glm::mat3 cov(0.0f);
        for (int idx: neighbors) {
            glm::vec3 d = m_vertices[idx] - centroid;
            cov += glm::outerProduct(d, d);
        }

        // eigendecomposition — eigenvector with smallest eigenvalue = surface normal
        CgEigenDecomposition3x3 ed(cov);
        glm::vec3 eigenvalues = ed.getEigenvalues();
        glm::mat3 eigenvectors = ed.getEigenvectors();

        int minIdx = 0;
        if (eigenvalues[1] < eigenvalues[minIdx]) minIdx = 1;
        if (eigenvalues[2] < eigenvalues[minIdx]) minIdx = 2;

        // due to the GLM/Eigen index mapping in CgEigenDecomposition3x3,
        // eigenvector k is stored as row k of the GLM matrix: eigenvectors[col][k]
        glm::vec3 normal(eigenvectors[0][minIdx], eigenvectors[1][minIdx], eigenvectors[2][minIdx]);
        m_vertex_normals[i] = glm::normalize(normal);
    }

    orientNormals();
}

void CgPointCloud::orientNormals() {
    int n = static_cast<int>(m_vertices.size());
    if (static_cast<int>(m_vertex_normals.size()) != n)
        return; // calculateNormals() must be called first

    // 1. seed: vertex with maximum z — we know its normal should point in +z direction
    int seed = 0;
    for (int i = 1; i < n; ++i)
        if (m_vertices[i].z > m_vertices[seed].z)
            seed = i;

    if (m_vertex_normals[seed].z < 0.0f)
        m_vertex_normals[seed] *= -1.0f;

    // 2. build undirected adjacency list from KNN
    // edge weight = 1 - |dot(ni, nj)|: low cost when normals are nearly parallel
    std::vector<std::vector<std::pair<int, float> > > adjacency_list(n);
    for (int i = 0; i < n; ++i) {
        std::vector<int> neighbors = kNearestNeighboursSimple(m_vertices[i], m_k);
        for (int j: neighbors) {
            if (j == i) continue;
            float weight = 1.0f - std::abs(glm::dot(m_vertex_normals[i], m_vertex_normals[j]));
            adjacency_list[i].emplace_back(j, weight);
            adjacency_list[j].emplace_back(i, weight);
        }
    }

    // 3. Prim's MST — min-heap on (weight, to, from)
    using vertex_link = std::tuple<float, int, int>;
    std::priority_queue<vertex_link, std::vector<vertex_link>, std::greater<>> priority_queue;

    std::vector<bool> inMST(n, false);
    std::vector<int> parents(n, -1);

    priority_queue.emplace(0.0f, seed, -1);

    while (!priority_queue.empty()) {
        auto [weight, to, from] = priority_queue.top();
        priority_queue.pop();
        if (inMST[to]) continue;
        inMST[to] = true;
        parents[to] = from;

        for (auto [v, wv]: adjacency_list[to])
            if (!inMST[v])
                priority_queue.emplace(wv, v, to);
    }

    // 4. BFS over MST: flip child normal if it points away from parent normal
    std::vector<std::vector<int> > children(n);
    for (int i = 0; i < n; ++i)
        if (parents[i] != -1)
            children[parents[i]].push_back(i);

    std::queue<int> bfsQueue;
    bfsQueue.push(seed);

    while (!bfsQueue.empty()) {
        int current = bfsQueue.front();
        bfsQueue.pop();
        for (int child: children[current]) {
            if (glm::dot(m_vertex_normals[current], m_vertex_normals[child]) < 0.0f)
                m_vertex_normals[child] *= -1.0f;
            bfsQueue.push(child);
        }
    }
}

std::vector<int> CgPointCloud::kNearestNeighboursSimple(const glm::vec3 &queryPoint, int k) const {
    // max-heap: (squared_distance, index) — der bisher weiteste Kandidat liegt oben
    std::priority_queue<std::pair<float, int> > queue;

    for (int i = 0; i < static_cast<int>(m_vertices.size()); ++i) {
        glm::vec3 diff = m_vertices[i] - queryPoint;
        float distSq = glm::dot(diff, diff);

        queue.emplace(distSq, i);
        if (static_cast<int>(queue.size()) > k)
            queue.pop(); // entfernt den bisher weitesten Punkt
    }

    std::vector<int> result;
    result.reserve(queue.size());
    while (!queue.empty()) {
        result.push_back(queue.top().second);
        queue.pop();
    }
    return result;
}

CgTriangleMesh* CgPointCloud::generateSplatMesh(const float radius, int segments) const
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> norms;
    std::vector<unsigned int> idx;

    const int n = static_cast<int>(m_vertices.size());
    vertices.reserve(n * (segments + 1));
    norms.reserve(n * (segments + 1));
    idx.reserve(n * segments * 3);

    constexpr float two_pi = M_PI * 2.0f;
    constexpr glm::vec3 fallback_normal(0.0f, 1.0f, 0.0f);

    for (int i = 0; i < n; ++i)
    {
        const glm::vec3& p = m_vertices[i];
        const glm::vec3& normal = i < m_vertex_normals.size() ? m_vertex_normals[i] : fallback_normal;

        // tangent frame via Gram-Schmidt
        glm::vec3 helper = (std::abs(normal.x) < 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        glm::vec3 t1 = glm::normalize(glm::cross(helper, normal));
        glm::vec3 t2 = glm::cross(normal, t1);

        unsigned int base = static_cast<unsigned int>(vertices.size());

        // center vertex
        vertices.push_back(p);
        norms.push_back(normal);

        // perimeter vertices
        for (int j = 0; j < segments; ++j)
        {
            const float angle = two_pi * j / segments;
            vertices.push_back(p + radius * (std::cos(angle) * t1 + std::sin(angle) * t2));
            norms.push_back(normal);
        }

        // fan triangles: (center, v_j, v_{j+1 mod segments})
        for (int j = 0; j < segments; ++j)
        {
            idx.push_back(base);
            idx.push_back(base + 1 + j);
            idx.push_back(base + 1 + (j + 1) % segments);
        }
    }

    return new CgTriangleMesh(vertices, norms, idx);
}
