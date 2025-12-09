#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <Eigen/Dense>

enum BoneType {
    CYLINDER,
    SPHERE,
    NONE
};

enum AnimType {
    ANIM_NONE,
    WALK_LEFT
};

struct KeyFrame {
    // glm::vec3 position;
    glm::quat rotation;
    float timestamp;
};

struct Animation {
    std::vector<KeyFrame> keyframes;
    int numKeys;
};

class Joint {
public:
    Joint(std::string name, Joint* parent, glm::vec3 localPosition, glm::quat localRotation,
                 bool dofX, bool dofY, bool dofZ, bool endJoint, BoneType boneType);

    inline std::string getName() { return m_name; }
    inline glm::quat getWorldRotation() { return m_worldRotation; }
    inline glm::mat4 getWorldTransform() { return m_worldTransform; }
    inline glm::vec4 getWorldPosition() { return m_worldTransform[3]; }
    inline Joint* getParent() { return m_parent; }
    inline bool isDOFX() { return m_dofX; }
    inline bool isDOFY() { return m_dofY; }
    inline bool isDOFZ() { return m_dofZ; }
    inline BoneType getBoneType() { return m_boneType; }
    inline bool isEndJoint() { return m_endJoint; }

    inline void multLocalRotation(glm::quat mult) { m_localRotation = glm::normalize(mult * m_localRotation); }
    inline void incLocalPosition(glm::vec3 inc) { m_localPosition += inc; }

    inline glm::vec3 getLocalPosition() { return m_localPosition; }
    inline glm::quat getLocalRotation() { return m_localRotation; }
    inline int getNumKeys(int anim) { return m_animations[anim].numKeys; }

    void addAnimation(std::vector<KeyFrame> keyframes, int numKeys);

    void update(float time, int anim);
    int getKeyIndex(float time, int anim);

    glm::vec3 getBoneVec();

    void computeFK();
    static void solveIK(Joint* endJoint, glm::vec3 ikTarget);

    static std::vector<Joint*> setupSkeleton();
    static void drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color, glm::mat4 VP,
                         GLuint shader, GLuint lineVAO, GLuint lineVBO);
    static void drawCircle(glm::vec3 c, float r, int param, glm::vec3 color,
                           glm::mat4 VP, GLuint shader, GLuint vao, GLuint vbo);

private:
    std::string m_name;
    Joint* m_parent;

    glm::vec3 m_localPosition;
    glm::quat m_localRotation;
    glm::quat m_worldRotation;
    glm::mat4 m_localTransform;
    glm::mat4 m_worldTransform;

    bool m_dofX, m_dofY, m_dofZ;

    BoneType m_boneType;

    bool m_endJoint;

    // std::vector<KeyFrame> m_keyframes;
    // int m_numKeys; // 1 less than number of keyframes, since last keyframe must be duplicate of first

    std::vector<Animation> m_animations;

    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float time);
};
