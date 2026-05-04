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

## What is stubbed / not yet implemented

These are the intentional gaps for student implementation:

| Location | Method | Notes |
|---|---|---|
| `cgpointcloud.cpp:70` | `CgPointCloud::getClosestPoint()` | Returns `{0,0,0}` — no proximity search, no KD-tree |
| `cgpointcloud.cpp` | `m_splat_dir_x`, `m_splat_dir_y` | Never populated — splat normal directions from PCA not done |
| `cgtrianglemesh.cpp:54` | `CgTriangleMesh::hasIntersection()` | Returns false — no ray-triangle intersection |
| `cgpointcloud.cpp` | Normal estimation | `CgEigenDecomposition3x3` is available but not called |
| `src/CgImageViewer/` | All five exercise tabs | Skeleton only — parallel educational track, likely unused |

`m_splat_dir_x` / `m_splat_dir_y` are declared in `CgBasePointCloud` and used by the renderer. Without them, point clouds render as dots without oriented splats.

---

## Future development direction: point clouds

The expected focus going forward is point cloud processing. The scaffolding that matters:

- **`CgPointCloud`** — holds `std::vector<glm::vec3>` for positions, normals, colors, and two splat direction vectors. The closest-point and splat direction methods are the primary stubs to fill in.
- **`CgEigenDecomposition3x3`** — ready to use for PCA-based normal estimation from local neighborhoods.
- **`CgBasePointCloud::getClosestPoint(ray_origin, ray_dir, maxDist)`** — signature already defined; implementing this enables picking/selection on point clouds.
- **Splat rendering** — the renderer already handles splat dir vectors when provided; the rendering path exists, only the data generation is missing.

The renderer dispatches point clouds as `GL_POINTS` with vertex/normal/color buffers. Splat geometry would be generated on the CPU side in `CgPointCloud` and uploaded as `m_splat_dir_x` / `m_splat_dir_y` arrays.

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
