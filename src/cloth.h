#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 prev_pos;
    float mass;
    bool anchored;
    glm::vec3 normal;
    glm::vec3 forces; //friction and normal
    float radius; //marble around each vertex
    std::vector<GLuint> neighbors; //neighboring vertices
};

enum class SpringType {
    STRUCTURAL,
    SHEAR,
    BEND
};

struct Spring {
    int vertexOne;
    int vertexTwo;
    float k; //spring elasticity
    float dampness;
    SpringType type;
    float rest_length;
};


class Cloth
{
public:
    float width;
    float depth;
    float widthStep;
    float depthStep;
    float height;
    glm::vec3 bottomLeftPos;
    Cloth(float w, float d, float wStep, float dStep, float h, glm::vec3 bottomLeft);

    std::vector<Vertex> m_vertices;
    std::vector<Spring> m_springs;
    std::vector<GLuint> m_triangleIndices;

    void setNormals();


private:
    void setTriangleIndices();
    void createVertices();
    void createSprings();
};

