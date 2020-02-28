[common]
#version 330 core

[vertex]
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec3 vColor;
//layout (location = 2) in vec2 vOffset;

out vec3 fColor;

void main() {
    //gl_Position = vec4(vPosition + vOffset, 0.0, 1.0);
    gl_Position = vec4(vPosition, 0.0, 1.0);
    fColor = vColor;
}  

[fragment]
in vec3 fColor;
out vec4 outColor;

void main() {
    outColor = vec4(fColor, 1);
}
