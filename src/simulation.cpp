#include "src/realtime.h"
#include "src/settings.h"


void Realtime::simulate(float deltaTime) {
    std::vector<glm::vec3> forces = computeForces(deltaTime);
    verletIntegration(forces, deltaTime);

    for (int i = 0; i < 5; i++) {
        constrainSprings(1);
        solveClothToClothCollisions(1, deltaTime);
        solveCollisions(1, deltaTime);

    }
}


std::vector<glm::vec3> Realtime::computeForces(float deltaTime) {

    std::vector<glm::vec3> forces(m_cloth->m_vertices.size(), glm::vec3(0.0f));

    //adding gravity
    for (int i = 0; i < m_cloth->m_vertices.size(); i++) {
        if (!m_cloth->m_vertices[i].anchored) {
            forces[i] += settings.gravity;
        }
    }

    //adding spring forces, hooks law
    for (int i = 0; i < m_cloth->m_springs.size(); i++) {
        Spring* s = &m_cloth->m_springs[i];

        glm::vec3* v1_pos = &m_cloth->m_vertices[s->vertexOne].pos;
        glm::vec3* v2_pos = &m_cloth->m_vertices[s->vertexTwo].pos;

        glm::vec3 vector = *v1_pos - *v2_pos; //vector from B(neighbor) to A(current). A = v1, B = v2
        float magnitude = glm::length(vector);
        glm::vec3 direction = glm::normalize(vector);


        glm::vec3 springForce = -s->k * (magnitude - s->rest_length) * direction; //force on A
        forces[s->vertexOne] += springForce; //A + force
        forces[s->vertexTwo] -= springForce; // B - force
    }

    //adding dampening to spring forces
    for (int i = 0; i < m_cloth->m_springs.size(); i++) {
        Spring* s = &m_cloth->m_springs[i];

        glm::vec3* v1_pos = &m_cloth->m_vertices[s->vertexOne].pos;
        glm::vec3* v1_prev_pos = &m_cloth->m_vertices[s->vertexOne].prev_pos;

        glm::vec3* v2_pos = &m_cloth->m_vertices[s->vertexTwo].pos;
        glm::vec3* v2_prev_pos = &m_cloth->m_vertices[s->vertexTwo].prev_pos;

        glm::vec3 v1_velocity = (*v1_pos - *v1_prev_pos) / deltaTime;
        glm::vec3 v2_velocity = (*v2_pos - *v2_prev_pos) / deltaTime;

        glm::vec3 vector = *v1_pos - *v2_pos; //vector from B(neighbor) to A(current). A = v1, B = v2
        float magnitude = glm::length(vector);
        glm::vec3 direction = glm::normalize(vector);

        glm::vec3 dampeningForce = -s->dampness * glm::dot((v1_velocity - v2_velocity), vector)/magnitude  * direction; //force on A
        forces[s->vertexOne] += dampeningForce; //A + force
        forces[s->vertexTwo] -= dampeningForce; // B - force
    }

    return forces;
}


void Realtime::verletIntegration(std::vector<glm::vec3> forces, float deltaTime) {
    //verlet integration
    //x_t+1 = 2*x_t - x_t-1 + (dv/dt)_t * (delta t * delta t)
    for (int i = 0; i < m_cloth->m_vertices.size(); i++) {
        Vertex* v = &m_cloth->m_vertices[i];

        if (!v->anchored) {
            glm::vec3 a = (forces[i] + v->forces) / v->mass; //a = F/m
            glm::vec3 newPos = 2.0f * v->pos - v->prev_pos + a * deltaTime * deltaTime;

            v->prev_pos = v->pos;
            v->pos = newPos;
        }
    }
}


void Realtime::solveCollisions(int iterations, float deltaTime) {
    // for (int iter = 0; iter < iterations; iter++) {

        // for (int i = 0; i < m_cloth->m_vertices.size(); i++) {
        //     Vertex* v = &m_cloth->m_vertices[i];

        //     if (v->anchored) {
        //         continue;
        //     }

        //     if (m_renderData.shapes.size() == 0) {
        //         continue;
        //     }

        //     else {
        //         glm::vec3 newPos = v->pos;

        //         for (const RenderShapeData& shape : m_renderData.shapes) {

        //             if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {

        //                 glm::vec3 newPosOS = glm::vec3(glm::inverse(shape.ctm) * glm::vec4(newPos, 1.0f));
        //                 glm::vec3 sphereCenter = glm::vec4(0,0,0,1.0f);

        //                 glm::vec3 centerToNewPos = newPosOS - sphereCenter;
        //                 float distance = glm::length(centerToNewPos);
        //                 //float radius = 0.5f * glm::length(glm::vec3(shape.ctm[0]));
        //                 float radius = 0.5f;

        //                 //repulsion correction
        //                 float epsilon = settings.clothToShapeCollisionCorrection;

        //                 //vertex is inside the sphere
        //                 if (distance < radius + epsilon) {

        //                     glm::vec3 normalOS = glm::normalize(centerToNewPos);

        //                     //position correction in Object Space
        //                     glm::vec3 repelledPosOS = sphereCenter + normalOS * (radius + epsilon);

        //                     //position correction in World Space
        //                     glm::vec3 repelledPosWS = glm::vec3(shape.ctm * glm::vec4(repelledPosOS, 1.0f));
        //                     newPos = repelledPosWS;

        //                     glm::vec3 velocity = (repelledPosWS - v->prev_pos) / deltaTime;

        //                     //friction
        //                     glm::vec3 normalWS = glm::transpose(glm::inverse(glm::mat3(shape.ctm))) * normalOS;
        //                     normalWS = glm::normalize(normalWS);
        //                     glm::vec3 frictionalForce = friction(velocity, normalWS);
        //                     v->forces += frictionalForce;
        //                 }

        //                 v->pos = newPos;
        //             }

        //             if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) { //sticking to the bottom fix that

        //                 float margin = 0.1f;
        //                 glm::vec3 newPosOS = glm::vec3(glm::inverse(shape.ctm) * glm::vec4(newPos, 1.0f));

        //                 //vertex is inside cube
        //                 if (fabs(newPosOS.x) <= 0.5f + margin && fabs(newPosOS.y) <= 0.5f + margin && fabs(newPosOS.z) <= 0.5f + margin) {

        //                     //distance to each face
        //                     float distance_x = 0.5 - fabs(newPosOS.x);
        //                     float distance_y = 0.5 - fabs(newPosOS.y);
        //                     float distance_z = 0.5 - fabs(newPosOS.z);

        //                     glm::vec3 normalOS(0.f);
        //                     glm::vec3 repelledPosOS = newPosOS;

        //                     //repulsion correction
        //                     float epsilon = settings.clothToShapeCollisionCorrection;

        //                     //find closest face
        //                     //position correction in Object Space
        //                     if (distance_x <= distance_y && distance_x <= distance_z) {
        //                         repelledPosOS.x = (newPosOS.x > 0 ? 0.5f + epsilon: -0.5f - epsilon);
        //                         normalOS = glm::vec3((newPosOS.x > 0 ? 1 : -1), 0, 0);
        //                     }
        //                     else if (distance_y <= distance_x && distance_y <= distance_z) {
        //                         repelledPosOS.y = (newPosOS.y > 0 ? 0.5f + epsilon: -0.5f - epsilon);
        //                         normalOS = glm::vec3(0, (newPosOS.y > 0 ? 1 : -1), 0);
        //                     }
        //                     else if (distance_z <= distance_x && distance_z <= distance_y) {
        //                         repelledPosOS.z = (newPosOS.z > 0 ? 0.5f + epsilon: -0.5f - epsilon);
        //                         normalOS = glm::vec3(0, 0, (newPosOS.z > 0 ? 1 : -1));
        //                     }

        //                     //position correction in World Space
        //                     glm::vec3 repelledPosWS = glm::vec3(shape.ctm * glm::vec4(repelledPosOS, 1.0f));
        //                     newPos = repelledPosWS;

        //                     glm::vec3 velocity = (repelledPosWS - v->prev_pos) / deltaTime;

        //                     //friction
        //                     glm::vec3 normalWS = glm::transpose(glm::inverse(glm::mat3(shape.ctm))) * normalOS;
        //                     normalWS = glm::normalize(normalWS);
        //                     glm::vec3 frictionalForce = friction(velocity, normalWS);
        //                     v->forces += frictionalForce;
        //                 }

        //                 v->pos = newPos;
        //             }

        //             if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {

        //                 glm::vec3 newPosOS = glm::vec3(glm::inverse(shape.ctm) * glm::vec4(newPos, 1.0f));

        //                 float radius = 0.5f;
        //                 //float radius = 0.5f * glm::length(glm::vec3(shape.ctm[0]));
        //                 float epsilon = settings.clothToShapeCollisionCorrection;

        //                 //distance from axis
        //                 float distance = glm::length(glm::vec2(newPosOS.x, newPosOS.z));

        //                 glm::vec3 cylinderCenter = glm::vec4(0,0,0,1.0f);
        //                 glm::vec3 centerToNewPos = newPosOS - cylinderCenter;

        //                 //vertex is inside cylinder
        //                 if (distance < radius + epsilon && newPosOS.y > -0.5f - epsilon && newPosOS.y < 0.5f + epsilon) {

        //                     glm::vec3 normalOS(0.f);
        //                     glm::vec3 repelledPosOS = newPosOS;

        //                     // Distances to boundaries
        //                     float distanceToSide = radius - distance;
        //                     float distanceToTop  = 0.5f - newPosOS.y;
        //                     float distanceToBottom  = newPosOS.y + 0.5f;

        //                     //find closest boundary
        //                     if (distanceToSide <= distanceToTop && distanceToSide <= distanceToBottom) {
        //                         //sides of cylinder
        //                         glm::vec2 direction = glm::normalize(glm::vec2(newPosOS.x, newPosOS.z));
        //                         normalOS = glm::vec3(direction.x, 0.f, direction.y);
        //                         repelledPosOS.x = direction.x * (radius + epsilon);
        //                         repelledPosOS.z = direction.y * (radius + epsilon);

        //                         //repelledPosOS.x = direction.x + normalOS.x * (radius + epsilon);
        //                         //repelledPosOS.z = direction.y + normalOS.z * (radius + epsilon);
        //                     }
        //                     else if (distanceToTop <= distanceToBottom) {
        //                         //top cap
        //                         normalOS = glm::vec3(0, 1, 0);
        //                         //normalOS = glm::normalize(centerToNewPos);

        //                         //repelledPosOS.y = distanceToTop + epsilon;

        //                         repelledPosOS.y = 0.5f + epsilon;

        //                     }
        //                     else {
        //                         //bottom cap
        //                         normalOS = glm::vec3(0, -1, 0);
        //                         //normalOS = glm::normalize(centerToNewPos);

        //                         //repelledPosOS.y = distanceToBottom - epsilon;

        //                         repelledPosOS.y = -0.5f - epsilon;
        //                         //repelledPosOS.y = cylinderCenter.y + normalOS.y * (-0.5f - epsilon);
        //                     }

        //                     //position correction in World Space
        //                     glm::vec3 repelledPosWS = glm::vec3(shape.ctm * glm::vec4(repelledPosOS, 1.0f));
        //                     newPos = repelledPosWS;

        //                     glm::vec3 velocity = (repelledPosWS - v->prev_pos) / deltaTime;

        //                     //friction
        //                     glm::vec3 normalWS = glm::transpose(glm::inverse(glm::mat3(shape.ctm))) * normalOS;
        //                     normalWS = glm::normalize(normalWS);
        //                     glm::vec3 frictionalForce = friction(velocity, normalWS);
        //                     v->forces += frictionalForce;
        //                 }

        //                 v->pos = newPos;

        //             }


        //         }
        //     }
        // }
    //}
}


void::Realtime::solveClothToClothCollisions(int iterations, float deltaTime) {
    //for (int iter = 0; iter < iterations; iter++) {

        for (int i = 0; i < m_cloth->m_vertices.size(); i++) {

            Vertex* v1 = &m_cloth->m_vertices[i];

            for (int j = 0; j < m_cloth->m_vertices.size(); j++) {

                if (i == j) { //same vertex
                    continue;
                }

                bool isNeighbor = false;
                for (int n : v1->neighbors) {
                    if (n == j) { //m_vertices[j] is a neighbor of m_vertices[i]
                        isNeighbor = true;
                        break;
                    }
                }

                if (isNeighbor) {
                    continue;
                }

                Vertex* v2 = &m_cloth->m_vertices[j];
                float distance = glm::length(v2->pos - v1->pos); //distance between vertices

                if (distance < 2*settings.clothVertexRadius) {
                    glm::vec3 direction = glm::normalize(v2->pos - v1->pos); //direction from v1 to v2
                    float overlap = 2*settings.clothVertexRadius - distance;
                    v1->pos -= 0.5f * overlap * direction + settings.clothToClothCollisionCorrection;
                    v2->pos += 0.5f * overlap * direction - settings.clothToClothCollisionCorrection;
                }
            }
        }
    //}
}


void Realtime::constrainSprings(int iterations) {
    //for (int iter = 0; iter < iterations; iter++) {
        for (Spring& s : m_cloth->m_springs) {

            Vertex* v1 = &m_cloth->m_vertices[s.vertexOne];
            Vertex* v2 = &m_cloth->m_vertices[s.vertexTwo];

            float distance = glm::length(v2->pos - v1->pos); //distance between vertices
            glm::vec3 direction = glm::normalize(v2->pos - v1->pos); //direction from v1 to v2

            if (distance < 1e-6f) { //no distance between the two vertices (spring length effectively 0)
                continue;
            }

            float stretch = distance - s.rest_length;
            float maxStretch = s.rest_length * 0.1f; //spring can stretch 10%

            //stretch correction
            if (fabs(stretch) > maxStretch) {
                float stretchCorrection = stretch - (stretch > 0 ? maxStretch : -maxStretch);

                if (!v1->anchored && !v2->anchored) {
                    v1->pos += 0.5f * direction * stretchCorrection;
                    v2->pos -= 0.5f * direction * stretchCorrection;
                }
                else if (!v1->anchored) {
                    v1->pos += direction * stretchCorrection;
                }
                else if (!v2->anchored) {
                    v2->pos -= direction * stretchCorrection;
                }
            }
        }
    //}
}


glm::vec3 Realtime::friction(glm::vec3 velocity, glm::vec3 normal) {
    //calculating tangential velocity (velocity of cloth sliding along surface of sphere)

    glm::vec3 perpendicularComponent = glm::dot(velocity, normal) * normal; //velocity component along normal
    glm::vec3 tangent_velocity = velocity - perpendicularComponent; //velocity component along surface (parallel/tangent)
    float tangent_velocity_len = glm::length(tangent_velocity);

    //calculating friction
    glm::vec3 friction = glm::vec3(0.f);

    if (tangent_velocity_len < 1e-4f) { //little tangential velocity => apply static friction
        glm::vec3 desired_friction =  -tangent_velocity; //friction needed to stop motion
        float maxStaticFriction = settings.mu_static * fabs(glm::dot(velocity, normal));

        if (glm::length(desired_friction) > maxStaticFriction) {
            friction = -maxStaticFriction * glm::normalize(tangent_velocity);
        }
        else {
            friction = desired_friction;
        }
    }
    else { //cloth moving along sphere
        //kinetic friction
        friction = -settings.mu_kinetic * tangent_velocity;
    }

    return friction;
}

