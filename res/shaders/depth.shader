#shader vertex
#version 330 core 

layout(location = 0) in vec3 vposition; 
uniform mat4 modelMatrix;
uniform mat4 lightMatrix;

void main() {
	gl_Position = lightMatrix * modelMatrix * vec4(vposition, 1.0);
}

/////////////////////////////////////////////////////////////////////

#shader fragment 
#version 330 core

void main() {}