#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils/assimp_glm_helpers.h"

// struct to hold position at keyframe
struct KeyPosition {
    glm::vec3 position;
    float timestamp;
};

// struct to hold rotation at keyframe
struct KeyRotation {
    glm::quat orientation;
    float timestamp;
};

// struct to hold scaling at keyframe
struct KeyScale {
    glm::vec3 scale;
    float timestamp;
};

// class to handle all data for a specific bone within skeleton
class Bone {
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);

    void update(float time);

    inline glm::mat4 getLocalTransform() { return m_localTransform; }
    inline std::string getBoneName() const { return m_name; }
    inline int getBoneID() { return m_id; }
    inline int getNumPositionKeys() { return m_numPositions; }

    int getPositionIndex(float time);
    int getRotationIndex(float time);
    int getScaleIndex(float time);
private:
    std::vector<KeyPosition> m_positions;
    std::vector<KeyRotation> m_rotations;
    std::vector<KeyScale> m_scales;
    int m_numPositions;
    int m_numRotations;
    int m_numScalings;

    glm::mat4 m_localTransform;
    std::string m_name;
    int m_id;

    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float time);
    glm::mat4 interpolatePosition(float time);
    glm::mat4 interpolateRotation(float time);
    glm::mat4 interpolateScaling(float time);

};
