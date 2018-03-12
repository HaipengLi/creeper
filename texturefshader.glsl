#version 330 core

in vec2 texPosition;
in vec3 vPositionWorldSpace;
in vec3 vNormalCameraSpace;
in vec3 eyeDirectionCmeraspace;
in vec3 lightDirectionCameraspace;


out vec3 color;

uniform sampler2D myTextureSampler;
uniform vec3 lightPositionWorldSpace;

void main() {
    vec3 lightColor = vec3(1, 1, 1);
    float lightPower = 60;

    vec3 materialDiffuseColor = texture(myTextureSampler, texPosition).rgb;
    vec3 materialAmbientColor = vec3(.5, .5, .5) * materialDiffuseColor;
    vec3 materialSpecularColor = vec3(.3, .3, .3);

    float distanceToLight = length(lightPositionWorldSpace - vPositionWorldSpace);

    vec3 n = normalize(vNormalCameraSpace);
    vec3 l = normalize(lightDirectionCameraspace);

    float cosTheta = clamp(dot(n, l), 0, 1);

    // eye vector to light
    vec3 eyeVector = normalize(eyeDirectionCmeraspace);

    vec3 r = reflect(-l, n);

    float cosAlpha = clamp(dot(eyeVector, r), 0, 1);

    color = materialAmbientColor +
            materialDiffuseColor * lightColor * lightPower * cosTheta / (distanceToLight * distanceToLight) +
            materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 2) / (distanceToLight * distanceToLight);
}