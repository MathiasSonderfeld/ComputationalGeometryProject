// gui stuff
#include "cgopenglrenderinggui.h"
#include "imgui.h"

// loader for obj-files
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//glm stuff
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext.hpp>

// geometry includes
#include "cgtrianglemesh.h"
#include "cgtrianglefan.h"
#include "cghalfedgetrianglemesh.h"
#include "cgpointcloud.h"
#include "cgpointlist.h"
#include "cgsphere.h"

// math lib for matrix decomposition
#include "Eigen/SVD"
#include "Eigen/Core"
#include "cgeigendecomposition3x3.h"
using namespace Eigen;


CgOpenGLRenderingGui::CgOpenGLRenderingGui() : show_pick_ray(false) {
    gui_current_path = "";
    show_demo_window = true;
    m_polygon_mode = 2;
    m_lighting_mode = true;
    m_normal_scale = 0.2f;

    m_selected_control_point = nullptr;
    m_show_render_normals = false;

    m_select_ray = nullptr;
    m_render_normals = nullptr;

    m_triangle_fan = nullptr;
    m_triangle_mesh = nullptr;
    m_half_edge_triangle_mesh = nullptr;
    m_point_cloud = nullptr;
    m_polyline = nullptr;

    // show an example for a control polygon
    // provided for BIN/MDI-CG3 lecture

    // uncomment to see it
    // createControlPolygonExample();


    // show an example for a triangle fan
    // can be used to show points as discs
    // provided for MIN-COG lecture

    // uncomment to see it
    //createTriangleFanExample();


    // example of how to use custom rotation
    // apply custom rotation to show how this works
    // provided for BIN-CG3 lecture
    glm::mat4 custom_rot = glm::mat4(1.0);
    custom_rot = glm::rotate(custom_rot, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    //std::cout << glm::to_string(custom_rot) << std::endl;
    m_renderer.setCustomRotationMatrix(custom_rot);
}

CgOpenGLRenderingGui::CgOpenGLRenderingGui(const std::string& current_path) : CgOpenGLRenderingGui() {
    gui_current_path = current_path;
}


CgOpenGLRenderingGui::~CgOpenGLRenderingGui() {
    reset();
}

void CgOpenGLRenderingGui::reset() {
    for (auto obj : m_renderable_objects) {
        m_renderer.removeObject(obj);
        delete obj;
        // ReSharper disable once CppDFAUnusedValue
        obj = nullptr;
    }
    m_renderable_objects.clear();


    delete m_triangle_mesh;


    delete m_point_cloud;


    delete m_half_edge_triangle_mesh;


    m_half_edge_triangle_mesh = nullptr;
    m_triangle_mesh = nullptr;
    m_point_cloud = nullptr;

    deleteControlPolygonExample();


    delete m_triangle_fan;
    m_triangle_fan = nullptr;


    delete m_render_normals;
    m_render_normals = nullptr;
}

void CgOpenGLRenderingGui::renderObjects() {
    // in a basic setting you have a list of renderable objects that are iterated for rendering
    for (const auto obj : m_renderable_objects) {
        if (obj != nullptr)
            m_renderer.renderObject(obj);
    }

    //render the triangle mesh
    if (m_triangle_mesh != nullptr)
        m_renderer.renderObject(m_triangle_mesh);

    // show normals of triangle mesh or point cloud
    if ((m_show_render_normals) && (m_render_normals != nullptr))
        m_renderer.renderObject(m_render_normals);

    // render the half edge mesh
    if (m_half_edge_triangle_mesh != nullptr)
        m_renderer.renderObject(m_half_edge_triangle_mesh);

    // render the point cloud
    if (m_point_cloud != nullptr)
        m_renderer.renderObject(m_point_cloud);

    // render picking ray
    if (m_select_ray != nullptr)
        m_renderer.renderObject(m_select_ray);

    // render objects for control polygon example
    renderControlPolygonExample();

    // render the triangle fan
    if (m_triangle_fan != nullptr)
        m_renderer.renderObject(m_triangle_fan);
}

void CgOpenGLRenderingGui::createTriangleFanExample() {
    // show an example for a triangle fan
    // provided for MIN-COG lecture

    // init some arbitrary point positions
    // these point could represent the border of an ellipse
    std::vector<glm::vec3> points;
    points.emplace_back(0.0, 0.0, 0.0);
    points.emplace_back(0.5, 0.0, 0.0);
    points.emplace_back(0.0, 0.5, 0.0);
    points.emplace_back(-0.5, 0.0, 0.0);
    points.emplace_back(0.0, -0.5, 0.0);

    m_triangle_fan = new CgTriangleFan(points);
    m_renderer.initObject(m_triangle_fan);
}

void CgOpenGLRenderingGui::createControlPolygonExample() {
    // init some arbitrary point positions
    std::vector<glm::vec3> points;
    points.emplace_back(-0.5, 0.0, 0.0);
    points.emplace_back(0.0, -0.5, 0.0);
    points.emplace_back(0.0, 0.5, 0.0);
    points.emplace_back(0.5, 0.0, 0.0);

    // draw a connecting line for the control polygon
    m_polyline = new CgPointList(points);
    m_polyline->setLineStyle(CG_LINE_STRIP);
    m_polyline->setLineWidth(5);
    m_renderer.initObject(m_polyline);

    // each control point is additionally a pickable sphere
    // holding cmd/ctrl as modifier key, picking and moving the spheres is possible
    for (const glm::vec3& point : points) {
        CgSphere* newSphere = new CgSphere(point, 0.03);
        m_control_points.push_back(newSphere);
        m_renderer.initObject(newSphere);
    }
}


void CgOpenGLRenderingGui::renderControlPolygonExample() {
    // render the control points for curve example
    for (const auto obj : m_control_points) {
        if (obj != nullptr)
            m_renderer.renderObject(obj);
    }

    // render the polyline for curve example
    if (m_polyline != nullptr)
        m_renderer.renderObject(m_polyline);
}

void CgOpenGLRenderingGui::deleteControlPolygonExample() {
    for (auto obj : m_control_points) {
        delete obj;
        // ReSharper disable once CppDFAUnusedValue
        obj = nullptr;
    }
    m_control_points.clear();

    delete m_polyline;
    m_polyline = nullptr;
}

void CgOpenGLRenderingGui::initGUI() {
    // let gui know its opengl renderer
    m_renderer.setOpenGLRenderingGui(this);

    // set some default values for "the matrices", no need to change this
    m_renderer.setModelViewMatrix(glm::mat4(1.0));
    m_renderer.setLookAtMatrix(
        glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
    m_renderer.setProjectionMatrix(glm::mat4x4(glm::vec4(1.792591, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.792591, 0.0, 0.0),
                                               glm::vec4(0.0, 0.0, -1.0002, -1.0),
                                               glm::vec4(0.0, 0.0, -0.020002, 0.0)));
    m_renderer.init();
}


void CgOpenGLRenderingGui::calculatePickRayIntersections(glm::vec3 ray_start, glm::vec3 ray_direction) {
    // init new CgPolyline to draw the pick ray

    delete m_select_ray;
    std::vector<glm::vec3> pointlist;
    pointlist.push_back(ray_start);
    pointlist.push_back(ray_start + ray_direction * 30.0f);

    m_select_ray = new CgPointList(pointlist);
    m_select_ray->setLineStyle(CG_LINES);

    m_renderer.initObject(m_select_ray);


    // pickable are at the moment only control points
    m_selected_control_point = nullptr;

    for (const auto obj : m_control_points) {
        if (obj != nullptr) {
            if (obj->hasIntersection(ray_start, ray_direction, 1000.0))
                m_selected_control_point = obj;
        }
    }
}

void CgOpenGLRenderingGui::moveSelectedObjectInLocalCoordinates(glm::vec3 dir) {
    std::vector<glm::vec3> points;

    for (auto obj : m_control_points) {
        if (obj != nullptr) {
            // move only selected point
            if (obj == m_selected_control_point) {
                // set a new center for the object in local coordinates
                // this is a hack, since we do not have a scenegraph
                glm::vec3 v = obj->getCenter() - dir;
                obj->setCenter(v);

                // we changed the geometry, so we have to re-upload the object to graphics card
                m_renderer.removeObject(obj);
                m_renderer.initObject(obj);
            }
            points.push_back(obj->getCenter());
        }

        // also generate a new connecting line for the control Points
        m_renderer.removeObject(m_polyline);
        delete m_polyline;
        m_polyline = new CgPointList(points);
        m_polyline->setLineStyle(CG_LINE_STRIP);
        m_renderer.initObject(m_polyline);
    }
}


void CgOpenGLRenderingGui::showGUI(int window_size_x, int window_size_y) {
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(window_size_x / 2.0 - 20, window_size_y - 20));

    showOpenGLWindow();

    if (show_demo_window) {
        ImGui::SetNextWindowPos(ImVec2(window_size_x / 2.0 + 10, 10));
        ImGui::SetNextWindowSize(ImVec2(window_size_x / 2.0 - 20, window_size_y / 6.0));
    }

    createRenderOptionsGui();

    // gui composition is a bit tricky to have nice sizes of the elements
    ImGui::SetNextWindowPos(ImVec2(window_size_x / 2.0 + 10, window_size_y / 6.0 + 20));
    if (show_demo_window)
        ImGui::SetNextWindowSize(ImVec2(window_size_x / 2.0 - 20, window_size_y / 3.0));
    else
        ImGui::SetNextWindowSize(ImVec2(window_size_x / 2.0 - 20, window_size_y - 10 - (window_size_y / 6.0 + 20)));


    // create gui for all exercises
    createAufgabenTabBar();

    // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()!
    // You can browse its code to learn more about Dear ImGui!).
    ImGui::SetNextWindowPos(ImVec2(window_size_x / 2.0 + 10, window_size_y / 3.0 + 30 + window_size_y / 6.0));
    ImGui::SetNextWindowSize(ImVec2(window_size_x / 2.0 - 20, window_size_y / 3.0 * 2 - 30));
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
}

void CgOpenGLRenderingGui::createRenderOptionsGui() {
    ImGui::Begin("Render Options");


    ImGui::RadioButton("POINTS", &m_polygon_mode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("WIREFRAME", &m_polygon_mode, 1);
    ImGui::SameLine();
    ImGui::RadioButton("FILLED", &m_polygon_mode, 2);
    m_renderer.setPolygonMode(m_polygon_mode);

    ImGui::Checkbox("enable lighting", &m_lighting_mode);
    m_renderer.setLightingMode(m_lighting_mode);

    ImGui::Checkbox("render normals", &m_show_render_normals);

    if (ImGui::SliderFloat("normal length", &m_normal_scale, 0.0f, 1.0f)) {
        if (m_show_render_normals && m_render_normals != nullptr) {
            if (m_triangle_mesh != nullptr) {
                updateRenderNormals(m_triangle_mesh);
            }
            else if (m_point_cloud != nullptr) {
                updateRenderNormals(m_point_cloud);
            }
            else if (m_half_edge_triangle_mesh != nullptr) {
                updateRenderNormalsHalfEdges(m_half_edge_triangle_mesh);
            }
        }
    }

    ImGui::End();
}

void CgOpenGLRenderingGui::showOpenGLWindow() {
    ImGui::Begin("OpenGL Rendering");
    ImGui::Text("Select obj file:");

    // change filenames if you want to use other models
    const char* items[] = {
        "../models/bunny.obj", "../models/venus.obj", "../models/airbus.obj", "../models/Man_sitting.obj", "../models/stumpf.obj", "../models/cube.obj"
    };
    static int item_current = 0;

    ImGui::SameLine();
    if (ImGui::Button("load as CgTriangleMesh")) {
        // set filename
        std::string obj_filename = gui_current_path;
        obj_filename += static_cast<std::string>(items[item_current]);

        // init destination datastructures
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        // load
        LoadFromFile(obj_filename.c_str(), vertices, normals, indices);

        if (!vertices.empty()) {
            // generate TriangleMesh
            m_triangle_mesh = new CgTriangleMesh(vertices, normals, indices);
            m_renderer.initObject(m_triangle_mesh);

            // normal rendering
            updateRenderNormals(m_triangle_mesh);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("load as CgPointCloud")) {
        delete m_render_normals;
        m_render_normals = nullptr;


        // set filename
        std::string obj_filename = gui_current_path;
        obj_filename += static_cast<std::string>(items[item_current]);

        // init destination datastructures
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        // load
        LoadFromFile(obj_filename.c_str(), vertices, normals, indices);

        // generate PointCloud
        m_point_cloud = new CgPointCloud(vertices);
        m_renderer.initObject(m_point_cloud);
    }

    if (ImGui::Button("load as CgHalfEdgeTriangleMesh")) {
        delete m_render_normals;
        m_render_normals = nullptr;

        // set filename
        std::string obj_filename = gui_current_path;
        obj_filename += static_cast<std::string>(items[item_current]);

        // init destination datastructures
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        LoadFromFile(obj_filename.c_str(), vertices, normals, indices);

        // this constructor does not process the datastructures,
        // showing only the simple example for HalfEdgeMesh
        m_half_edge_triangle_mesh = new CgHalfEdgeTriangleMesh(vertices, normals, indices);
        m_renderer.initObject(m_half_edge_triangle_mesh);
        updateRenderNormalsHalfEdges(m_half_edge_triangle_mesh);
    }

    ImGui::SameLine();
    if (ImGui::Button("reset")) {
        reset();
    }

    ImGui::ListBox(" ", &item_current, items, IM_ARRAYSIZE(items), 4);

    m_renderer.show();

    ImGui::End();
}


void CgOpenGLRenderingGui::createAufgabenTabBar() {
    ImGui::Begin("Wähle Aufgabe");
    constexpr ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    ImGui::BeginTabBar("Wähle Aufgabe", tab_bar_flags);

    if (ImGui::BeginTabItem("Aufgabe 1")) {
        if (ImGui::Button("Subdivide Half-Edge Mesh")) {
            if (m_half_edge_triangle_mesh != nullptr) {
                dynamic_cast<CgHalfEdgeTriangleMesh*>(m_half_edge_triangle_mesh)->subdivide();
                m_renderer.removeObject(m_half_edge_triangle_mesh);
                m_renderer.initObject(m_half_edge_triangle_mesh);
                updateRenderNormalsHalfEdges(m_half_edge_triangle_mesh);
            }
        }

        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Aufgabe 2")) {
        ImGui::Text("Das ist ein weiterer Tab \nadd whatever you need");
        ImGui::EndTabItem();
    }


    ImGui::EndTabBar();


    ImGui::End();
}


void CgOpenGLRenderingGui::LoadFromFile(
    const char* filename,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices) {
    std::vector<CgTriangleMesh*> results;

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;

    tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, filename, nullptr);


    std::cout << shapes.size() << " shapes found" << std::endl;;


    //return only first shape
    tinyobj::shape_t& shape = shapes[0];
    tinyobj::mesh_t& mesh = shape.mesh;

    for (int j = 0; j < attributes.vertices.size() / 3; j++) {
        vertices.emplace_back(attributes.vertices[j * 3],
                              attributes.vertices[j * 3 + 1],
                              attributes.vertices[j * 3 + 2]);
    }

    std::cout << attributes.vertices.size() / 3 << " vertices found" << std::endl;;

    for (int j = 0; j < attributes.normals.size() / 3; j++) {
        normals.emplace_back(attributes.normals[j * 3],
                             attributes.normals[j * 3 + 1],
                             attributes.normals[j * 3 + 2]);
    }

    std::cout << attributes.normals.size() / 3 << " normals found" << std::endl;;


    for (auto i : mesh.indices) {
        indices.push_back(i.vertex_index);
    }

    // center object around center of gravity, scale to [-1,1]^3
    double max_x = vertices[0].x;
    double min_x = vertices[0].x;
    double max_y = vertices[0].y;
    double min_y = vertices[0].y;
    double max_z = vertices[0].z;
    double min_z = vertices[0].z;
    glm::vec3 cog = glm::vec3(0.0, 0.0, 0.0);

    for (auto v : vertices) {
        if (v.x < min_x)
            min_x = v.x;
        if (v.x < min_y)
            min_x = v.y;
        if (v.x < min_z)
            min_x = v.z;
        if (v.x > max_x)
            max_x = v.x;
        if (v.x > max_y)
            max_y = v.y;
        if (v.x > max_z)
            max_z = v.z;
        cog += v;
    }


    cog /= static_cast<double>(vertices.size());

    double x = std::max(std::abs(max_x), std::abs(min_x));
    double y = std::max(std::abs(max_y), std::abs(min_y));
    double z = std::max(std::abs(max_z), std::abs(min_z));

    // scale greatest value to -1 or 1, others accordingly
    double scale = 1.0 / std::max(std::max(x, y), z);

    for (auto& v : vertices) {
        // put cog in origin
        v -= cog;
        //scale....
        v *= scale;
    }
}

void CgOpenGLRenderingGui::updateRenderNormalsHalfEdges(CgBaseRenderableObject* obj) {
    std::vector<glm::vec3> render_normals;
    for (const auto face : dynamic_cast<CgHalfEdgeTriangleMesh*>(obj)->getFaces()) {
        glm::vec3 center = face->center();
        render_normals.push_back(center);
        render_normals.push_back(center + face->normal() * m_normal_scale);
    }

    for (const auto vertex : dynamic_cast<CgHalfEdgeTriangleMesh*>(obj)->getVertices()) {
        glm::vec3 center = vertex->position();
        render_normals.push_back(center);
        render_normals.push_back(center + vertex->normal() * m_normal_scale);
    }

    delete m_render_normals;
    m_render_normals = new CgPointList(render_normals);
    m_render_normals->setLineStyle(CG_LINES);
    m_renderer.initObject(m_render_normals);
}

void CgOpenGLRenderingGui::updateRenderNormals(CgBaseRenderableObject* obj) {
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;

    // show normals only for TriangleMesh or PointCloud
    if ((obj->getType() == TriangleMesh) || (obj->getType() == PointCloud)) {
        points = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertices();
        normals = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertexNormals();
    }
    else if (points.size() == normals.size()) {
        std::vector<glm::vec3> render_normals;
        //generate lines
        for (unsigned int i = 0; i < points.size(); i++) {
            render_normals.push_back(points[i]);
            render_normals.push_back(points[i] + normals[i] * m_normal_scale);
        }

        // init a Pointlist rendered als GL_LINES

        delete m_render_normals;
        m_render_normals = new CgPointList(render_normals);
        m_render_normals->setLineStyle(CG_LINES);
        m_renderer.initObject(m_render_normals);
    }
}


void CgOpenGLRenderingGui::calculateEigenDecomposition3x3() {
    const glm::mat3 test_matrix = glm::mat3(3., 2., 4., 2., 0., 2., 4., 2., 3.);
    std::cout << glm::to_string(test_matrix) << std::endl;

    const CgEigenDecomposition3x3 eigen(test_matrix);
    const glm::mat3 eigenvectors = eigen.getEigenvectors();
    const glm::vec3 eigenvalues = eigen.getEigenvalues();

    std::cout << glm::to_string(eigenvalues) << std::endl;
    std::cout << glm::to_string(eigenvectors) << std::endl;
}

void CgOpenGLRenderingGui::calculateSingularValueDecomposition() {
    //init some arbitrary test matrix
    MatrixXd C(2, 3);
    C(0, 0) = 3.0;
    C(0, 1) = 2.0;
    C(0, 2) = 2.0;
    C(1, 0) = 2.0;
    C(1, 1) = 3.0;
    C(1, 2) = -2.0;

    std::cout << C << std::endl;

    //compute decomposition
    const JacobiSVD<MatrixXd> svd(C, ComputeThinU | ComputeThinV);
    MatrixXd U = svd.matrixU();
    MatrixXd V = svd.matrixV();
    VectorXd SV = svd.singularValues();

    //build a diagonal matrix out of the singular values
    MatrixXd S(2, 2);
    S.setZero();
    S(0, 0) = SV(0);
    S(1, 1) = SV(1);

    std::cout << U << std::endl;
    std::cout << S << std::endl;
    std::cout << V.transpose() << std::endl;


    // compute Moore-Penrose inverse now

    S(0, 0) = 1.0 / S(0, 0);
    S(1, 1) = 1.0 / S(1, 1);
    MatrixXd C_plus = V * S * U.transpose();

    std::cout << std::endl;
    std::cout << C_plus << std::endl;
}
