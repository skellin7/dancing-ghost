#include <stdexcept>
#include "camera.h"
#include "glm/gtx/transform.hpp"

void Camera::setCameraData(const SceneCameraData &cameraData) {
    m_heightAngle = cameraData.heightAngle;
    findProjMatrix();

    glm::vec4 w = -1.f * glm::normalize(cameraData.look);
    glm::vec4 v = glm::normalize(cameraData.up - glm::dot(cameraData.up, w) * w);
    glm::vec4 u = glm::vec4(glm::cross(glm::vec3(v.x, v.y, v.z), glm::vec3(w.x, w.y, w.z)), 0.f);
    glm::mat4 r = glm::mat4(u.x, v.x, w.x, 0.f, u.y, v.y, w.y, 0.f, u.z, v.z, w.z, 0.f, 0.f, 0.f, 0.f, 1.f);
    glm::vec4 p = cameraData.pos;
    glm::mat4 t = glm::mat4(1.0, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, -1.f*p.x, -1.f*p.y, -1.f*p.z, 1.f);

    m_viewMat = r * t;
    m_invViewMat = glm::inverse(m_viewMat);
}

void Camera::findProjMatrix() {
    float c = -m_near/m_far;
    float widthAngle = 2.f * std::atan(m_aspectRatio * std::tan(m_heightAngle * 0.5f));
    glm::mat4 remap = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                0.f, 1.f, 0.f, 0.f,
                                0.f, 0.f, -2.f, 0.f,
                                0.f, 0.f, -1.f, 1.f);
    glm::mat4 unhinge = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                  0.f, 1.f, 0.f, 0.f,
                                  0.f, 0.f, 1.f/(1.f+c), -1.f,
                                  0.f, 0.f, -c/(1.f+c), 0.f);
    glm::mat4 scale = glm::mat4(1.f/(m_far*tan(widthAngle/2.f)), 0.f, 0.f, 0.f,
                                0.f, 1.f/(m_far*tan(m_heightAngle/2.f)), 0.f, 0.f,
                                0.f, 0.f, 1.f/m_far, 0.f,
                                0.f, 0.f, 0.f, 1.f);
    m_projMat = remap * unhinge * scale;
}

void Camera::setNearFar(float near, float far) {
    m_near = near;
    m_far = far;
    findProjMatrix();
}

void Camera::setWidthHeight(int width, int height) {
    m_aspectRatio = (float)width / height;
    findProjMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return m_viewMat;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return m_invViewMat;
}

glm::mat4 Camera::getProjMatrix() const {
    return m_projMat;
}
