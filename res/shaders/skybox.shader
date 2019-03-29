#shader vertex
#version 330
layout (location = 0) in vec3 vposition;

out vec3 ftextureUV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
    ftextureUV = vposition;
    gl_Position = projectionMatrix * vec4((viewMatrix * vec4(vposition, 0.0)).xyz, 1.0);
}  

//////////////////////////////////////////////////////////////

#shader fragment
#version 330

out vec4 outColor;

in vec3 ftextureUV;

uniform samplerCube skybox;

void main() {    
    outColor = texture(skybox, ftextureUV);
}