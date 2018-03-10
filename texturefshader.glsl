#version 330 core

in vec2 texPosition;

out vec3 color;

uniform sampler2D myTextureSampler;

void main() {
    color = texture(myTextureSampler, texPosition).rgb;
}