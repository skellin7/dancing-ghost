#version 330 core
layout(location = 0) in vec3 objectSpacePosition;
layout(location = 1) in vec3 color;

out vec3 worldSpacePosition;
out vec3 springColor;


uniform mat4 inverseModelMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    worldSpacePosition = vec3(modelMatrix * vec4(objectSpacePosition, 1.0));
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(objectSpacePosition, 1.0);
    springColor = color;
}
