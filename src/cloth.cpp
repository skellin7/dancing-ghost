#include "cloth.h"
#include "settings.h"
#include <GL/glew.h>
#include "iostream"


Cloth::Cloth(float w, float d, float wStep, float dStep, float h, glm::vec3 bottomLeft)
    : width(w), depth(d), widthStep(wStep), depthStep(dStep), height(h), bottomLeftPos(bottomLeft) {

    sphereTop = glm::vec3(0.f);

    m_triangleIndices = std::vector<GLuint>();
    createVertices();
    createSprings();
    setTriangleIndices();
    setNormals();
};


void Cloth::createVertices() {
    int widthPoints = static_cast<int>(width / widthStep) + 1;
    int depthPoints = static_cast<int>(depth / depthStep) + 1;

    for (int i = 0; i < widthPoints; i++) {
        for (int j = 0; j < depthPoints; j++) {

            glm::vec3 position = bottomLeftPos + glm::vec3(i * widthStep, height, j * depthStep);
            Vertex v;

            v = {position, position, 5.0f, false, glm::vec3(0.f,0.f,0.f), glm::vec2(float(i) / float(widthPoints - 1), float(j) / float(depthPoints - 1)), glm::vec3(0.f,0.f,0.f), settings.clothVertexRadius, {}}; //uncomment to unanchor all vertices

            m_vertices.push_back(v);
        }
    }
}

void Cloth::createSprings() {
    int widthPoints = static_cast<int>(width / widthStep) + 1;
    int depthPoints = static_cast<int>(depth / depthStep) + 1;

    for (int i = 0; i < widthPoints; i++) {
        for (int j = 0; j < depthPoints; j++) {

            int current = i * depthPoints + j;

            //each vertex has 4 structural springs
            //dont double count! only put right and top

            if (i < widthPoints - 1) { //vertex not on the right edge of cloth
                int rightNeighbor = (i+1) * depthPoints + j; //traveling down x axis towards +x
                float restLen = glm::length(m_vertices[rightNeighbor].pos - m_vertices[current].pos);
                m_springs.push_back({current, rightNeighbor, settings.structuralK, settings.damping, SpringType::STRUCTURAL, restLen});
                m_vertices[current].neighbors.push_back(rightNeighbor);
                m_vertices[rightNeighbor].neighbors.push_back(current);
            }

            if (j < depthPoints - 1) { //vertex not on the top edge of cloth
                int topNeighbor = i * depthPoints + (j+1); //traveling down z axis towards -z
                float restLen = glm::length(m_vertices[topNeighbor].pos - m_vertices[current].pos);
                m_springs.push_back({current, topNeighbor, settings.structuralK, settings.damping, SpringType::STRUCTURAL, restLen});
                m_vertices[current].neighbors.push_back(topNeighbor);
                m_vertices[topNeighbor].neighbors.push_back(current);
            }


            //each vertex has 2 shear springs
            if (i < widthPoints - 1 && j < depthPoints - 1) { //vertex not on right edge or top edge of cloth
                int topDiagonal = (i+1) * depthPoints + (j+1);
                float restLen = glm::length(m_vertices[topDiagonal].pos - m_vertices[current].pos);
                m_springs.push_back({current, topDiagonal, settings.shearK, settings.damping, SpringType::SHEAR, restLen});
                m_vertices[current].neighbors.push_back(topDiagonal);
                m_vertices[topDiagonal].neighbors.push_back(current);
            }
            if (i < widthPoints - 1 && j > 0) { //vertex not on right edge or bottom edge of cloth
                int bottomDiagonal = (i+1) * depthPoints + (j-1);
                float restLen = glm::length(m_vertices[bottomDiagonal].pos - m_vertices[current].pos);
                m_springs.push_back({current, bottomDiagonal, settings.shearK, settings.damping, SpringType::SHEAR, restLen});
                m_vertices[current].neighbors.push_back(bottomDiagonal);
                m_vertices[bottomDiagonal].neighbors.push_back(current);
            }


            //each vertex has 4 bend springs
            //dont double count! only put right and top
            if (i < widthPoints - 2) { //vertex not on the right edge of cloth
                int rightNeighbor = (i+2) * depthPoints + j; //traveling down x axis towards +x
                float restLen = glm::length(m_vertices[rightNeighbor].pos - m_vertices[current].pos);
                m_springs.push_back({current, rightNeighbor, settings.bendK, settings.damping, SpringType::BEND, restLen});
                m_vertices[current].neighbors.push_back(rightNeighbor);
                m_vertices[rightNeighbor].neighbors.push_back(current);
            }
            if (j < depthPoints - 2) { //vertex not on the top edge of cloth
                int topNeighbor = i * depthPoints + (j+2); //traveling down z axis towards -z
                float restLen = glm::length(m_vertices[topNeighbor].pos - m_vertices[current].pos);
                m_springs.push_back({current, topNeighbor, settings.bendK, settings.damping, SpringType::BEND, restLen});
                m_vertices[current].neighbors.push_back(topNeighbor);
                m_vertices[topNeighbor].neighbors.push_back(current);
            }
        }
    }
}


void Cloth::setTriangleIndices() {
    m_triangleIndices.clear();

    int widthPoints = static_cast<int>(width / widthStep) + 1;
    int depthPoints = static_cast<int>(depth / depthStep) + 1;

    for (int i = 0; i < widthPoints-1; i++) {
        for (int j = 0; j < depthPoints-1; j++) {

            int current = i * depthPoints + j;
            int topNeighbor = i * depthPoints + (j+1);
            int rightNeighbor = (i+1) * depthPoints + j;
            int topDiagonal = (i+1) * depthPoints + (j+1);

            //bottom right triangle, insert CCW
            m_triangleIndices.push_back(current);
            m_triangleIndices.push_back(topDiagonal);
            m_triangleIndices.push_back(rightNeighbor);

            //top left triangle, insert CCW
            m_triangleIndices.push_back(current);
            m_triangleIndices.push_back(topNeighbor);
            m_triangleIndices.push_back(topDiagonal);

        }
    }
}


void Cloth::setNormals() {
    for (auto &v : m_vertices) {
        v.normal = glm::vec3(0.f, 0.f, 0.f);
    }

    for (int i = 0; i < m_triangleIndices.size(); i += 3) {
        int index0 = m_triangleIndices[i];
        int index1 = m_triangleIndices[i + 1];
        int index2 = m_triangleIndices[i + 2];

        glm::vec3 normal = glm::normalize(glm::cross(m_vertices[index2].pos - m_vertices[index1].pos, m_vertices[index1].pos - m_vertices[index0].pos));

        m_vertices[index0].normal += normal;
        m_vertices[index1].normal += normal;
        m_vertices[index2].normal += normal;
    }

    for (auto &v : m_vertices) {
        v.normal = glm::normalize(v.normal);
    }
}


void Cloth::updateClothPos(glm::vec3 newSphereTop, bool left) {

    glm::vec3 offset = glm::abs(sphereTop - newSphereTop);

    for (auto &v : m_vertices) {
        if (left) {
            v.pos = v.pos - offset;
        }
        else {
            v.pos = v.pos + offset;
        }
    }

    sphereTop = newSphereTop;
}

