#include "src/realtime.h"
#include "src/settings.h"

void Realtime::clothvbovaoGeneration() {

    if (settings.renderType == RenderType::vertices) {
        //cloth vertices
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

    else { //for rendering cloth with normals or texture

        glGenBuffers(1, &m_cloth_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cloth_vbo);

        m_cloth->setNormals(); //bc position of vertices changed

        std::vector<float> verticePositions;
        for (auto vertex : m_cloth->m_vertices) {
            verticePositions.push_back(vertex.pos.x);
            verticePositions.push_back(vertex.pos.y);
            verticePositions.push_back(vertex.pos.z);
            verticePositions.push_back(vertex.normal.x);
            verticePositions.push_back(vertex.normal.y);
            verticePositions.push_back(vertex.normal.z);
            verticePositions.push_back(vertex.uv.x);
            verticePositions.push_back(vertex.uv.y);
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticePositions.size(), verticePositions.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_cloth_vao);
        glBindVertexArray(m_cloth_vao);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), reinterpret_cast<void*>(0));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), reinterpret_cast<void*>(6*sizeof(GLfloat)));

        glGenBuffers(1, &m_cloth_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cloth_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * m_cloth->m_triangleIndices.size(), m_cloth->m_triangleIndices.data(), GL_STATIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}
