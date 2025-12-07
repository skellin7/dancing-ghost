#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uVP;   // view-projection matrix

void main() {
    gl_Position = uVP * vec4(aPos, 1.0);
}
