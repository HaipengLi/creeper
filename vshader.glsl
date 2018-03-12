#version 330 

in vec3 vPositionModelSpace;
out vec3 vPositionWorldSpace;

in vec2 vTexPosition;
out vec2 texPosition;

in vec3 vNormalModelSpace;
out vec3 vNormalCameraSpace;

out vec3 eyeDirectionCmeraspace;
out vec3 lightDirectionCameraspace;

uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;

uniform vec3 lightPositionWorldSpace;

void main() {
    // output the final vertex position in device coordinates
    // The final position of the vertex is assigned to the special `gl_Position` variable
    gl_Position = uniProjection * uniView * uniModel * vec4(vPositionModelSpace, 1);
    vPositionWorldSpace = (uniModel * vec4(vPositionModelSpace, 1)).xyz;

    vNormalCameraSpace = (uniView * uniModel * vec4(vNormalModelSpace, 0)).xyz;
    // output any data the fragment shader requires. 
    texPosition = vTexPosition;

    vec3 vPositionCameraSpace= ( uniView * uniModel * vec4(vPositionModelSpace,1)).xyz;
	eyeDirectionCmeraspace = vec3(0,0,0) - vPositionCameraSpace;

    vec3 lightPositionCameraspace = (uniView * vec4(lightPositionWorldSpace, 1)).xyz;
    lightDirectionCameraspace = lightPositionCameraspace + eyeDirectionCmeraspace;
}
