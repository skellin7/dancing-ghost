#version 330 core
layout(location = 0) in vec3 objectSpacePosition;

//uncomment for rendering cloth via normals
//layout(location = 1) in vec3 objectSpaceNormal;

//uncomment for rendering cloth via lines and points
layout(location = 1) in vec3 color;

out vec3 worldSpacePosition;

//uncomment for rendering cloth via normals
//out vec3 worldSpaceNormal;

//uncomment for rendering cloth via lines and points
out vec3 springColor;


uniform mat4 inverseModelMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {

    worldSpacePosition = vec3(modelMatrix * vec4(objectSpacePosition, 1.0));

    //uncomment for rendering cloth via normals
    //worldSpaceNormal = normalize((transpose(mat3(inverseModelMatrix))) * objectSpaceNormal);

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(objectSpacePosition, 1.0);

    //uncomment for rendering cloth via lines and points
    springColor = color;
}
