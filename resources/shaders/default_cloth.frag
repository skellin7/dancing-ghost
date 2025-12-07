#version 330 core
in vec3 worldSpacePosition;

//uncomment for rendering cloth via normals
//in vec3 worldSpaceNormal;

//uncomment for rendering cloth via lines and points
in vec3 springColor;

out vec4 fragColor;




void main() {

    //uncomment for rendering cloth via normals
    //fragColor = vec4(worldSpaceNormal * 0.5 + 0.5, 1.0);

    //uncomment for rendering cloth via lines and points
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    if (springColor == vec3(0, 0, 0)) {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else {
        fragColor = vec4(springColor, 1.0);
    }
}
