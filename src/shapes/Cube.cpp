#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 normal = glm::normalize(glm::cross(bottomRight-topLeft, topRight-topLeft));
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

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 top = topRight - topLeft;
    glm::vec3 left = bottomLeft - topLeft;
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            makeTile(topLeft + (float)i/m_param1 * top + (float)j/m_param1 * left,
                     topLeft + (float)(i+1)/m_param1 * top + (float)j/m_param1 * left,
                     topLeft + (float)i/m_param1 * top + (float)(j+1)/m_param1 * left,
                     topLeft + (float)(i+1)/m_param1 * top + (float)(j+1)/m_param1 * left);
        }
    }
}

void Cube::setVertexData() {
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));
    makeFace(glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));
    makeFace(glm::vec3(0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f));
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, 0.5f));
    makeFace(glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));
}

void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
