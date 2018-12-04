#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec3 position; 

uniform mat4 viewMatrix;
// uniform mat4 modelViewMatrix;
// uniform mat4 projectionMatrix;
// uniform vec2 vResolution;
// out vec2 fResolution;
void main() { 
	mat4 M = mat4(
		vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(0.0, 1.0, 0.0, 1.0)
	);
	gl_Position = viewMatrix * vec4(position, 1);
}

#shader fragment // fragment shader
#version 330 core 

layout(location = 0) out vec4 color; 
//in vec2 fResolution;

void main() {
	color = vec4(gl_FragCoord.xy / 500, cos(cos(gl_FragCoord.x / 5) * cos(gl_FragCoord.y / 5)), 1.0);
}