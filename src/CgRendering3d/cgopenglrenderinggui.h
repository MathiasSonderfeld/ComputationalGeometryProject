#ifndef OPENGL_GUI_H
#define OPENGL_GUI_H

#include <string>
#include <vector>
#include "cgbasegui.h"
#include "cgframebuffershader.h"
#include "cghalfedgetrianglemesh.h"

class CgPointList;
class CgSphere;
class CgBaseTriangleMesh;
class CgBaseHalfEdgeTriangleMesh;
class CgBasePointCloud;
class CgTriangleMesh;
class CgTriangleFan;
class CgPointCloud;

class CgOpenGLRenderingGui : public CgBaseGui
{
public:
 // constructors / destructor
 explicit CgOpenGLRenderingGui(const std::string& current_path);
 CgOpenGLRenderingGui();
 ~CgOpenGLRenderingGui() override;

 // inherited from base gui
 void showGUI(int window_size_x, int window_size_y) override;
 void initGUI() override;
 void renderObjects() override;

 // additional gui-interaction
 // picking is executed holding cmd/ctrl modifier plus mouse left
 void calculatePickRayIntersections(glm::vec3 ray_start, glm::vec3 ray_direction);

 // if an object is moved by mouse interaction
 // selected spheres can be moved by picking and holding cmd/ctrl
 // while moving the mouse in the example
 void moveSelectedObjectInLocalCoordinates(glm::vec3 dir);

private:
 //OpenGl Rendering
 CgFrameBufferShader m_renderer;

 //variables for render settings
 int m_polygon_mode;
 bool m_lighting_mode;
 bool show_pick_ray;
 bool m_show_render_normals;

 // build gui
 void showOpenGLWindow();
 void createRenderOptionsGui();
 static void createAufgabenTabBar();

 // variables for gui
 bool show_demo_window;
 std::string gui_current_path;

 // resetting the below used objects
 void reset();


 /* ***********************************************************
  example for geometry objects
 *************************************************************/

 // List for 3D objects
 std::vector<CgBaseRenderableObject*> m_renderable_objects;

 // datastructures for some example objects
 CgBaseTriangleMesh* m_triangle_mesh;
 CgBasePointCloud* m_point_cloud;
 CgBaseHalfEdgeTriangleMesh* m_half_edge_triangle_mesh;

 // loading obj files
 static void LoadFromFile(const char* filename, std::vector<glm::vec3>& vertices,
                   std::vector<glm::vec3>& normals,
                   std::vector<unsigned int>& indices);
 void updateRenderNormalsHalfEdges(CgBaseRenderableObject* obj);


 /* ***********************************************************
  example for CgTriangleFan, provided for MIN-COG
 *************************************************************/
 CgTriangleFan* m_triangle_fan;
 void createTriangleFanExample();


 /* ***********************************************************
  example for building a control polygon using geometry objects
 *************************************************************/

 // positions of control points (rendered as Spheres)
 std::vector<CgSphere*> m_control_points;

 // selected sphere can be moved by picking and holding cmd/ctrl
 // while moving the mouse
 CgSphere* m_selected_control_point;

 // Polyline for drawing lines in 3D
 CgPointList* m_polyline; //style CG_LINE_STRIP

 void createControlPolygonExample();
 void renderControlPolygonExample();
 void deleteControlPolygonExample();


 /* ***********************************************************
  object below used for rendering the pick ray
  picking is executed holding cmd/ctrl modifier plus mouse left
 *************************************************************/

 CgPointList* m_select_ray; //style CG_LINES


 /* ***********************************************************
  show the normals of TriangleMeshes or point clouds
  constructed as CgPointList, normally this would be
  in geometry shader, but to keep GL-standard simple (e.g.
  for usage of this code in a VM) this is done as renderable
  objects (inefficient)
 *************************************************************/

 CgPointList* m_render_normals; //style CG_LINES
 void updateRenderNormals(CgBaseRenderableObject* obj);


 /* ***********************************************************
  examples of how to use math library for decompositions
 *************************************************************/

 static void calculateEigenDecomposition3x3();
 static void calculateSingularValueDecomposition();
};

#endif
