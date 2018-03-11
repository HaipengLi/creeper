#version 330 

in vec3 vPosition;
in vec2 vTexPosition;
out vec2 texPosition;

uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;

void main() {
    // output the final vertex position in device coordinates
    // The final position of the vertex is assigned to the special `gl_Position` variable
    gl_Position = uniProjection * uniView * uniModel* vec4(vPosition, 1);
    // output any data the fragment shader requires. 
    texPosition = vTexPosition;
}
