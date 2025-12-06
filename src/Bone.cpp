#include "Bone.h"

// read keyframes from aiNodeAnim to setup Bone
Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : m_name(name), m_id(ID), m_localTransform(1.0f)
{
    m_numPositions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < m_numPositions; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
        data.timestamp = timeStamp;
        m_positions.push_back(data);
    }

    m_numRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_numRotations; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
        data.timestamp = timeStamp;
        m_rotations.push_back(data);
    }

    m_numScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_numScalings; ++keyIndex) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.scale = AssimpGLMHelpers::GetGLMVec(scale);
        data.timestamp = timeStamp;
        m_scales.push_back(data);
    }
}

// interpolates between positions, rotations, & scaling keys for current state
void Bone::update(float time) {
    glm::mat4 translation = interpolatePosition(time);
    glm::mat4 rotation = interpolateRotation(time);
    glm::mat4 scale = interpolateScaling(time);
    m_localTransform = translation * rotation * scale;
}

// gets current position key from current time in animation
int Bone::getPositionIndex(float time) {
    for (int index = 0; index < m_numPositions - 1; ++index)
    {
        if (time < m_positions[index + 1].timestamp)
            return index;
    }
    assert(0);
}

// gets current rotation key from current time in animation
int Bone::getRotationIndex(float time) {
    for (int index = 0; index < m_numRotations - 1; ++index)
    {
        if (time < m_rotations[index + 1].timestamp)
            return index;
    }
    assert(0);
}

// gets current scale key from current time in animation
int Bone::getScaleIndex(float time) {
    for (int index = 0; index < m_numScalings - 1; ++index)
    {
        if (time < m_scales[index + 1].timestamp)
            return index;
    }
    assert(0);
}

// gets normalized value for scaling
float Bone::getScaleFactor(float lastTimestamp, float nextTimestamp, float time) {
    float scaleFactor = 0.0f;
    float midWayLength = time - lastTimestamp;
    float framesDiff = nextTimestamp - lastTimestamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

// gets current position by interpolating between current & next position keys
glm::mat4 Bone::interpolatePosition(float time) {
    if (1 == m_numPositions)
        return glm::translate(glm::mat4(1.0f), m_positions[0].position);

    int p0 = getPositionIndex(time);
    int p1 = p0 + 1;
    float scaleFactor = getScaleFactor(m_positions[p0].timestamp,
                                       m_positions[p1].timestamp,
                                       time);
    glm::vec3 finalPosition = glm::mix(m_positions[p0].position,
                                       m_positions[p1].position,
                                       scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

// gets current rotation by interpolating between current & next rotation keys
glm::mat4 Bone::interpolateRotation(float time) {
    if (1 == m_numRotations)
    {
        auto rotation = glm::normalize(m_rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0 = getRotationIndex(time);
    int p1 = p0 + 1;
    float scaleFactor = getScaleFactor(m_rotations[p0].timestamp,
                                       m_rotations[p1].timestamp,
                                       time);
    glm::quat finalRotation = glm::slerp(m_rotations[p0].orientation,
                                         m_rotations[p1].orientation,
                                         scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

// gets current scaling by interpolating between current & next scale keys
glm::mat4 Bone::interpolateScaling(float time) {
    if (1 == m_numScalings)
        return glm::scale(glm::mat4(1.0f), m_scales[0].scale);

    int p0 = getScaleIndex(time);
    int p1 = p0 + 1;
    float scaleFactor = getScaleFactor(m_scales[p0].timestamp,
                                       m_scales[p1].timestamp,
                                       time);
    glm::vec3 finalScale = glm::mix(m_scales[p0].scale,
                                    m_scales[p1].scale,
                                    scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
