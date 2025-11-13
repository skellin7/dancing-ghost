#version 330 core

layout(location = 0) in vec3 objPos;
layout(location = 1) in vec3 objNormal;

out vec3 worldPos;
out vec3 worldNorm;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
    worldPos = vec3(modelMat * vec4(objPos, 1.0));
    worldNorm = transpose(inverse(mat3(modelMat))) * normalize(objNormal);

    gl_Position = projMat * viewMat * modelMat * vec4(objPos, 1.0);
}
