#include "joint.h"

Joint::Joint(std::string name, Joint* parent, glm::vec3 localPosition, glm::quat localRotation,
             bool dofX, bool dofY, bool dofZ, bool endJoint, BoneType boneType)
    : m_name(name), m_parent(parent), m_localPosition(localPosition), m_localRotation(localRotation),
      m_dofX(dofX), m_dofY(dofY), m_dofZ(dofZ), m_endJoint(endJoint), m_boneType(boneType)
{
    computeFK();
}

void Joint::update(float time, int anim) {
    if (m_animations[anim].numKeys == 1) {
        // m_localPosition = m_keyframes[0].position;
        m_localRotation = m_animations[anim].keyframes[0].rotation;
    }
    else {
        int p0 = getKeyIndex(time, anim);
        int p1 = p0 + 1;
        float scaleFactor = getScaleFactor(m_animations[anim].keyframes[p0].timestamp,
                                           m_animations[anim].keyframes[p1].timestamp,
                                           time);
        // glm::vec3 finalPosition = glm::mix(m_keyframes[p0].position,
        //                                    m_keyframes[p1].position,
        //                                    scaleFactor);
        // m_localPosition = finalPosition;

        glm::quat finalRotation = glm::slerp(m_animations[anim].keyframes[p0].rotation,
                                             m_animations[anim].keyframes[p1].rotation,
                                             scaleFactor);
        m_localRotation = finalRotation;
    }

    this->computeFK();
}

// gets current key from current time in animation
int Joint::getKeyIndex(float time, int anim) {
    for (int index = 0; index < m_animations[anim].numKeys; index++)
    {
        if (time < m_animations[anim].keyframes[index + 1].timestamp)
            return index;
    }
    assert(0);
}

float Joint::getScaleFactor(float lastTimestamp, float nextTimestamp, float time) {
    float scaleFactor = 0.0f;
    float midWayLength = time - lastTimestamp;
    float framesDiff = nextTimestamp - lastTimestamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::vec3 Joint::getBoneVec() {
    return m_localPosition;
}

void Joint::computeFK() {
    m_localTransform =
        glm::translate(glm::mat4(1.0f), m_localPosition) *
        glm::toMat4(m_localRotation);

    if (m_parent == nullptr) {
        m_worldTransform = m_localTransform;
        m_worldRotation = m_localRotation;
    }
    else {
        m_worldRotation = m_parent->getWorldRotation() * m_localRotation;
        m_worldTransform = m_parent->getWorldTransform() * m_localTransform;
    }
}

inline Eigen::Vector3f glmToEigen(const glm::vec3 &v) {
    return Eigen::Vector3f(v.x, v.y, v.z);
}

void Joint::solveIK(Joint* endJoint, glm::vec3 ikTarget) {
    const int ITER = 10;
    const float step = 5e-3f;

    int DOFCOUNT = 0;

    std::vector<Joint*> chain;
    Joint* j = endJoint;
    float chainLen = 0.f;

    while (j != nullptr && (j->isDOFX() || j->isDOFY() || j->isDOFZ())) {
        chain.push_back(j);
        DOFCOUNT += (j->isDOFX() ? 1 : 0) + (j->isDOFY() ? 1 : 0) + (j->isDOFZ() ? 1 : 0);
        chainLen += glm::length(j->getBoneVec());
        j = j->getParent();
    }
    std::reverse(chain.begin(), chain.end());

    if (glm::length(ikTarget) > chainLen) {
        ikTarget = chainLen * glm::normalize(ikTarget);
    }
    ikTarget += glm::vec3(chain.front()->getWorldPosition());
    // std::cout << glm::to_string(ikTarget) << std::endl;

    for (int it = 0; it < ITER; it++) {
        for (Joint* j : chain) {
            j->computeFK();
        }

        glm::vec3 p = chain.back()->getWorldPosition();
        glm::vec3 e = ikTarget - p;
        if (glm::length(e) < step)
            return;

        Eigen::MatrixXf J(3, DOFCOUNT);
        int col = 0;

        for (Joint* j : chain)
        {
            glm::vec3 jointPos = j->getWorldPosition();
            glm::vec3 toEnd = p - jointPos;

            glm::quat R = j->getWorldRotation();

            if (j->isDOFX()) {
                glm::vec3 ax = R * glm::vec3(1,0,0);
                glm::vec3 dpx = glm::cross(ax, toEnd);
                J.col(col++) = glmToEigen(dpx);
            }

            if (j->isDOFY()) {
                glm::vec3 ay = R * glm::vec3(0,1,0);
                glm::vec3 dpy = glm::cross(ay, toEnd);
                J.col(col++) = glmToEigen(dpy);
            }

            if (j->isDOFZ()) {
                glm::vec3 az = R * glm::vec3(0,0,1);
                glm::vec3 dpz = glm::cross(az, toEnd);
                J.col(col++) = glmToEigen(dpz);
            }
        }

        Eigen::VectorXf dTheta =
            (J.transpose() * J + 0.01f * Eigen::MatrixXf::Identity(DOFCOUNT, DOFCOUNT))
            .ldlt()
            .solve(J.transpose() * glmToEigen(e));

        int k = 0;
        for (Joint* j : chain)
        {
            glm::quat dq = glm::quat(1.0f, 0, 0, 0);
            float delta = dTheta(k++) * step;

            if (j->isDOFX()) {
                glm::quat qx = glm::angleAxis(delta, glm::vec3(1.0f, 0.0f, 0.0f));
                dq = qx * dq;
            }
            if (j->isDOFY()) {
                glm::quat qy = glm::angleAxis(delta, glm::vec3(0.0f, 1.0f, 0.0f));
                dq = qy * dq;
            }

            if (j->isDOFZ()) {
                glm::quat qz = glm::angleAxis(delta, glm::vec3(0.0f, 0.0f, 1.0f));
                dq = qz * dq;
            }

            j->multLocalRotation(dq);
        }
    }

    for (Joint* j : chain) {
        j->computeFK();
    }
}

void Joint::addAnimation(std::vector<KeyFrame> keyframes, int numKeys) {
    m_animations.push_back({keyframes, numKeys});
}

std::vector<Joint*> Joint::setupSkeleton() {
    std::vector<KeyFrame> defaultkf;
    defaultkf.push_back({glm::quat(1.f, 0.f, 0.f, 0.f), 0.f});

    Joint *chest = new Joint{"chest", nullptr,
                             glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                             false, false, false, false, BoneType::NONE};
    chest->addAnimation(defaultkf, 1);
    chest->addAnimation(defaultkf, 1);
    chest->addAnimation(defaultkf, 1);

    Joint *rightShoulder = new Joint{"rightShoulder", chest,
                                     glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                     false, false, true, false, BoneType::NONE};
    rightShoulder->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> rshoulderkf;
    rshoulderkf.push_back({glm::quat(0.995f, 0.f, 0.f, -0.101f), 0.f});
    rshoulderkf.push_back({glm::quat(1.f, 0.f, 0.f, 0.006f), 1.f});
    rshoulderkf.push_back({glm::quat(0.968f, 0.f, 0.f, -0.251f), 2.f});
    rshoulderkf.push_back({glm::quat(0.947f, 0.f, 0.f, -0.320f), 3.f});
    rshoulderkf.push_back({glm::quat(0.996f, 0.f, 0.f, -0.085f), 4.f});
    rshoulderkf.push_back({glm::quat(0.995f, 0.f, 0.f, -0.101f), 5.f});
    rightShoulder->addAnimation(rshoulderkf, 5);

    Joint *rightElbow = new Joint{"rightElbow", rightShoulder,
                                  glm::vec3(0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                  false, false, true, false, BoneType::CYLINDER};
    rightElbow->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> relbowkf;
    relbowkf.push_back({glm::quat(0.990f, 0.f, 0.f, -0.141f), 0.f});
    relbowkf.push_back({glm::quat(0.908f, 0.f, 0.f, -0.419f), 1.f});
    relbowkf.push_back({glm::quat(-0.932f, 0.f, 0.f, -0.363f), 2.f});
    relbowkf.push_back({glm::quat(-0.902f, 0.f, 0.f, -0.432f), 3.f});
    relbowkf.push_back({glm::quat(-0.972f, 0.f, 0.f, -0.233f), 4.f});
    relbowkf.push_back({glm::quat(0.990f, 0.f, 0.f, -0.141f), 5.f});
    rightElbow->addAnimation(relbowkf, 5);

    Joint *rightWrist = new Joint{"rightWrist", rightElbow,
                                  glm::vec3(0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                  false, false, true, true, BoneType::CYLINDER};
    rightWrist->addAnimation(defaultkf, 1);
    rightWrist->addAnimation(defaultkf, 1);
    rightWrist->addAnimation(defaultkf, 1);

    Joint *leftShoulder = new Joint{"leftShoulder", chest,
                                    glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                    false, false, true, false, BoneType::NONE};
    leftShoulder->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> lshoulderkf;
    lshoulderkf.push_back({glm::quat(0.194f, 0.f, 0.f, 0.981f), 0.f});
    lshoulderkf.push_back({glm::quat(0.349f, 0.f, 0.f, 0.937f), 1.f});
    lshoulderkf.push_back({glm::quat(0.092f, 0.f, 0.f, 0.996f), 2.f});
    lshoulderkf.push_back({glm::quat(0.101f, 0.f, 0.f, -0.995f), 3.f});
    lshoulderkf.push_back({glm::quat(-0.034f, 0.f, 0.f, -0.999f), 4.f});
    lshoulderkf.push_back({glm::quat(0.194f, 0.f, 0.f, 0.981f), 5.f});
    leftShoulder->addAnimation(lshoulderkf, 5);

    Joint *leftElbow = new Joint{"leftElbow", leftShoulder,
                                 glm::vec3(-0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, false, BoneType::CYLINDER};
    leftElbow->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> lelbowkf;
    lelbowkf.push_back({glm::quat(0.987f, 0.f, 0.f, 0.163f), 0.f});
    lelbowkf.push_back({glm::quat(0.878f, 0.f, 0.f, 0.478f), 1.f});
    lelbowkf.push_back({glm::quat(0.994f, 0.f, 0.f, 0.111f), 2.f});
    lelbowkf.push_back({glm::quat(-0.970f, 0.f, 0.f, 0.241f), 3.f});
    lelbowkf.push_back({glm::quat(-0.988f, 0.f, 0.f, 0.152f), 4.f});
    lelbowkf.push_back({glm::quat(0.987f, 0.f, 0.f, 0.163f), 5.f});
    leftElbow->addAnimation(lelbowkf, 5);

    Joint *leftWrist = new Joint{"leftWrist", leftElbow,
                                 glm::vec3(-0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, true, BoneType::CYLINDER};
    leftWrist->addAnimation(defaultkf, 1);
    leftWrist->addAnimation(defaultkf, 1);
    leftWrist->addAnimation(defaultkf, 1);

    Joint *hip = new Joint{"hip", chest,
                           glm::vec3(0.f, -0.5f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                           false, false, false, false, BoneType::CYLINDER};
    hip->addAnimation(defaultkf, 1);
    hip->addAnimation(defaultkf, 1);
    hip->addAnimation(defaultkf, 1);

    Joint *rightHip = new Joint{"rightHip", hip,
                                glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                false, false, true, false, BoneType::NONE};
    rightHip->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> rhipkf;
    rhipkf.push_back({glm::quat(-0.998f, 0.f, 0.f, 0.065f), 0.f});
    rhipkf.push_back({glm::quat(-0.999f, 0.f, 0.f, 0.013f), 1.f});
    rhipkf.push_back({glm::quat(-0.896f, 0.f, 0.f, 0.444f), 2.f});
    rhipkf.push_back({glm::quat(-0.935f, 0.f, 0.f, 0.356f), 3.f});
    rhipkf.push_back({glm::quat(-0.955f, 0.f, 0.f, 0.296f), 4.f});
    rhipkf.push_back({glm::quat(-0.995f, 0.f, 0.f, 0.098f), 5.f});
    rhipkf.push_back({glm::quat(-0.998f, 0.f, 0.f, 0.065f), 6.f});
    rightHip->addAnimation(rhipkf, 6);

    Joint *rightKnee = new Joint{"rightKnee", rightHip,
                                 glm::vec3(0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, false, BoneType::CYLINDER};
    rightKnee->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> rkneekf;
    rkneekf.push_back({glm::quat(-0.987f, 0.f, 0.f, 0.159f), 0.f});
    rkneekf.push_back({glm::quat(-0.626f, 0.f, 0.f, 0.780f), 1.f});
    rkneekf.push_back({glm::quat(-0.957f, 0.f, 0.f, 0.290f), 2.f});
    rkneekf.push_back({glm::quat(-0.996f, 0.f, 0.f, 0.093f), 3.f});
    rkneekf.push_back({glm::quat(-0.993f, 0.f, 0.f, 0.119f), 4.f});
    rkneekf.push_back({glm::quat(-0.994f, 0.f, 0.f, 0.109f), 5.f});
    rkneekf.push_back({glm::quat(-0.987f, 0.f, 0.f, 0.159f), 6.f});
    rightKnee->addAnimation(rkneekf, 6);

    Joint *rightAnkle = new Joint{"rightAnkle", rightKnee,
                                 glm::vec3(0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, true, BoneType::CYLINDER};
    rightAnkle->addAnimation(defaultkf, 1);
    rightAnkle->addAnimation(defaultkf, 1);
    rightAnkle->addAnimation(defaultkf, 1);

    Joint *leftHip = new Joint{"leftHip", hip,
                               glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                               false, false, true, false, BoneType::NONE};
    leftHip->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> lhipkf;
    lhipkf.push_back({glm::quat(-0.965f, 0.f, 0.f, -0.261f), 0.f});
    lhipkf.push_back({glm::quat(-0.955f, 0.f, 0.f, -0.296f), 1.f});
    lhipkf.push_back({glm::quat(-0.913f, 0.f, 0.f, -0.408f), 2.f});
    lhipkf.push_back({glm::quat(-0.913f, 0.f, 0.f, -0.408f), 3.f});
    lhipkf.push_back({glm::quat(-0.897f, 0.f, 0.f, -0.442f), 4.f});
    lhipkf.push_back({glm::quat(-0.988f, 0.f, 0.f, -0.157f), 5.f});
    lhipkf.push_back({glm::quat(-0.965f, 0.f, 0.f, -0.261f), 6.f});
    leftHip->addAnimation(lhipkf, 6);

    Joint *leftKnee = new Joint{"leftKnee", leftHip,
                                 glm::vec3(-0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, false, BoneType::CYLINDER};
    leftKnee->addAnimation(defaultkf, 1);
    std::vector<KeyFrame> lkneekf;
    lkneekf.push_back({glm::quat(-0.985f, 0.f, 0.f, 0.172f), 0.f});
    lkneekf.push_back({glm::quat(-0.992f, 0.f, 0.f, 0.130f), 1.f});
    lkneekf.push_back({glm::quat(-0.996f, 0.f, 0.f, 0.086f), 2.f});
    lkneekf.push_back({glm::quat(-0.996f, 0.f, 0.f, 0.086f), 3.f});
    lkneekf.push_back({glm::quat(-0.646f, 0.f, 0.f, 0.763f), 4.f});
    lkneekf.push_back({glm::quat(-0.983f, 0.f, 0.f, 0.182f), 5.f});
    lkneekf.push_back({glm::quat(-0.985f, 0.f, 0.f, 0.172f), 6.f});
    leftKnee->addAnimation(lkneekf, 6);

    Joint *leftAnkle = new Joint{"leftAnkle", leftKnee,
                                 glm::vec3(-0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, true, BoneType::CYLINDER};
    leftAnkle->addAnimation(defaultkf, 1);
    leftAnkle->addAnimation(defaultkf, 1);
    leftAnkle->addAnimation(defaultkf, 1);

    Joint *collar = new Joint{"collar", chest,
                              glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                              false, false, false, false, BoneType::NONE};
    collar->addAnimation(defaultkf, 1);
    collar->addAnimation(defaultkf, 1);
    collar->addAnimation(defaultkf, 1);

    Joint *neck = new Joint{"neck", collar,
                            glm::vec3(0.f, 0.25f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                            false, false, false, false, BoneType::CYLINDER};
    neck->addAnimation(defaultkf, 1);
    neck->addAnimation(defaultkf, 1);
    neck->addAnimation(defaultkf, 1);

    Joint *head = new Joint{"head", neck,
                            glm::vec3(0.f, 0.2f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                            false, false, false, false, BoneType::SPHERE};
    head->addAnimation(defaultkf, 1);
    head->addAnimation(defaultkf, 1);
    head->addAnimation(defaultkf, 1);

    return {chest, rightShoulder, rightElbow, rightWrist, leftShoulder, leftElbow, leftWrist,
                hip, rightHip, rightKnee, rightAnkle, leftHip, leftKnee, leftAnkle, collar, neck, head};
}

void Joint::drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color, glm::mat4 VP,
              GLuint shader, GLuint lineVAO, GLuint lineVBO) {
    float vertices[6] = {
        a.x, a.y, a.z,
        b.x, b.y, b.z
    };

    glUseProgram(shader);

    glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, &color[0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uVP"), 1, GL_FALSE, &VP[0][0]);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
}

// draws circle in x-y plane
void Joint::drawCircle(glm::vec3 c, float r, int param, glm::vec3 color,
                glm::mat4 VP, GLuint shader, GLuint vao, GLuint vbo)
{
    std::vector<float> vertices(3 * param);

    for(int i = 0; i < param; i++) {
        float theta = i * 2.0f * M_PI / param;
        vertices[3*i + 0] = c.x + r * cos(theta);
        vertices[3*i + 1] = c.y + r * sin(theta);
        vertices[3*i + 2] = c.z;
    }

    glUseProgram(shader);
    glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, &color[0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uVP"), 1, GL_FALSE, &VP[0][0]);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    glDrawArrays(GL_LINE_LOOP, 0, param);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
}
