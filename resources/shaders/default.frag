#version 330 core
in vec3 worldPos;
in vec3 worldNorm;

out vec4 fragColor;

uniform float ka;
uniform float kd;
uniform float ks;

uniform vec4 camera;

struct Light {
    int type;             // 0=point, 1=directional, 2=spot
    vec3 function;
    vec4 color;
    vec4 pos;
    vec4 dir;
    float penumbra;
    float angle;
};

uniform int numLights;
uniform Light lights[8];

uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;

uniform float shininess;

void main() {
    vec3 normal = normalize(worldNorm);

    // fragColor = vec4((normal.x + 1) / 2, (normal.y + 1) / 2, (normal.z + 1) / 2, 1.0);

    // ambient
    fragColor = vec4(ka * cAmbient[0], ka * cAmbient[1], ka * cAmbient[2], 1.0);

    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        float fAtt;
        vec3 l;

        if (light.type == 1) { // directional
            fAtt = 1.f;
            l = -normalize(vec3(light.dir));
        }
        else if (light.type == 0) { // point
            float d = sqrt((light.pos.x-worldPos.x)*(light.pos.x-worldPos.x)
                      + (light.pos.y-worldPos.y)*(light.pos.y-worldPos.y)
                      + (light.pos.z-worldPos.z)*(light.pos.z-worldPos.z));
            fAtt = min(1.f, 1.f/(light.function[0] + d*light.function[1] + d*d*light.function[2]));
            l = normalize(vec3(light.pos) - worldPos);
        }
        else { // spot
            float d = sqrt((light.pos.x-worldPos.x)*(light.pos.x-worldPos.x)
                      + (light.pos.y-worldPos.y)*(light.pos.y-worldPos.y)
                      + (light.pos.z-worldPos.z)*(light.pos.z-worldPos.z));
            fAtt = min(1.f, 1.f/(light.function[0] + d*light.function[1] + d*d*light.function[2]));
            l = normalize(vec3(light.pos) - worldPos);

            vec3 lightDir = -normalize(vec3(light.dir));
            float x = acos(dot(l, lightDir));
            float outer = light.angle;
            float inner = outer - light.penumbra;
            if (x > outer) {
                fAtt = 0;
            }
            else if (x > inner) {
                fAtt *= (1 + 2 * pow((x-inner)/(outer-inner), 3) - 3 * pow((x-inner)/(outer-inner), 2));
            }
        }

        // diffuse
        float dp = clamp(dot(normal, l), 0.f, 1.f);
        vec3 diffuse = vec3(kd * dp * cDiffuse[0], kd * dp * cDiffuse[1], kd * dp * cDiffuse[2]);

        // specular
        vec3 r = normalize(-l - 2.f * normal * dot(normal, -l));
        vec3 toCamera = normalize(vec3(camera) - worldPos);
        dp = pow(clamp(dot(r, toCamera), 1e-4f, 1.f), shininess);
        vec3 specular = vec3(ks * dp * cSpecular[0], ks * dp * cSpecular[1], ks * dp * cSpecular[2]);

        fragColor[0] += (fAtt * light.color[0] * (diffuse[0] + specular[0]));
        fragColor[1] += (fAtt * light.color[1] * (diffuse[1] + specular[1]));
        fragColor[2] += (fAtt * light.color[2] * (diffuse[2] + specular[2]));
    }
}
