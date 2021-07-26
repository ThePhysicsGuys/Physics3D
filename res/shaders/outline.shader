[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec4 vPositionUV;

out vec2 fUV;

void main() {
    gl_Position = vec4(vPositionUV.xy, 0, 1);
    fUV = vPositionUV.zw;
}

//------------------------------------------------------------------------------//

[fragment]

in vec2 fUV;
out vec4 outColor;

uniform sampler2D image;
uniform vec4 color = vec4(1, 1, 1, 1);

mat3 sx = mat3(
     1.0,  2.0,  1.0,
     0.0,  0.0,  0.0,
    -1.0, -2.0, -1.0
);

mat3 sy = mat3(
    1.0, 0.0, -1.0,
    2.0, 0.0, -2.0,
    1.0, 0.0, -1.0
);

void main() {
    vec3 diffuse = texture(image, fUV.st).rgb;
    mat3 I;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 sample = texelFetch(image, ivec2(gl_FragCoord) + ivec2(i - 1, j - 1), 0).rgb;
            I[i][j] = length(sample);
        }
    }

    float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]);
    float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);
    float g = clamp(gx * gx + gy * gy, 0.0, 1.0);

    outColor = vec4(g) * color;
}