#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <glm/glm.hpp>

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 25;
    int shapeParameter2 = 25;
    float nearPlane = 0.1f;
    float farPlane = 100.f;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;

    float headRadius = 0.2f;
    float forearmLength = 0.5f;
    float upperarmLength = 0.5f;
    float thighLength = 0.5f;
    float calfLength = 0.5f;
    float bodyLength = 0.5f;

    float structuralK = 50;
    float shearK = 25;
    float bendK = 5;
    float damping = .2;
    glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f);
    float x_clothBottomLeft = -1.f;
    float y_clothBottomLeft = 0.f;
    float z_clothBottomLeft = -1.f;

    float cloth_width = 2.f;
    float cloth_width_step = 0.3f;
    float cloth_length = 2.f; //depth
    float cloth_length_step = 0.3f;

    float mu_static = 0.5f; //static friction  0.5f;
    float mu_kinetic = 0.9f; //kinetic friction 0.3f

    float clothToShapeCollisionCorrection; // for cloth to shape collisions

    //for cloth to cloth collisions
    float clothVertexRadius;
    float clothToClothCollisionCorrection = 0.001;

    bool renderVertices = false; //false = render cloth as normals, true = render cloth as vertices

};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
