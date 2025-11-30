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

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;

};

class Model
{
    public:
        Model(char *path)
        {
            loadModel(path);
            std::cout << m_meshes.size() << std::endl;
        }
        void Draw(GLuint shader);

        std::map<std::string, BoneInfo>& GetBoneInfoMap();
        int& GetBoneCount();
    private:
        // model data
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        std::vector<Texture> m_textures_loaded;

        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  std::string typeName);

        void SetVertexBoneDataToDefault(Vertex& vertex);
        void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
        void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};
