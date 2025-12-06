#include "Animator.h"

Animator::Animator(Animation* animation) {
    m_currTime = 0.0;
    m_animation = animation;

    m_finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_finalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::updateAnimation(float dt) {
    m_deltaTime = dt;
    if (m_animation) {
        m_currTime += m_animation->getTicksPerSecond() * dt;
        m_currTime = fmod(m_currTime, m_animation->getDuration());
        calculateBoneTransform(&m_animation->getRootNode(), glm::mat4(1.0f));
    }
}

void Animator::playAnimation(Animation* pAnimation) {
    m_animation = pAnimation;
    m_currTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = m_animation->findBone(nodeName);

    if (Bone) {
        Bone->update(m_currTime);
        nodeTransform = Bone->getLocalTransform();
    }

    glm::mat4 worldTransform = parentTransform * nodeTransform;

    std::map<std::string, BoneInfo> boneInfoMap = m_animation->getBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_finalBoneMatrices[index] = worldTransform * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        calculateBoneTransform(&node->children[i], worldTransform);
}
