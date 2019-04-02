#shader vertex 
#version 330

layout (location = 0) in vec4 vertex;

out vec2 fUVTexture;

uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * vec4(vertex.xy, -1, 1);
    fUVTexture = vertex.zw;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330

in vec2 fUVTexture;
out vec4 outColor;

uniform sampler2D text;
uniform vec3 color;

void main() {    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, fUVTexture).r);
	outColor = vec4(color, 1.0) * sampled;
} 