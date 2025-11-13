#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cone::makeCapCorner(glm::vec3 top,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight) {
    glm::vec3 normal = glm::vec3(0.f, -1.f, 0.f);
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
}

void Cone::makeCapTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight) {
    glm::vec3 normal = glm::vec3(0.f, -1.f, 0.f);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
}

void Cone::makeCapSlice(float currentTheta, float nextTheta){
    glm::vec3 bl = glm::vec3(0.5f / m_param1 * glm::cos(nextTheta),
                             -0.5f,
                             0.5f / m_param1 * glm::sin(nextTheta));
    glm::vec3 br = glm::vec3(0.5f / m_param1 * glm::cos(currentTheta),
                             -0.5f,
                             0.5f / m_param1 * glm::sin(currentTheta));
    makeCapCorner(glm::vec3(0.f, -0.5f, 0.f), bl, br);
    for (int i = 1; i < m_param1; i++) {
        glm::vec3 tl = glm::vec3(i * 0.5f / m_param1 * glm::cos(nextTheta),
                                 -0.5f,
                                 i * 0.5f / m_param1 * glm::sin(nextTheta));
        glm::vec3 tr = glm::vec3(i * 0.5f / m_param1 * glm::cos(currentTheta),
                                 -0.5f,
                                 i * 0.5f / m_param1 * glm::sin(currentTheta));
        glm::vec3 bl = glm::vec3((i+1) * 0.5f / m_param1 * glm::cos(nextTheta),
                                 -0.5f,
                                 (i+1) * 0.5f / m_param1 * glm::sin(nextTheta));
        glm::vec3 br = glm::vec3((i+1) * 0.5f / m_param1 * glm::cos(currentTheta),
                                 -0.5f,
                                 (i+1) * 0.5f / m_param1 * glm::sin(currentTheta));
        makeCapTile(tl, tr, bl, br);
    }
}

glm::vec3 Cone::calcNorm(glm::vec3& pt) {
    float xNorm = (2 * pt.x);
    float yNorm = -(1.f/4.f) * (2.f * pt.y - 1.f);
    float zNorm = (2 * pt.z);

    return glm::normalize(glm::vec3{ xNorm, yNorm, zNorm });
}

void Cone::makeSlopeCorner(glm::vec3 top,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight) {
    glm::vec3 blNormal = calcNorm(bottomLeft);
    glm::vec3 brNormal = calcNorm(bottomRight);
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, 0.5f * (blNormal + brNormal));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, blNormal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, brNormal);
}

void Cone::makeSlopeTile(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight) {
    glm::vec3 leftNorm = calcNorm(topLeft);
    glm::vec3 rightNorm = calcNorm(topRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, leftNorm);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, rightNorm);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, rightNorm);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, leftNorm);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, leftNorm);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, rightNorm);
}

void Cone::makeSlopeSlice(float currentTheta, float nextTheta){
    glm::vec3 bl = glm::vec3(0.5f/m_param1 * cos(currentTheta),
                             0.5f - 1.f/m_param1,
                             0.5f/m_param1 * sin(currentTheta));
    glm::vec3 br = glm::vec3(0.5f/m_param1 * cos(nextTheta),
                             0.5f - 1.f/m_param1,
                             0.5f/m_param1 * sin(nextTheta));
    makeSlopeCorner(glm::vec3(0.f, 0.5f, 0.f), bl, br);
    for (int i = 1; i < m_param1; i++) {
        glm::vec3 tl = glm::vec3(0.5f*i/m_param1 * cos(currentTheta),
                                 0.5f - (float)i/m_param1,
                                 0.5f*i/m_param1 * sin(currentTheta));
        glm::vec3 tr = glm::vec3(0.5f*i/m_param1 * cos(nextTheta),
                                 0.5f - (float)i/m_param1,
                                 0.5f*i/m_param1 * sin(nextTheta));
        bl = glm::vec3(0.5f*(i+1)/m_param1 * cos(currentTheta),
                       0.5f - (float)(i+1)/m_param1,
                       0.5f*(i+1)/m_param1 * sin(currentTheta));
        br = glm::vec3(0.5f*(i+1)/m_param1 * cos(nextTheta),
                       0.5f - (float)(i+1)/m_param1,
                       0.5f*(i+1)/m_param1 * sin(nextTheta));
        makeSlopeTile(tl, tr, bl, br);
    }
}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    makeCapSlice(currentTheta, nextTheta);
    makeSlopeSlice(currentTheta, nextTheta);
}

void Cone::setVertexData() {
    for (int i = 0; i < m_param2; i++) {
        makeWedge((i+1) * 2*M_PI / m_param2, i * 2*M_PI / m_param2);
    }
}

void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
