#shader vertex 
#version 330 

layout(location = 0) in vec3 vposition; 

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() { 
	gl_Position = projectionMatrix * viewMatrix * transpose(modelMatrix) * vec4(vposition, 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330

out vec4 outColor;

void main() {
	outColor = vec4(1, 0, 0, 1);
}