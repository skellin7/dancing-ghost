#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    for (int i = 0; i < m_param1; i++) {
        float currentPhi = i * M_PI / m_param1;
        float nextPhi = (i+1) * M_PI / m_param1;
        glm::vec3 tl = glm::vec3(0.5f * glm::sin(currentPhi) * glm::cos(currentTheta),
                                 0.5f * glm::cos(currentPhi),
                                 -0.5f * glm::sin(currentPhi) * glm::sin(currentTheta));
        glm::vec3 tr = glm::vec3(0.5f * glm::sin(currentPhi) * glm::cos(nextTheta),
                                 0.5f * glm::cos(currentPhi),
                                 -0.5f * glm::sin(currentPhi) * glm::sin(nextTheta));
        glm::vec3 bl = glm::vec3(0.5f * glm::sin(nextPhi) * glm::cos(currentTheta),
                                 0.5f * glm::cos(nextPhi),
                                 -0.5f * glm::sin(nextPhi) * glm::sin(currentTheta));
        glm::vec3 br = glm::vec3(0.5f * glm::sin(nextPhi) * glm::cos(nextTheta),
                                 0.5f * glm::cos(nextPhi),
                                 -0.5f * glm::sin(nextPhi) * glm::sin(nextTheta));
        makeTile(tl, tr, bl, br);
    }
}

void Sphere::makeSphere() {
    for (int i = 0; i < m_param2; i++) {
        makeWedge(i * 2*M_PI / m_param2, (i+1) * 2*M_PI / m_param2);
    }
}

void Sphere::setVertexData() {
    makeSphere();
}

void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
