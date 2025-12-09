#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "utils/sceneparser.h"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include "camera/camera.h"
#include "src/cloth.h"
#include "src/joint.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    //Cloth Methods
    void clothvbovaoGeneration();
    void simulate(float deltaTime);
    std::vector<glm::vec3> computeForces(float deltaTime);
    void verletIntegration(std::vector<glm::vec3> forces, float deltaTime);
    void solveCollisions(int iterations, float deltaTime);
    void solveClothToClothCollisions(int iterations, float deltaTime);
    void constrainSprings(int iterations);
    glm::vec3 friction(glm::vec3 velocity, glm::vec3 normal);

    // Animation Methods
    void setupSkeleton();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    GLuint m_shader; // Stores id of shader program

    RenderData m_renderData;

    // Sphere* m_sphere;
    // GLuint m_sphere_vbo, m_sphere_vao;

    Camera* m_camera;

    // Animation
    GLuint m_lineVAO, m_lineVBO;
    GLuint m_circleVAO, m_circleVBO;

    glm::mat4 m_VP;

    glm::vec3 m_ikTarget = glm::vec3(0.f);
    float m_ikPlaneZ = 0.f;

    std::string m_activeJoint;

    std::vector<Joint*> m_joints;

    int m_animType = AnimType::ANIM_NONE;
    bool m_startAnim = false;
    float m_animTime = 0.f;

    //Cloth
    Cloth* m_cloth;
    GLuint m_cloth_vbo;
    GLuint m_cloth_vao;
    GLuint m_spring_vbo;
    GLuint m_spring_vao;
    GLuint m_cloth_ebo;
    GLuint m_cloth_normals_shader;
    GLuint m_cloth_vertices_shader;

};
