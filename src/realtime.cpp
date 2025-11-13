#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"

// ================== Rendering the Scene!

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
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

    glDeleteVertexArrays(1, &m_cone_vao);
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteVertexArrays(1, &m_cylinder_vao);
    glDeleteVertexArrays(1, &m_sphere_vao);

    glDeleteBuffers(1, &m_cone_vbo);
    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteBuffers(1, &m_cylinder_vbo);
    glDeleteBuffers(1, &m_sphere_vbo);

    delete m_cone;
    delete m_cube;
    delete m_cylinder;
    delete m_sphere;

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
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    m_cone = new Cone();
    m_cube = new Cube();
    m_cylinder = new Cylinder();
    m_sphere = new Sphere();

    glGenBuffers(1, &m_cone_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);

    glGenVertexArrays(1, &m_cone_vao);
    glBindVertexArray(m_cone_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);

    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_cylinder_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);

    glGenVertexArrays(1, &m_cylinder_vao);
    glBindVertexArray(m_cylinder_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    m_camera = new Camera();

    sceneChanged();
    settingsChanged();
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    glUniformMatrix4fv(glGetUniformLocation(m_shader, "viewMat"), 1, GL_FALSE, &m_camera->getViewMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projMat"), 1, GL_FALSE, &m_camera->getProjMatrix()[0][0]);

    glUniform1fv(glGetUniformLocation(m_shader, "ka"), 1, &m_renderData.globalData.ka);
    glUniform1fv(glGetUniformLocation(m_shader, "kd"), 1, &m_renderData.globalData.kd);
    glUniform1fv(glGetUniformLocation(m_shader, "ks"), 1, &m_renderData.globalData.ks);

    glm::vec4 camera = m_camera->getInverseViewMatrix()[3];
    glUniform4fv(glGetUniformLocation(m_shader, "camera"), 1, &camera[0]);

    int numLights = m_renderData.lights.size();
    glUniform1i(glGetUniformLocation(m_shader, "numLights"), numLights);

    for (int i = 0; i < numLights; i++) {
        const SceneLightData &light = m_renderData.lights[i];
        std::string base = "lights[" + std::to_string(i) + "]";

        int type = (int)light.type;
        glUniform1iv(glGetUniformLocation(m_shader, (base + ".type").c_str()), 1, &type);
        glUniform4fv(glGetUniformLocation(m_shader, (base + ".color").c_str()), 1, &light.color[0]);
        glUniform3fv(glGetUniformLocation(m_shader, (base + ".function").c_str()), 1, &light.function[0]);
        glUniform4fv(glGetUniformLocation(m_shader, (base + ".pos").c_str()), 1, &light.pos[0]);
        glUniform4fv(glGetUniformLocation(m_shader, (base + ".dir").c_str()), 1, &light.dir[0]);
        glUniform1fv(glGetUniformLocation(m_shader, (base + ".penumbra").c_str()), 1, &light.penumbra);
        glUniform1fv(glGetUniformLocation(m_shader, (base + ".angle").c_str()), 1, &light.angle);
    }

    for (RenderShapeData shape : m_renderData.shapes) {
        int dataLen;
        switch (shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CONE:
            glBindVertexArray(m_cone_vao);
            dataLen = m_cone->dataLen();
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            glBindVertexArray(m_cube_vao);
            dataLen = m_cube->dataLen();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            glBindVertexArray(m_cylinder_vao);
            dataLen = m_cylinder->dataLen();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            glBindVertexArray(m_sphere_vao);
            dataLen = m_sphere->dataLen();
            break;
        }

        glUniformMatrix4fv(glGetUniformLocation(m_shader, "modelMat"), 1, GL_FALSE, &shape.ctm[0][0]);

        glm::vec4 cAmbient = shape.primitive.material.cAmbient;
        glm::vec4 cDiffuse = shape.primitive.material.cDiffuse;
        glm::vec4 cSpecular = shape.primitive.material.cSpecular;
        glUniform4fv(glGetUniformLocation(m_shader, "cAmbient"), 1, &cAmbient[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "cDiffuse"), 1, &cDiffuse[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "cSpecular"), 1, &cSpecular[0]);

        glUniform1fv(glGetUniformLocation(m_shader, "shininess"), 1, &shape.primitive.material.shininess);

        glDrawArrays(GL_TRIANGLES, 0, dataLen / 6);
        glBindVertexArray(0);
    }

    glUseProgram(0);
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

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!isValid()) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);
    m_cone->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    std::vector<float> vdata = m_cone->generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    m_cube->updateParams(settings.shapeParameter1);
    vdata = m_cube->generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);
    m_cylinder->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    vdata = m_cylinder->generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    m_sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    vdata = m_sphere->generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_camera->setNearFar(settings.nearPlane, settings.farPlane);

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
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_camera->rotateX(0.005 * deltaX);
        m_camera->rotateY(0.005 * deltaY);

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
