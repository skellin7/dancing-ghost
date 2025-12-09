#version 330 core
in vec3 worldSpacePosition;
in vec2 uv_coordinate;

uniform sampler2D my_texture;

uniform vec3 bottomLeft;
uniform float clothWidth;
uniform float clothDepth;

out vec4 fragColor;


void main() {
    fragColor = vec4(1);
    fragColor = texture(my_texture, uv_coordinate);
}
