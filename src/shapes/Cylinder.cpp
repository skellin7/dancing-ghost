#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cylinder::makeCapCorner(glm::vec3 top,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight) {
    glm::vec3 normal = glm::vec3(0.f, top.y / std::abs(top.y), 0.f);
    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
}

void Cylinder::makeCapTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight) {
    glm::vec3 normal = glm::vec3(0.f, topLeft.y / std::abs(topLeft.y), 0.f);
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

void Cylinder::makeCapSlice(float currentTheta, float nextTheta){
    glm::vec3 bl = glm::vec3(0.5f / m_param1 * glm::cos(nextTheta),
                             -0.5f,
                             0.5f / m_param1 * glm::sin(nextTheta));
    glm::vec3 br = glm::vec3(0.5f / m_param1 * glm::cos(currentTheta),
                             -0.5f,
                             0.5f / m_param1 * glm::sin(currentTheta));
    makeCapCorner(glm::vec3(0.f, -0.5f, 0.f), bl, br);
    bl.y = 0.5f;
    br.y = 0.5f;
    makeCapCorner(glm::vec3(0.f, 0.5f, 0.f), br, bl);
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
        tl.y = 0.5f;
        tr.y = 0.5f;
        bl.y = 0.5f;
        br.y = 0.5f;
        makeCapTile(tr, tl, br, bl);
    }
}

glm::vec3 Cylinder::calcNorm(glm::vec3& pt) {
    float xNorm = (2 * pt.x);
    float yNorm = 0.f;
    float zNorm = (2 * pt.z);

    return glm::normalize(glm::vec3{ xNorm, yNorm, zNorm });
}

void Cylinder::makeSideTile(glm::vec3 topLeft,
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

void Cylinder::makeSideSlice(float currentTheta, float nextTheta){
    for (int i = 0; i < m_param1; i++) {
        glm::vec3 tl = glm::vec3(0.5f * cos(currentTheta),
                                 0.5f - (float)i/m_param1,
                                 0.5f * sin(currentTheta));
        glm::vec3 tr = glm::vec3(0.5f * cos(nextTheta),
                                 0.5f - (float)i/m_param1,
                                 0.5f * sin(nextTheta));
        glm::vec3 bl = glm::vec3(0.5f * cos(currentTheta),
                                 0.5f - (float)(i+1)/m_param1,
                                 0.5f * sin(currentTheta));
        glm::vec3 br = glm::vec3(0.5f * cos(nextTheta),
                                 0.5f - (float)(i+1)/m_param1,
                                 0.5f * sin(nextTheta));
        makeSideTile(tl, tr, bl, br);
    }
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    makeCapSlice(currentTheta, nextTheta);
    makeSideSlice(currentTheta, nextTheta);
}

void Cylinder::setVertexData() {
    for (int i = 0; i < m_param2; i++) {
        makeWedge((i+1) * 2*M_PI / m_param2, i * 2*M_PI / m_param2);
    }
}

void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
