#include "joint.h"

Joint::Joint(std::string name, Joint* parent, glm::vec3 localPosition, glm::quat localRotation,
             bool dofX, bool dofY, bool dofZ, bool endJoint, BoneType boneType)
    : m_name(name), m_parent(parent), m_localPosition(localPosition), m_localRotation(localRotation),
      m_dofX(dofX), m_dofY(dofY), m_dofZ(dofZ), m_endJoint(endJoint), m_boneType(boneType)
{
    computeFK();
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

std::vector<Joint*> Joint::setupSkeleton() {
    Joint *chest = new Joint("chest", nullptr,
                             glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                             false, false, false, false, BoneType::NONE);

    Joint *rightShoulder = new Joint{"rightShoulder", chest,
                                     glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                     false, false, true, false, BoneType::NONE};

    Joint *rightElbow = new Joint{"rightElbow", rightShoulder,
                                  glm::vec3(0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                  false, false, true, false, BoneType::CYLINDER};

    Joint *rightWrist = new Joint{"rightWrist", rightElbow,
                                  glm::vec3(0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                  false, false, true, true, BoneType::CYLINDER};

    Joint *leftShoulder = new Joint{"leftShoulder", chest,
                                    glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                    false, false, true, false, BoneType::NONE};

    Joint *leftElbow = new Joint{"leftElbow", leftShoulder,
                                 glm::vec3(-0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, false, BoneType::CYLINDER};

    Joint *leftWrist = new Joint{"leftWrist", leftElbow,
                                 glm::vec3(-0.5f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, true, BoneType::CYLINDER};

    Joint *hip = new Joint{"hip", chest,
                           glm::vec3(0.f, -0.75f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                           false, false, false, false, BoneType::CYLINDER};

    Joint *rightHip = new Joint{"rightHip", hip,
                                glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                false, false, true, false, BoneType::NONE};

    Joint *rightKnee = new Joint{"rightKnee", rightHip,
                                 glm::vec3(0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, false, BoneType::CYLINDER};

    Joint *rightAnkle = new Joint{"rightAnkle", rightKnee,
                                 glm::vec3(0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, true, BoneType::CYLINDER};

    Joint *leftHip = new Joint{"leftHip", hip,
                               glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                               false, false, true, false, BoneType::NONE};

    Joint *leftKnee = new Joint{"leftKnee", leftHip,
                                 glm::vec3(-0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, false, BoneType::CYLINDER};

    Joint *leftAnkle = new Joint{"leftAnkle", leftKnee,
                                 glm::vec3(-0.25f, -0.375f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                                 false, false, true, true, BoneType::CYLINDER};

    Joint *collar = new Joint{"collar", chest,
                              glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                              false, false, false, false, BoneType::NONE};

    Joint *neck = new Joint{"neck", collar,
                            glm::vec3(0.f, 0.25f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                            false, false, false, false, BoneType::CYLINDER};

    Joint *head = new Joint{"head", neck,
                            glm::vec3(0.f, 0.4f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f),
                            false, false, false, true, BoneType::SPHERE};

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
