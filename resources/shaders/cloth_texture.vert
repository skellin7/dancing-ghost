#version 330 core
layout(location = 0) in vec3 objectSpacePosition;
layout(location = 2) in vec2 uv;

out vec3 worldSpacePosition;
out vec2 uv_coordinate;


uniform mat4 inverseModelMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {

    worldSpacePosition = vec3(modelMatrix * vec4(objectSpacePosition, 1.0));

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(objectSpacePosition, 1.0);

    uv_coordinate = vec2(uv);

}
