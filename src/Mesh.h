#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4

// struct for data for each vertex in mesh
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

// struct for data for a texture
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

// class to render mesh with texture
class Mesh {
public:
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture>      m_textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(GLuint shader);

private:
    GLuint m_vao, m_vbo, m_ebo;
    void setupMesh();
};
