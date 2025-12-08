#version 330 core
in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;

out vec4 fragColor;


void main() {
    fragColor = vec4(worldSpaceNormal * 0.5 + 0.5, 1.0);
}
