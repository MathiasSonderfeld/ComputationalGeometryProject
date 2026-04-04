#include "cgframebuffershader.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cgbaserenderableobject.h"
#include "cgbasetrianglemesh.h"
#include "cgbasetrianglefan.h"
#include "cgbasepointcloud.h"
#include "cgbasepointlist.h"
#include "cgbasehalfedgetrianglemesh.h"
#include "cgopenglrenderinggui.h"

#include <fstream>
#include <iostream>
#include <string>

#include "cgglframebuffer.h"
#include "cgshader.h"
#include "textureloader.h"


CgFrameBufferShader::CgFrameBufferShader() : m_shade_mode(0), m_lighting_mode(false), m_trackball(nullptr),
                                             m_controller(nullptr),
                                             my_image_texture(0)
{
    m_global_scale_matrix = glm::mat4(1.0);
    m_custom_rotation_matrix = glm::mat4(1.0);
    m_global_scale_matrix = glm::scale(m_global_scale_matrix, glm::vec3(0.3, 0.3, 0.3));
    m_polygon_mode = 2;

    m_framebuffer = new CgGLFrameBuffer();
    m_shaderprogram = new CgShader();
}


CgFrameBufferShader::~CgFrameBufferShader()
{
    delete m_framebuffer;
}

void CgFrameBufferShader::setOpenGLRenderingGui(CgOpenGLRenderingGui* gui)
{
    m_controller = gui;
}

void CgFrameBufferShader::setPolygonMode(int mode)
{
    m_polygon_mode = mode;
}

void CgFrameBufferShader::setShadeMode(int mode)
{
    m_shade_mode = mode;
}

void CgFrameBufferShader::setLightingMode(bool mode)
{
    m_lighting_mode = mode;
}

void CgFrameBufferShader::removeObject(const CgBaseRenderableObject* obj)
{
    if (m_gl_buffer_objects.find(obj->getID()) != m_gl_buffer_objects.end())
    {
        delete m_gl_buffer_objects[obj->getID()];
        m_gl_buffer_objects[obj->getID()] = nullptr;
    }
    m_gl_buffer_objects.erase(obj->getID());
}

void CgFrameBufferShader::initObject(CgBaseRenderableObject* obj)
{
    CgGLBufferObject* object_buffer;

    if (m_gl_buffer_objects.find(obj->getID()) != m_gl_buffer_objects.end())
    {
        object_buffer = m_gl_buffer_objects[obj->getID()];
    }
    else
    {
        object_buffer = new CgGLBufferObject();
        m_gl_buffer_objects.insert(std::make_pair(obj->getID(), object_buffer));
    }

    if (obj->getType() == TriangleMesh)
    {
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;
        std::vector<glm::vec3> vertex_normals = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertexNormals();

        if (vertex_normals.empty())
        {
            std::vector<unsigned int> triangle_indices = dynamic_cast<CgBaseTriangleMesh*>(obj)->getTriangleIndices();
            std::vector<glm::vec3> temp_points = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertices();

            for (int i = 0; i < triangle_indices.size(); i = i + 3)
            {
                points.push_back(temp_points[triangle_indices[i]]);
                points.push_back(temp_points[triangle_indices[i + 1]]);
                points.push_back(temp_points[triangle_indices[i + 2]]);
                glm::vec3 normal = glm::cross(temp_points[triangle_indices[i + 1]] - temp_points[triangle_indices[i]],
                                              temp_points[triangle_indices[i + 2]] - temp_points[triangle_indices[i]]);
                normal = glm::normalize(normal);

                normals.push_back(normal);
                normals.push_back(normal);
                normals.push_back(normal);

                indices.push_back(i);
                indices.push_back(i + 1);
                indices.push_back(i + 2);
            }
        }
        else
        {
            points = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertices();
            normals = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertexNormals();
            indices = dynamic_cast<CgBaseTriangleMesh*>(obj)->getTriangleIndices();
        }

        std::vector<glm::vec3> colors = dynamic_cast<CgBaseTriangleMesh*>(obj)->getVertexColors();


        std::vector<VertexHolder> vertices;

        for (unsigned int i = 0; i < points.size(); i++)
        {
            glm::vec3 col;
            if (colors.size() == points.size())
                col = colors[i];
            else
                col = obj->getColor();
            vertices.emplace_back(points[i], normals[i], col);
        }

        object_buffer->create_vertex_index_buffers(vertices, indices);
    }


    if (obj->getType() == HalfEdgeTriangleMesh)
    {
        // render flat only, no smooth normals


        std::vector<CgBaseHeFace*> faces = dynamic_cast<CgBaseHalfEdgeTriangleMesh*>(obj)->getFaces();
        std::cout << "faces: " << faces.size() << std::endl;


        std::vector<glm::vec3> multiple_vertices;
        std::vector<glm::vec3> multiple_colors;
        std::vector<glm::vec3> multiple_normals;
        std::vector<unsigned int> multiple_indices;

        for (unsigned int i = 0; i < faces.size(); i++)
        {
            multiple_vertices.push_back(faces[i]->edge()->vert()->position());
            multiple_colors.push_back(faces[i]->edge()->vert()->color());
            multiple_indices.push_back(3 * i);

            multiple_vertices.push_back(faces[i]->edge()->next()->vert()->position());
            multiple_colors.push_back(faces[i]->edge()->next()->vert()->color());
            multiple_indices.push_back(3 * i + 1);

            multiple_vertices.push_back(faces[i]->edge()->next()->next()->vert()->position());
            multiple_colors.push_back(faces[i]->edge()->next()->next()->vert()->color());
            multiple_indices.push_back(3 * i + 2);

            multiple_normals.push_back(faces[i]->normal());
            multiple_normals.push_back(faces[i]->normal());
            multiple_normals.push_back(faces[i]->normal());
        }

        std::vector<VertexHolder> vertices;
        vertices.reserve(multiple_vertices.size());

        for (unsigned int i = 0; i < multiple_vertices.size(); i++)
        {
            vertices.emplace_back(multiple_vertices[i], multiple_normals[i], multiple_colors[i]);
        }

        object_buffer->create_vertex_index_buffers(vertices, multiple_indices);
    }


    if (obj->getType() == PointCloud)
    {
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;

        points = dynamic_cast<CgBasePointCloud*>(obj)->getVertices();
        normals = dynamic_cast<CgBasePointCloud*>(obj)->getVertexNormals();
        colors = dynamic_cast<CgBasePointCloud*>(obj)->getVertexColors();


        std::vector<VertexHolder> object_vertices;

        bool hasNormals = false;
        if (normals.size() == points.size())
            hasNormals = true;

        for (unsigned int i = 0; i < points.size(); i++)
        {
            glm::vec3 col;
            if (colors.size() == points.size())
                col = colors[i];
            else
                col = obj->getColor();

            if (hasNormals)
                object_vertices.emplace_back(points[i], normals[i], col);
            else
                object_vertices.emplace_back(points[i], glm::vec3(0.0, 0.0, 0.0), col);
        }

        object_buffer->create_vertex_buffers(object_vertices);
    }


    if (obj->getType() == PointList)
    {
        std::vector<glm::vec3> points = dynamic_cast<CgBasePointList*>(obj)->getVertices();

        std::vector<VertexHolder> object_vertices;

        object_vertices.reserve(points.size());
        for (glm::vec3& point : points)
        {
            object_vertices.emplace_back(point, glm::vec3(0.0, 0.0, 0.0));
        }

        object_buffer->create_vertex_buffers(object_vertices);
    }


    if (obj->getType() == TriangleFan)
    {
        std::vector<glm::vec3> points = dynamic_cast<CgBaseTriangleFan*>(obj)->getVertices();
        std::vector<VertexHolder> object_vertices;

        // calculate flat surface normal for complete fan
        // since it is used as disc only
        glm::vec3 normal = glm::cross(points[0] - points[1], points[0] - points[2]);

        object_vertices.reserve(points.size());
        for (glm::vec3& point : points)
            object_vertices.emplace_back(point, normal);

        object_buffer->create_vertex_buffers(object_vertices);
    }
}


void CgFrameBufferShader::renderObject(CgBaseRenderableObject* obj)
{
    if (m_gl_buffer_objects.find(obj->getID()) != m_gl_buffer_objects.end())
    {
        m_shaderprogram->setUniformValue("mycolor", obj->getColor());
        m_gl_buffer_objects[obj->getID()]->renderObject(obj);
    }
}


void CgFrameBufferShader::init()
{
    m_trackball = new CgTrackball();
    m_trackball->init();
    m_trackball->place(glm::vec3(0.0, 0.0, 0.0), 0.75);

    m_shaderprogram->create_shaders();

    int my_image_width = 0;
    int my_image_height = 0;
    my_image_texture = 0;
    LoadTextureFromFile("../Images/street.jpg", &my_image_texture, &my_image_width, &my_image_height);
}


void CgFrameBufferShader::show()
{
    ImGui::BeginChild("Child");

    const float window_width = ImGui::GetContentRegionAvail().x;
    const float window_height = ImGui::GetContentRegionAvail().y;

    m_framebuffer->create_buffers(window_width, window_height);
    m_pos = ImGui::GetCursorScreenPos();

    const unsigned int textureID = m_framebuffer->get_texture();

    ImGui::GetWindowDrawList()->AddImage(
        reinterpret_cast<void*>(textureID),
        ImVec2(m_pos.x, m_pos.y),
        ImVec2(m_pos.x + window_width, m_pos.y + window_height),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::EndChild();


    switch (m_polygon_mode)
    {
    case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default: break;
    }

    m_framebuffer->bind();
    handleMouseEvents();
    handleKeyEvents();

    m_shaderprogram->use();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);


    const glm::mat4 val = m_lookat_matrix * m_modelview_matrix * m_trackball->getRotationMatrix() *
        m_custom_rotation_matrix * m_global_scale_matrix;
    const glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(val)));

    m_shaderprogram->setUniformValue("normalMatrix", normal_matrix);
    m_shaderprogram->setUniformValue("modelviewMatrix", val);
    m_shaderprogram->setUniformValue("projMatrix", m_projection_matrix);
    m_shaderprogram->setUniformValue("uselighting", (int)m_lighting_mode);
    m_shaderprogram->setUniformValue("flatshading", (int)m_shade_mode);


    m_shaderprogram->setUniformValue("lightpos", glm::vec3(0.0, 0.0, 70.0));

    m_shaderprogram->setUniformValue("matDiffuseColor", glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f));
    m_shaderprogram->setUniformValue("lightDiffuseColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    m_shaderprogram->setUniformValue("matAmbientColor", glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f));
    m_shaderprogram->setUniformValue("lightAmbientColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    m_shaderprogram->setUniformValue("matSpecularColor", glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f));
    m_shaderprogram->setUniformValue("lightSpecularColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    m_shaderprogram->setUniformValue("shininess", 27.8974f);


    m_controller->renderObjects();

    m_framebuffer->unbind();
}

void CgFrameBufferShader::setModelViewMatrix(const glm::mat4& val)
{
    m_modelview_matrix = val;
}

void CgFrameBufferShader::setLookAtMatrix(const glm::mat4& val)
{
    m_lookat_matrix = val;
}

void CgFrameBufferShader::setProjectionMatrix(const glm::mat4& val)
{
    m_projection_matrix = val;
}

void CgFrameBufferShader::setCustomRotationMatrix(const glm::mat4& val)
{
    m_custom_rotation_matrix = val;
}


void CgFrameBufferShader::handleMouseEvents()
{
    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const GLfloat w = static_cast<GLfloat>(viewport[2]);
    const GLfloat h = static_cast<GLfloat>(viewport[3]);

    float x = io.MousePos.x - m_pos.x;
    float y = io.MousePos.y - m_pos.y;

    // normalize into [-1;1]
    x = 2.0 * x / w - 1.0;
    if (x < -1.0) x = -1.0;
    if (x > 1.0) x = 1.0;

    y = 2.0 * y / h - 1.0;
    if (y < -1.0) y = -1.0;
    if (y > 1.0) y = 1.0;

    // Trackball usage if no ctrl Modifier used
    if (io.MouseClicked[0] && (!io.KeyCtrl))
    {
        m_trackball->Ball_Mouse(glm::vec3(x, -y, 0.0));
        m_trackball->beginDrag();
    }

    if (io.MouseDown[0] && (!io.KeyCtrl))
    {
        m_trackball->Ball_Mouse(glm::vec3(x, -y, 0.0));
        m_trackball->Ball_Update();
    }

    if (io.MouseReleased[0] && (!io.KeyCtrl))
    {
        m_trackball->endDrag();
    }

    // Picking usage if ctrl Modifier used
    if (io.MouseClicked[0] && (io.KeyCtrl))
    {
        std::cout << "Picking intended" << std::endl;
        calculatePickRayInWorldCoordinates(x, -y);
        m_old_mouse_pos = ImVec2(x, -y);
    }

    if (io.MouseDown[0] && (io.KeyCtrl))
    {
        calculateMovement(x, -y);
    }
}


void CgFrameBufferShader::handleKeyEvents()
{
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd))
    {
        m_global_scale_matrix = glm::scale(m_global_scale_matrix, glm::vec3(1.05, 1.05, 1.05));
    }
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract))
    {
        m_global_scale_matrix = glm::scale(m_global_scale_matrix, glm::vec3(0.95, 0.95, 0.95));
    }
    if (ImGui::IsKeyPressed(ImGuiKey_S))
    {
        m_global_scale_matrix = glm::scale(m_global_scale_matrix, glm::vec3(0.95, 0.95, 0.95));
    }
    if (ImGui::IsKeyPressed(ImGuiKey_B))
    {
        m_global_scale_matrix = glm::scale(m_global_scale_matrix, glm::vec3(1.05, 1.05, 1.05));
    }
}


// x,y must be normalized into [-1;1]
void CgFrameBufferShader::calculatePickRayInWorldCoordinates(double x, double y) const
{
    glm::mat4 inverse_proj = glm::inverse(m_projection_matrix);

    //unproject point on front clipping plane
    glm::vec4 p(x, y, -0.01, 1);
    glm::vec4 q = inverse_proj * p;
    q /= q.w;

    //unproject point on back clipping plane
    glm::vec4 r(x, y, 1.0, 1);
    glm::vec4 s = inverse_proj * r;
    s /= s.w;

    //construct current model view matrix by hand, since there is no scenegraph
    glm::mat4 mv_matrix = m_lookat_matrix * m_modelview_matrix * m_trackball->getRotationMatrix() *
        m_custom_rotation_matrix * m_global_scale_matrix;

    // convert pick ray into local "bunny coordinates"
    glm::mat4 mv_matrix_inv = glm::inverse(mv_matrix);
    glm::vec4 raystart = mv_matrix_inv * q;
    glm::vec4 rayend = mv_matrix_inv * s;


    glm::vec3 dir = glm::vec3(rayend) - glm::vec3(raystart);
    dir = glm::normalize(dir);
    m_controller->calculatePickRayIntersections(glm::vec3(raystart), dir);
}

// x,y must be normalized into [-1;1]
void CgFrameBufferShader::calculateMovement(double x, double y)
{
    glm::mat4 inverse_proj = glm::inverse(m_projection_matrix);

    //unproject point on front clipping plane
    glm::vec4 p(x, y, -0.01, 1);
    glm::vec4 q = inverse_proj * p;
    q /= q.w;

    //unproject point on front clipping plane
    glm::vec4 r(m_old_mouse_pos.x, m_old_mouse_pos.y, -0.01, 1);
    glm::vec4 s = inverse_proj * r;
    s /= s.w;

    m_old_mouse_pos.x = x;
    m_old_mouse_pos.y = y;


    //construct current model view matrix by hand, since there is no scenegraph
    glm::mat4 mv_matrix = m_lookat_matrix * m_modelview_matrix * m_trackball->getRotationMatrix() *
        m_global_scale_matrix;

    // convert pick ray into local "bunny coordinates"
    glm::mat4 mv_matrix_inv = glm::inverse(mv_matrix);
    glm::vec4 ray_start = mv_matrix_inv * q;
    glm::vec4 ray_end = mv_matrix_inv * s;

    // dirty with magic number so that it works
    glm::vec3 dir = (glm::vec3(ray_end) - glm::vec3(ray_start)) * 50.0f;


    m_controller->moveSelectedObjectInLocalCoordinates(glm::vec3(dir));
}
