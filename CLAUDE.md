# CgToolkit — Agent Context

## What this project is

An OpenGL-based computational geometry toolkit written in C++11, built as an educational framework for students. The codebase is deliberately scaffolded: there is more infrastructure than active algorithm, and many stubs exist for exercises that haven't been implemented yet. This is intentional — the expected development direction is **point cloud processing** (normal estimation, PCA, proximity queries, splat rendering).

## Build system

CMake 3.27+, outputs to `build/`. Main executable: `CgToolkit`. All dependencies are bundled under `src/Dependencies/` (Eigen, GLM, Dear ImGui, GLEW, tiny_obj_loader, stb_image). GLFW3 is system-linked on macOS/Linux, bundled on Windows.

---

## Architecture overview

### Layer map

```
main.cpp → CgGlfwWindow → CgOpenGLRenderingGui → CgFrameBufferShader
                                ↓
                      m_renderable_objects[]
                      (CgBaseRenderableObject*)
                                ↓
         CgPointCloud | CgTriangleMesh | CgHalfEdgeTriangleMesh | ...
```

### Abstract base class hierarchy (`src/CgBase/`)

All renderable objects extend `CgBaseRenderableObject`, which carries a type enum:

| Enum value | Concrete class |
|---|---|
| `CG_POINT_CLOUD` | `CgPointCloud` |
| `CG_POINT_LIST` | `CgPointList` |
| `CG_TRIANGLE_MESH` | `CgTriangleMesh` |
| `CG_HALF_EDGE_TRIANGLE_MESH` | `CgHalfEdgeTriangleMesh` |
| `CG_TRIANGLE_FAN` | `CgTriangleFan` |

The renderer in `CgFrameBufferShader` dispatches on `getType()` to load the correct geometry into VAO/VBO/IBO.

### Half-edge mesh (`src/CgGeometry/cghalfedgeprimitives.h/.cpp`, `cghalfedgetrianglemesh.h/.cpp`)

The most developed algorithmic component. Three classes: `CgHeVert`, `CgHeEdge`, `CgHeFace`. The mesh builds its half-edge structure from an indexed triangle list via an `unordered_map<pair<int,int>, CgHeEdge*>` for edge pairing. A `PairHash` functor is defined for this map.

The mesh validates itself with `consistencyCheck()`, which traverses the full structure and reports the Euler characteristic (V − E + F, should be 2 for a closed manifold).

---

## What is implemented

### Half-edge mesh & Loop subdivision

`CgHalfEdgeTriangleMesh::subdivide()` — full Loop subdivision in three passes:
1. **Edge insertion**: inserts midpoint vertices; interior edge midpoints use the Loop mask `(3(v₀+v₁) + v₂+v₃)/8`, boundary midpoints use `(v₀+v₁)/2`.
2. **Face splitting**: each triangle → 4 triangles (three corner triangles + central).
3. **Vertex update**: interior vertices use `(1 − n·β)·p + β·Σneighbors`, boundary vertices use `¾·p + ⅛·(prev+next)`. β is calculated via `calculateBeta(n)`.

`calculateNormals()` — face normals via cross product; vertex normals by averaging adjacent face normals, with boundary-safe traversal.

`consistencyCheck()` — validates edge pairing, face winding, and Euler characteristic.

### Rendering pipeline

`CgFrameBufferShader` handles the full OpenGL render loop:
- Phong shading (GLSL 130/150, in `shadercodephong.h`)
- Toggle: flat vs smooth shading, lighting on/off, polygon mode (point/line/fill)
- Normal visualization as line segments
- Trackball camera via `CgTrackball` (quaternion-based, mouse drag)
- Picking ray construction on Ctrl+click (world-space ray from viewport coords)

### Ray-sphere intersection

`CgSphere::hasIntersection()` — implemented with quadratic formula. Only sphere has this; mesh intersection is stubbed.

### GUI

Dear ImGui panels in `CgOpenGLRenderingGui`:
- Object list with load/delete/visibility
- Render options (shading, polygon mode, normals, pick ray)
- Triangle fan and control polygon examples (movable sphere control points)

### Eigendecomposition

`CgEigenDecomposition3x3` wraps Eigen's `SelfAdjointEigenSolver` for 3×3 symmetric matrices. Intended for PCA of point clouds — wired into the GUI via `Eigen::JacobiSVD` declaration but not yet called.

---

## What is implemented in CgPointCloud

All major point cloud algorithms are now implemented in `src/CgGeometry/cgpointcloud.cpp`.

### kNearestNeighboursSimple
`std::vector<int> kNearestNeighboursSimple(glm::vec3 queryPoint, int k)`

O(n) scan with a max-heap of size k. Returns indices of the k nearest neighbours. `m_k` is initialised in the constructor as `max(5, sqrt(n))`.

### calculateNormals + orientNormals
`calculateNormals()` performs PCA-based normal estimation per vertex:
1. Find k nearest neighbours via `kNearestNeighboursSimple`
2. Compute covariance matrix: `C = Σ (p - centroid) ⊗ (p - centroid)`
3. Eigendecompose via `CgEigenDecomposition3x3` — eigenvector with **smallest** eigenvalue is the surface normal

`orientNormals()` propagates consistent orientation via an MST (Prim's algorithm):
- Seed: vertex with maximum z, forced to point in +z
- Edge weight: `1 - |dot(nᵢ, nⱼ)|` (low cost = normals nearly parallel)
- BFS over MST, flipping child normals that disagree with their parent

**GLM eigenvector indexing quirk:** `CgEigenDecomposition3x3` stores eigenvector k as row k of the GLM matrix, so eigenvector k is extracted as `(eigenvectors[0][k], eigenvectors[1][k], eigenvectors[2][k])`.

### getClosestPoint
`getClosestPoint(origin, dir, maxDistance)` implements ray-to-point-cloud picking:
- Finds the point with minimum perpendicular distance to the ray
- Rejects if that distance exceeds `maxDistance`
- Sets `m_vertex_colors`: all points green (default), k nearest neighbours of the selected point red
- Returns the selected point position (or `{0,0,0}` on miss)

### generateSplatMesh
`CgTriangleMesh* generateSplatMesh(float radius, int segments = 8)`

Creates one circular disc (triangle fan) per point, oriented along the vertex normal. Tangent frame built as:
```
t1 = normalize(cross(helper, normal))
t2 = cross(normal, t1)
```
Fan winding: `(center, p_j, p_{j+1})` — face normals point in +normal direction.

### regionGrowing
`std::vector<std::vector<int>> regionGrowing(float maxAngleDeg, int minClusterSize = 1)`

Best-first region growing. For each unassigned candidate point, grows a cluster by iterating through its precomputed min-heap PQ until the cluster reaches `minClusterSize` or the PQ is exhausted.

**Precomputation:** For each point i, a per-point min-heap `point_pq[i]` stores `(distSq, j, parent=i)` for the `2 * minClusterSize` nearest neighbours, filtered using a temporary max-heap during construction.

**Growth loop:**
```cpp
while (!point_pq[candidate].empty() && cluster.size() < minClusterSize) {
    pop (distSq, j, parent) from point_pq[candidate]
    if j already assigned: skip
    if angle(normal[parent], normal[j]) <= maxAngleDeg:
        assign j to cluster
}
```

**Orphan assignment:** After the main loop, any unassigned points are assigned to the nearest cluster by centroid distance.

**Key design decisions:**
- Uses `2 * minClusterSize` nearest neighbours (not all n-1) to prevent clusters from grabbing distant points with compatible normals when the nearby ones fail the angle check
- Grows only from `point_pq[candidate]` — no frontier expansion, no merging of PQs. Smaller clusters are acceptable (`minClusterSize` is a target, not a guarantee)
- `parent` in the tuple is always the candidate, so the angle check compares j's normal against the candidate's normal

### generateClusterMesh
`CgTriangleMesh* generateClusterMesh(const std::vector<std::vector<int>>& clusters, int segments = 32)`

Generates one oriented ellipse per cluster via 2D PCA:
1. Compute cluster centroid and mean normal
2. Build tangent frame `(tangent1, tangent2)` orthogonal to mean normal
3. Project all cluster points onto the tangent plane → 2D covariance matrix
4. Analytic 2×2 eigendecomposition → major eigenvector direction
5. `ellipseAxis1` / `ellipseAxis2` = major/minor axes in 3D (rotation of tangent frame)
6. `semiAxisMajor` = max `|dot(p - centroid, ellipseAxis1)|` over all cluster points
7. `semiAxisMinor = semiAxisMajor * sqrt(minorEigenvalue / majorEigenvalue)`
8. Triangle fan: `(center, p_j, p_{j+1})` — same winding as `generateSplatMesh`

Colors are HSV-distributed using golden ratio for perceptual separation.

---

## What is still stubbed

| Location | Method | Notes |
|---|---|---|
| `cgtrianglemesh.cpp` | `CgTriangleMesh::hasIntersection()` | Returns false — no ray-triangle intersection |
| `src/CgImageViewer/` | All five exercise tabs | Skeleton only — parallel educational track, likely unused |

---

## GUI controls for point cloud / region growing (Aufgabe 2 tab)

In `cgopenglrenderinggui.cpp`, the "Aufgabe 2" ImGui tab contains:
- **Splat radius slider** + "Splats berechnen" button → calls `generateSplatMesh(m_splat_radius)` → stored in `m_splat_mesh`
- **Angle slider** (`m_region_growing_angle`) + **Min cluster size slider** (`m_min_cluster_size`, default 10) + "Region Growing berechnen" button → calls `regionGrowing(angle, minClusterSize)` then `generateClusterMesh(clusters)` → stored in `m_cluster_mesh`
- **Radio buttons** `m_splat_display_mode`: 0 = "Pro Punkt" (renders `m_splat_mesh`), 1 = "Pro Cluster" (renders `m_cluster_mesh`)
- **Checkbox "render normals"** — calls `updateRenderNormals(m_point_cloud)` on toggle

Member variables added to `CgOpenGLRenderingGui`: `m_min_cluster_size`, `m_splat_display_mode`.

---

## Camera / zoom controls

In `cgframebuffershader.cpp`:
- **Mouse scroll wheel**: zoom via `m_global_scale_matrix`
- **Option + `+`** (German keyboard: `ImGuiKey_RightBracket`): zoom in 5%
- **Option + `-`** (`ImGuiKey_Minus`): zoom out 5%

---

## Dependencies (all bundled, no install needed)

| Library | Used for |
|---|---|
| Eigen | Eigendecomposition, SVD — in `src/Dependencies/Eigen/` |
| GLM | Vector/matrix math throughout |
| Dear ImGui | All GUI panels |
| GLEW | OpenGL extension loading |
| tiny_obj_loader | `.obj` mesh loading (`src/CgUtils/tiny_obj_loader.h`) |
| stb_image | Image I/O (`src/CgUtils/stb_image.h`) |

---

## Key file locations

| File | Purpose |
|---|---|
| `src/CgGeometry/cghalfedgetrianglemesh.cpp` | Loop subdivision, normals, consistency check |
| `src/CgGeometry/cgpointcloud.cpp` | Point cloud — primary target for new work |
| `src/CgRendering3d/cgframebuffershader.cpp` | Render dispatch, OpenGL draw calls |
| `src/CgRendering3d/cgopenglrenderinggui.cpp` | Scene management, ImGui panels |
| `src/CgRendering3d/shadercodephong.h` | Inline GLSL shader strings |
| `src/CgMath/cgeigendecomposition3x3.h/.cpp` | PCA utility |
| `src/CgBase/cgbaserenderableobject.h` | Type enum, base interface |

---

## Notes for agents working on this codebase

- Many declared virtual methods have stub implementations returning zero/false. Do not assume a method works because it compiles — check the `.cpp` body.
- Memory is managed with raw `new`/`delete`. Smart pointers are not used. Be careful with ownership when adding new geometry objects to the renderer's `m_renderable_objects` list.
- The half-edge mesh was recently stabilized (April 2026). If something looks broken there, check git history — several bugs were fixed in that period.
- There is no test framework. The main validation tool is `consistencyCheck()` on the half-edge mesh.
- The image viewer (`src/CgImageViewer/`) is a parallel educational track and is unlikely to be relevant to point cloud work.
- **LSP diagnostics showing `glm/glm.hpp not found` and `undeclared identifier 'glm'` are pre-existing false positives** — the LSP doesn't have CMake include paths. The project compiles normally via CMake. Ignore these diagnostics.
- The user prefers German or English — both are fine. Code and identifiers stay in English.
- Do not use the word "seed" in the context of region growing — use "candidate" instead.
- The `CgOpenGLRenderingGui` renders either `m_splat_mesh` or `m_cluster_mesh` based on `m_splat_display_mode`, never both simultaneously. Both are `CgTriangleMesh*` and managed (allocated/freed) inside `cgopenglrenderinggui.cpp`.
