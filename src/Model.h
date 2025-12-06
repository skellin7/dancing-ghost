#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "utils/assimp_glm_helpers.h"

#define MAX_BONE_WEIGHTS 8

// struct to hold offset matrix of each bone
struct BoneInfo {
    int id;
    glm::mat4 offset;
};

// class to render model made of meshes & bones
class Model {
public:
    Model(char *path) { loadModel(path); }

    void draw(GLuint shader);

    inline std::map<std::string, BoneInfo>& getBoneInfoMap() { return m_boneInfoMap; }
    inline int& getBoneCount() { return m_boneCount; }

private:
    std::vector<Mesh> m_meshes;
    std::string m_dir;
    std::vector<Texture> m_loadedTextures;

    std::map<std::string, BoneInfo> m_boneInfoMap;
    int m_boneCount = 0;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              std::string typeName);

    void setVertexBoneDataDefault(Vertex& vertex);
    void setVertexBoneData(Vertex& vertex, int boneID, float weight);
    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};
