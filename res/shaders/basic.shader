#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec4 position; 

void main() { 
	gl_Position = position; 
}

#shader fragment // fragment shader
#version 330 core 

layout(location = 0) out vec4 color; 

void main() { 
	color = vec4(gl_FragCoord.xy, 0.0, 1.0); 
}