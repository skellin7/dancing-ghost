#version 330 core
in vec3 worldSpacePosition;
in vec3 springColor;

out vec4 fragColor;


void main() {
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    if (springColor == vec3(0, 0, 0)) {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else {
        fragColor = vec4(springColor, 1.0);
    }
}
