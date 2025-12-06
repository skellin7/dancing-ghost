#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Animation.h"
#include "Bone.h"

// class to run given animation by calculating world transforms for each bone

class Animator {
public:
    Animator(Animation* Animation);

    void updateAnimation(float dt);
    void playAnimation(Animation* pAnimation);

    void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    inline std::vector<glm::mat4> getFinalBoneMatrices() { return m_finalBoneMatrices; }

private:
    std::vector<glm::mat4> m_finalBoneMatrices;
    Animation* m_animation;
    float m_currTime;
    float m_deltaTime;
};
