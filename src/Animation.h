#pragma once

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Bone.h"
#include "Model.h"
#include "utils/assimp_glm_helpers.h"

// struct holding node data as a tree
struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

// class to hold data for skeleton as a whole
class Animation {
public:
    Animation() = default;

    Animation(const std::string& animationPath, Model* model);

    ~Animation() {}

    Bone* findBone(const std::string& name);

    inline float getTicksPerSecond() { return m_ticksPerSec; }

    inline float getDuration() { return m_duration;}

    inline const AssimpNodeData& getRootNode() { return m_root; }

    inline const std::map<std::string, BoneInfo>& getBoneIDMap()
    {
        return m_boneInfoMap;
    }

private:
    void readMissingBones(const aiAnimation* animation, Model& model);

    void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_duration;
    int m_ticksPerSec;
    std::vector<Bone> m_bones;
    AssimpNodeData m_root;
    std::map<std::string, BoneInfo> m_boneInfoMap;
};
