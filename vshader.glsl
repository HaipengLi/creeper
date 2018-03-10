attribute  vec4 vPosition;
attribute  vec4 vColor;
varying vec4 color;

uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;

in vec2 vTexPosition;
out vec2 TexPosition;
void main() {
    color = vColor;
    gl_Position = uniProjection * uniView * uniModel * vPosition;
} 
