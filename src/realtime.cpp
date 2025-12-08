#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include "src/cloth.h"

#define PARAM 20

// ================== Rendering the Scene!

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    //create cloth
    m_cloth = new Cloth(settings.cloth_width, settings.cloth_length, settings.cloth_width_step, settings.cloth_length_step, 0.0f, glm::vec3(settings.x_clothBottomLeft, settings.y_clothBottomLeft, settings.z_clothBottomLeft));

    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteProgram(m_shader);
    glDeleteProgram(m_cloth_shader);

    glDeleteVertexArrays(1, &m_lineVAO);
    glDeleteVertexArrays(1, &m_circleVAO);

    glDeleteBuffers(1, &m_lineVBO);
    glDeleteBuffers(1, &m_circleVBO);

    glDeleteBuffers(1, &m_cloth_vbo);
    glDeleteVertexArrays(1, &m_cloth_vao);
    glDeleteBuffers(1, &m_cloth_ebo);

    glDeleteBuffers(1, &m_spring_vbo);
    glDeleteVertexArrays(1, &m_spring_vao);

    delete m_camera;
    for (Joint* j : m_joints) {
        delete j;
    }

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glDisable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    std::cout << size().width() << ", " << size().height() << std::endl;

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_cloth_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default_cloth.vert", ":/resources/shaders/default_cloth.frag");

    float aspect = (float)size().width() / size().height();
    m_VP = glm::ortho(-3.f*aspect, 3.f*aspect, -3.f, 3.f, -10.f, 10.f);

    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);

    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_circleVAO);
    glGenBuffers(1, &m_circleVBO);

    glBindVertexArray(m_circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (3 * PARAM), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glLineWidth(10.0f);

    m_joints = Joint::setupSkeleton();

    m_camera = new Camera();

    SceneCameraData data = {
        .pos = glm::vec4(0.f, 0.f, 10.f, 1.f),
        .look = glm::vec4(0.f, 0.f, -10.f, 0.f),
        .up = glm::vec4(0.f, 1.f, 0.f, 0.f),
        .heightAngle = 0.5f,
        .aperture = 0.f,
        .focalLength = 0.f
    };
    m_camera->setCameraData(data);
    m_camera->setWidthHeight(size().width(), size().height());
    m_camera->setNearFar(settings.nearPlane, settings.farPlane);

    m_sphere = new Sphere();

    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);

    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // sceneChanged();
    settingsChanged();
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_mouseDown) {
        if (settings.endjoint == EndJoint::RWRIST) {
            Joint::solveIK(m_joints[3], m_ikTarget);
        }
        else if (settings.endjoint == EndJoint::LWRIST) {
            Joint::solveIK(m_joints[6], m_ikTarget);
        }
        else if (settings.endjoint == EndJoint::RANKLE) {
            Joint::solveIK(m_joints[10], m_ikTarget);
        }
        else if (settings.endjoint == EndJoint::LANKLE) {
            Joint::solveIK(m_joints[13], m_ikTarget);
        }
    }

    glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
    m_VP = m_camera->getProjMatrix() * m_camera->getViewMatrix();

    for (Joint* j : m_joints) {
        if (j->getBoneType() == BoneType::CYLINDER) {
            Joint::drawLine(j->getParent()->getWorldPosition(), j->getWorldPosition(),
                     color, m_VP, m_shader, m_lineVAO, m_lineVBO);
        }
        else if (j->getBoneType() == BoneType::SPHERE) {
            float r = glm::length(j->getWorldPosition()-j->getParent()->getWorldPosition());
            Joint::drawCircle(j->getWorldPosition(), r, PARAM,
                       color, m_VP, m_shader, m_circleVAO, m_circleVBO);
        }
    }

    //Cloth stuff
    glUseProgram(m_cloth_shader);

    // for (RenderShapeData shape : m_renderData.shapes) {
    //     int dataLen;
    //     switch (shape.primitive.type) {
    //     case PrimitiveType::PRIMITIVE_SPHERE:
    //         glBindVertexArray(m_sphere_vao);
    //         dataLen = m_sphere->dataLen();
    //         break;
    //     }

    //     glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "modelMatrix"), 1, GL_FALSE, &shape.ctm[0][0]);

    //     glDrawArrays(GL_TRIANGLES, 0, dataLen / 6);
    //     glBindVertexArray(0);
    // }

    glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "viewMatrix"), 1, GL_FALSE, &m_camera->getViewMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "projMatrix"), 1, GL_FALSE, &m_camera->getProjMatrix()[0][0]);

    // //uncomment for rendering cloth via normals
    // glBindVertexArray(m_cloth_vao);

    // glm::mat4 identityMatrix = glm::mat4(glm::vec4(1.f, 0.f, 0.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 0.f, 1.f));
    // glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "modelMatrix"), 1, GL_FALSE, &identityMatrix[0][0]);
    // glm::mat4 inverseCTM = glm::inverse(identityMatrix); //same thing
    // glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "inverseModelMatrix"), 1, GL_FALSE, &inverseCTM[0][0]);

    // glDrawElements(GL_TRIANGLES, m_cloth->m_triangleIndices.size(), GL_UNSIGNED_INT, 0);

    // glBindVertexArray(0);

    //uncomment for rendering cloth via lines and points
    //painting vertices in cloth as points
    glPointSize(10.0f);
    glBindVertexArray(m_cloth_vao);

    glm::mat4 identityMatrix = glm::mat4(glm::vec4(1.f, 0.f, 0.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 0.f, 1.f));
    glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "modelMatrix"), 1, GL_FALSE, &identityMatrix[0][0]);
    glm::mat4 inverseCTM = glm::inverse(identityMatrix); //same thing
    glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "inverseModelMatrix"), 1, GL_FALSE, &inverseCTM[0][0]);

    glDrawArrays(GL_POINTS, 0, m_cloth->m_vertices.size());
    glBindVertexArray(0);

    //painting springs in cloth as lines
    glLineWidth(2.0f);
    glBindVertexArray(m_spring_vao);

    glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "modelMatrix"), 1, GL_FALSE, &identityMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_cloth_shader, "inverseModelMatrix"), 1, GL_FALSE, &inverseCTM[0][0]);

    glDrawArrays(GL_LINES, 0, m_cloth->m_springs.size() * 2);
    glBindVertexArray(0);

    //deactivate shader program
    glUseProgram(0);
}

void Realtime::clothvbovaoGeneration() {
    // //uncomment for rendering cloth via normals
    // glGenBuffers(1, &m_cloth_vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, m_cloth_vbo);

    // m_cloth->setNormals(); //bc position of vertices changed

    // std::vector<float> verticePositions;
    // // for (auto vertex : m_cloth->m_vertices) {
    // //     verticePositions.push_back(vertex.pos.x);
    // //     verticePositions.push_back(vertex.pos.y);
    // //     verticePositions.push_back(vertex.pos.z);
    // //     verticePositions.push_back(vertex.normal.x);
    // //     verticePositions.push_back(vertex.normal.y);
    // //     verticePositions.push_back(vertex.normal.z);
    // // }
    // verticePositions = {-1, 2.2, -1, 0, -1, 0, -1, 2.2, -0.9, 0, -1, 0, -1, 2.2, -0.8, 0, -1, 0};

    // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticePositions.size(), verticePositions.data(), GL_STATIC_DRAW);

    // glGenVertexArrays(1, &m_cloth_vao);
    // glBindVertexArray(m_cloth_vao);

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(0));

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

    // glDrawArrays(GL_TRIANGLES, 0, verticePositions.size() / 6);
    // glBindVertexArray(0);

    // // std::vector<int> triangleIndices = {0, 1, 2};

    // // glGenBuffers(1, &m_cloth_ebo);
    // // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cloth_ebo);
    // // // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * m_cloth->m_triangleIndices.size(), m_cloth->m_triangleIndices.data(), GL_STATIC_DRAW);
    // // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), triangleIndices.data(), GL_STATIC_DRAW);


    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);


    //uncomment for rendering cloth via lines and points

    //cloth vertcies
    glGenBuffers(1, &m_cloth_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cloth_vbo);

    std::vector<float> verticePositions;
    for (auto vertex : m_cloth->m_vertices) {
        verticePositions.push_back(vertex.pos.x);
        verticePositions.push_back(vertex.pos.y);
        verticePositions.push_back(vertex.pos.z);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticePositions.size(), verticePositions.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_cloth_vao);
    glBindVertexArray(m_cloth_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //cloth springs
    glGenBuffers(1, &m_spring_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_spring_vbo);

    std::vector<float> springData;
    for (auto spring : m_cloth->m_springs) {

        glm::vec3 color;

        if (spring.type == SpringType::STRUCTURAL) {
            color = glm::vec3(1, 0, 0); //Red
        }
        else if (spring.type == SpringType::SHEAR) {
            color = glm::vec3(0, 1, 0); //Green
        }
        else if (spring.type == SpringType::BEND) {
            color = glm::vec3(0, 0, 1); //Blue
        }

        Vertex* vOne = &m_cloth->m_vertices[spring.vertexOne];
        springData.push_back(vOne->pos.x);
        springData.push_back(vOne->pos.y);
        springData.push_back(vOne->pos.z);
        springData.push_back(color.x);
        springData.push_back(color.y);
        springData.push_back(color.z);

        Vertex* vTwo = &m_cloth->m_vertices[spring.vertexTwo];
        springData.push_back(vTwo->pos.x);
        springData.push_back(vTwo->pos.y);
        springData.push_back(vTwo->pos.z);
        springData.push_back(color.x);
        springData.push_back(color.y);
        springData.push_back(color.z);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * springData.size(), springData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_spring_vao);
    glBindVertexArray(m_spring_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_camera->setWidthHeight(size().width(), size().height());
}

void Realtime::sceneChanged() {
    m_renderData.lights.clear();
    m_renderData.shapes.clear();
    SceneParser::parse(settings.sceneFilePath, m_renderData);
    m_camera->setCameraData(m_renderData.cameraData);

    clothvbovaoGeneration();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!isValid()) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    m_sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    std::vector<float> vdata = m_sphere->generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_camera->setNearFar(settings.nearPlane, settings.farPlane);

    delete m_cloth;
    m_cloth = new Cloth(settings.cloth_width, settings.cloth_length, settings.cloth_width_step, settings.cloth_length_step, 0.0f, glm::vec3(settings.x_clothBottomLeft, settings.y_clothBottomLeft, settings.z_clothBottomLeft));
    clothvbovaoGeneration();

    update(); // asks for a PaintGL() call to occur
}

// ================== Camera Movement!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        // int posX = event->position().x();
        // int posY = event->position().y();
        // int deltaX = posX - m_prev_mouse_pos.x;
        // int deltaY = posY - m_prev_mouse_pos.y;
        // m_prev_mouse_pos = glm::vec2(posX, posY);

        // // Use deltaX and deltaY here to rotate
        // m_camera->rotateX(0.005 * deltaX);
        // m_camera->rotateY(0.005 * deltaY);

        float mx = event->position().x();
        float my = event->position().y();

        // Convert mouse position → normalized device coords
        float x = (2.0f * mx) / width() - 1.0f;
        float y = 1.0f - (2.0f * my) / height();

        glm::vec4 rayStartNDC(x, y, -1.0f, 1.0f);
        glm::vec4 rayEndNDC  (x, y,  1.0f, 1.0f);

        glm::mat4 invVP = glm::inverse(m_VP);

        glm::vec4 rayStartWorld = invVP * rayStartNDC;
        rayStartWorld /= rayStartWorld.w;

        glm::vec4 rayEndWorld = invVP * rayEndNDC;
        rayEndWorld /= rayEndWorld.w;

        glm::vec3 r0 = glm::vec3(rayStartWorld);
        glm::vec3 r1 = glm::vec3(rayEndWorld);
        glm::vec3 dir = glm::normalize(r1 - r0);

        // Intersect ray with plane z = ikPlaneZ
        float t = (m_ikPlaneZ - r0.z) / dir.z;
        m_ikTarget = r0 + t * dir;

        float canvasx = 2 * std::tan(m_camera->getWidthAngle() / 2) * ((mx + 0.5) / size().width() - 0.5);
        float canvasy = 2 * std::tan(m_camera->getHeightAngle() / 2) * ((size().height() - 1 - my + 0.5) / size().height() - 0.5);
        glm::vec4 pixel = m_camera->getInverseViewMatrix() * glm::vec4(canvasx, canvasy, -1.f, 1.f);
        glm::vec4 d = glm::normalize(pixel - m_camera->getInverseViewMatrix()[3]);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_W]) {
        m_camera->moveLookDir(5.f * deltaTime);
    }
    if (m_keyMap[Qt::Key_S]) {
        m_camera->moveLookDir(-5.f * deltaTime);
    }
    if (m_keyMap[Qt::Key_A]) {
        m_camera->moveSideDir(-5.f * deltaTime);
    }
    if (m_keyMap[Qt::Key_D]) {
        m_camera->moveSideDir(5.f * deltaTime);
    }
    if (m_keyMap[Qt::Key_Space]) {
        m_camera->moveUpDir(5.f * deltaTime);
    }
    if (m_keyMap[Qt::Key_Control]) {
        m_camera->moveUpDir(-5.f * deltaTime);
    }

    // static int totalElapsedMs = 0;
    // totalElapsedMs += elapsedms;
    // if (totalElapsedMs >= 5000) {
    //     std::cout << deltaTime << std::endl;
    //     simulate(deltaTime);
    // }
    simulate(deltaTime);
    clothvbovaoGeneration();

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
