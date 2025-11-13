#pragma once

#include <glm/glm.hpp>
#include <utils/scenedata.h>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    Camera() {}

    void setCameraData(const SceneCameraData &cameraData);
    void setNearFar(float near, float far);
    void setWidthHeight(int width, int height);

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;
    glm::mat4 getInverseViewMatrix() const;

    glm::mat4 getProjMatrix() const;

private:
    glm::mat4 m_viewMat;
    glm::mat4 m_invViewMat;
    glm::mat4 m_projMat;

    float m_near;
    float m_far;

    float m_heightAngle;
    float m_aspectRatio;

    void findProjMatrix();
};
