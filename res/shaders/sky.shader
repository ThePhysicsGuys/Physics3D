[common]

#version 330 core

[vertex]

layout(location = 0) in vec4 vPosition;

out vec2 fUV;

void main() {
	gl_Position = vec4(vPosition.xy, 0.0, 1.0);
	fUV = vPosition.zw;
}

[fragment]

in vec2 fUV;
out vec4 outColor;

uniform vec3 viewPosition;
uniform vec3 viewDirection;
uniform vec2 resolution;
uniform sampler2D image;
uniform float time;

vec3 nightColor   = vec3(.15, 0.3, 0.6);
vec3 horizonColor = vec3(0.6, 0.3, 0.4);
vec3 dayColor     = vec3(0.7,0.8,1);
vec3 sunColor     = vec3(1.0,0.8,0.6);
vec3 sunRimColor  = vec3(1.0,0.66,0.33);

float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f * f * f * (3.0 - 2.0 * f);
	vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
	vec4 rg = texture(image, (uv + 0.5) / 256.0, -100.0);
	return (-1.0 + 2.0 * mix(rg.g, rg.r, f.z));
}

vec4 render(vec3 ro, vec3 rd) {
    float sunSpeed = 0.35 * time;
    vec3 sunDirection = normalize(vec3(cos(sunSpeed), sin(sunSpeed), 0.0));
	float sun = clamp(dot(sunDirection, rd), 0.0, 1.0);
    float sunHeight = sunDirection.y;
    
    // below this height will be full night color
    float nightHeight = -0.8;
    // above this height will be full day color
    float dayHeight   = 0.3;
    
    float horizonLength = dayHeight - nightHeight;
    float inverseHL = 1.0 / horizonLength;
    float halfHorizonLength = horizonLength / 2.0;
    float inverseHHL = 1.0 / halfHorizonLength;
    float midPoint = nightHeight + halfHorizonLength;
    
    float nightContribution = clamp((sunHeight - midPoint) * (-inverseHHL), 0.0, 1.0);
    float horizonContribution = -clamp(abs((sunHeight - midPoint) * (-inverseHHL)), 0.0, 1.0) + 1.0;
    float dayContribution = clamp((sunHeight - midPoint) * ( inverseHHL), 0.0, 1.0);
    
    // sky color
    vec3 skyColor = vec3(0.0);
    skyColor += mix(vec3(0.0),   nightColor, nightContribution);   // Night
    skyColor += mix(vec3(0.0), horizonColor, horizonContribution); // Horizon
    skyColor += mix(vec3(0.0),     dayColor, dayContribution);     // Day
    
	vec3 color = skyColor;
    
    // atmosphere brighter near horizon
    color -= clamp(rd.y, 0.0, 0.5);
    
    // draw sun
	color += 0.4 * sunRimColor * pow(sun,    4.0);
	color += 1.0 * sunColor    * pow(sun, 2000.0);
    
    // stars
    float starSpeed = -sunSpeed * 0.5;
    
    float starContribution = clamp((sunHeight - dayHeight) * (-inverseHL), 0.0, 1.0);
    
    vec3 starDirection = rd * mat3( vec3(cos(starSpeed), -sin(starSpeed), 0.0),
                                    vec3(sin(starSpeed),  cos(starSpeed), 0.0),
                                    vec3(0.0,             0.0,            1.0));
                              
    color += pow((texture(image, starDirection.xy).r + texture(image, starDirection.zy).r) * 0.5, 42.0) * starContribution * 40.0;
    return vec4(color, 1.0);
}

mat3 setCamera(vec3 ro, vec3 ta, float cr) {
	vec3 cw = normalize(ta - ro);
	vec3 cp = vec3(sin(cr), cos(cr), 0.0);
	vec3 cu = normalize(cross(cw,cp));
	vec3 cv = normalize(cross(cu,cw));
    return mat3(cu, cv, cw);
}

void main() {
	// uv
	vec2 p = (fUV - vec2(0.5)) * 2.0;

    // camera
	vec3 ro = viewDirection;
	vec3 ta = vec3(0.0, -1.0, 0.0);
    mat3 ca = setCamera(ro, ta, 0.0);

    // ray
    vec3 rd = ca * normalize(vec3(p, 1.5));
    
	// color
	outColor = render(ro, rd);
	//outColor = vec4(p, 0.0, 1.0);
}