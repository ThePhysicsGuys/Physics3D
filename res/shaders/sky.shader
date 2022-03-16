[properties]
float densityFalloff (uniform_ddkd) = 4 (0:100);
int inScatterPoints = 10 (1:100);
int opticalDepthPoints = 10 (1:100);
vec3 sunPosition = 10.0, 20.0, 0.0;
vec3 waveLengths = 700, 530, 440;
float scatteringStrength = 1.0 (1.0:10.0);
float atmosphereScale = 1.0 (0.0:5.0);

[common]

#version 330 core

const float INFINITY = 1.0 / 0.0;
const float EPSILON = 0.0001;
const float PI = 3.14159265358979323846;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform float densityFalloff = 4.0;
uniform int inScatterPoints = 10;
uniform int opticalDepthPoints = 10;
uniform vec3 sunPosition = vec3(10.0, 20.0, 0.0);
uniform vec3 waveLengths = vec3(700, 530, 440);
uniform float scatteringStrength = 1.0;
uniform float atmosphereScale = 1.0;

vec4 applyT(mat4 matrix, vec3 vector) {
	return matrix * vec4(vector, 1.0);
}

vec3 applyT3(mat4 matrix, vec3 vector) {
	return (matrix * vec4(vector, 1.0)).xyz;
}

///////////////////////////////////////////////////////////////////////////////////////

[vertex]

layout(location = 0) in vec4 vPositionUV;

smooth out vec2 fUV;

void main() {
	fUV = vPositionUV.zw;
	gl_Position = vec4(vPositionUV.xy, 0.0, 1.0);
}

///////////////////////////////////////////////////////////////////////////////////////

[fragment]

// Structs
struct Sphere {
	vec3 center;
	float radius;
};

struct Plane {
	vec3 center;
	vec3 normal;
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Hit {
	float t;
	float dt;
};

struct Screen {
	vec2 dimension;
};

// In
smooth in vec2 fUV;

// Out
out vec4 outColor;

uniform Sphere uAtmosphere = Sphere(vec3(0.0, 0.0, 0.0), 10.0);
uniform Plane uGround = Plane(vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
uniform Ray uCamera = Ray(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
uniform Screen uScreen = Screen(vec2(1.0, 1.0));

uniform sampler2D uOriginalColor;

// Functions
bool inside(vec3 point, Sphere sphere) {
	return length(point - sphere.center) <= sphere.radius;
}

bool above(vec3 point, Plane plane) {
	return dot(point - plane.center, plane.normal) > 0.0;
}

Hit hitSphere(Sphere sphere, Ray ray) {
	vec3 offset = ray.origin - sphere.center;

	float a = 1.0;
	float b = 2.0 * dot(offset, ray.direction);
	float c = dot(offset, offset) - sphere.radius * sphere.radius;
	float d = b * b - 4.0 * a * c;

	if (d > 0.0) {
		float s = sqrt(d);
		float near = (-b - s) / (2.0 * a);
		float far = (-b + s) / (2.0 * a);

		// Behind
		if (near < 0.0 && far < 0.0)
			return Hit(INFINITY, 0.0);

		// Inside
		if (near * far < 0.0)
			return Hit(max(near, far), 0.0);

		// Outside
		return Hit(min(near, far), 0.0);
	}

	// Miss
	return Hit(INFINITY, 0.0);
}

Hit hitPlane(Plane plane, Ray ray) {
	float denominator = dot(ray.direction, plane.normal);

	// Parallel
	if (abs(denominator) < EPSILON)
		return Hit(INFINITY, 0.0);

	float t = -dot(plane.center - ray.origin, plane.normal) / denominator;

	// Behind
	if (t < 0.0)
		return Hit(INFINITY, 0.0);

	return Hit(t, 0.0);
}

Hit hitDisk(Plane plane, Sphere sphere, Ray ray) {
	Hit planeHit = hitPlane(plane, ray);

	if (planeHit.t == INFINITY)
		return Hit(INFINITY, 0.0);

	vec3 point = ray.origin + planeHit.t * ray.direction;
	if (!inside(point, sphere))
		return Hit(INFINITY, 0.0);

	return planeHit;
}

Hit hitHemisphere(Plane plane, Sphere sphere, Ray ray) {
	Hit sphereHit = hitSphere(sphere, ray);
	Hit planeHit = hitPlane(plane, ray);
	Hit hit = Hit(min(sphereHit.t, planeHit.t), 0.0);

	return hit;
}

float densityAtPoint(vec3 densitySamplePoint) {
	float heightAboveSurface = dot(densitySamplePoint - uGround.center, uGround.normal);
	float atmosphereHeight = uAtmosphere.radius;
	float normalizedHeightAboveSurface = heightAboveSurface / atmosphereHeight;
	float localDensity = exp(-normalizedHeightAboveSurface * (densityFalloff / 10.0)) * (1.0 - normalizedHeightAboveSurface);

	return localDensity;
}

float opticalDepth(Ray ray, float rayLength) {
	vec3 densityAtSamplePoint = ray.origin;
	float stepSize = rayLength / float(opticalDepthPoints - 1);
	
	float opticalDepth = 0.0;
	for (int i = 0; i < opticalDepthPoints; i++) {
		float localDensity = densityAtPoint(densityAtSamplePoint);

		opticalDepth += localDensity * stepSize;
		densityAtSamplePoint += ray.direction * stepSize;
	}

	return opticalDepth;
}

vec3 calculateLight(Ray ray, float rayLength, vec3 originalColor) {
	vec3 scatteringCoefficients = pow(vec3(400.0) / waveLengths, vec3(4.0)) * scatteringStrength;

	vec3 inScatterPoint = ray.origin;
	float stepSize = rayLength / float(inScatterPoints - 1);
	float viewRayOpticalDepth = 0.0;
	vec3 inScatteredLight = vec3(0.0);
	for (int i = 0; i < inScatterPoints; i++) {
		Ray sunRay = Ray(inScatterPoint, normalize(sunPosition - inScatterPoint));
		Ray viewRay = Ray(inScatterPoint, -ray.direction);

		float sunRayLength = hitSphere(uAtmosphere, sunRay).t;
		float viewRayLength = stepSize * float(i);

		float sunRayOpticalDepth = opticalDepth(sunRay, sunRayLength);
		viewRayOpticalDepth = opticalDepth(viewRay, viewRayLength);

		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoefficients);
		float localDensity = densityAtPoint(inScatterPoint);

		inScatteredLight += localDensity * transmittance * scatteringCoefficients * stepSize;
		inScatterPoint += ray.direction * stepSize;
	}

	float originalColorTransmittance = exp(-viewRayOpticalDepth);

	return originalColor * originalColorTransmittance + normalize(inScatteredLight);
}

float atan2(float y, float x) {
	bool s = abs(x) > abs(y);
	return mix(PI / 2.0 - atan(x, y), atan(y, x), s);
}

const float pi = 3.14159265359;
const float invPi = 1.0 / pi;

const float zenithOffset = 0.1;
const float multiScatterPhase = 0.1;
const float density = 0.7;

const float anisotropicIntensity = 0.0; //Higher numbers result in more anisotropic scattering

const vec3 skyColor = vec3(0.39, 0.57, 1.0) * (1.0 + anisotropicIntensity); //Make sure one of the conponents is never 0.0

#define smooth(x) x*x*(3.0-2.0*x)
#define zenithDensity(x) density / pow(max(x - zenithOffset, 0.35e-2), 0.75)

vec3 getSkyAbsorption(vec3 x, float y) {

	vec3 absorption = x * -y;
	absorption = exp2(absorption) * 2.0;

	return absorption;
}

float getSunPoint(vec2 p, vec2 lp) {
	return smoothstep(0.03, 0.026, distance(p, lp)) * 50.0;
}

float getRayleigMultiplier(vec2 p, vec2 lp) {
	return 1.0 + pow(1.0 - clamp(distance(p, lp), 0.0, 1.0), 2.0) * pi * 0.5;
}

float getMie(vec2 p, vec2 lp) {
	float disk = clamp(1.0 - pow(distance(p, lp), 0.1), 0.0, 1.0);

	return disk * disk * (3.0 - 2.0 * disk) * 2.0 * pi;
}

vec3 getAtmosphericScattering(vec2 p, vec2 lp) {
	vec2 correctedLp = lp / max(uScreen.dimension.x, uScreen.dimension.y) * uScreen.dimension.xy;

	float zenith = zenithDensity(p.y);
	float sunPointDistMult = clamp(length(max(correctedLp.y + multiScatterPhase - zenithOffset, 0.0)), 0.0, 1.0);

	float rayleighMult = getRayleigMultiplier(p, correctedLp);

	vec3 absorption = getSkyAbsorption(skyColor, zenith);
	vec3 sunAbsorption = getSkyAbsorption(skyColor, zenithDensity(correctedLp.y + multiScatterPhase));
	vec3 sky = skyColor * zenith * rayleighMult;
	vec3 sun = getSunPoint(p, correctedLp) * absorption;
	vec3 mie = getMie(p, correctedLp) * sunAbsorption;

	vec3 totalSky = mix(sky * absorption, sky / (sky + 0.5), sunPointDistMult);
	totalSky += sun + mie;
	totalSky *= sunAbsorption * 0.5 + 0.5 * length(sunAbsorption);

	return totalSky;
}

vec3 jodieReinhardTonemap(vec3 c) {
	float l = dot(c, vec3(0.2126, 0.7152, 0.0722));
	vec3 tc = c / (c + 1.0);

	return mix(c / (l + 1.0), tc, tc);
}

// Main
void main() {
	float s = 1;
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 w = -uCamera.direction;
	vec3 u = cross(up, w);
	vec3 v = cross(w, u);
	vec3 cam = vec3((gl_FragCoord.xy - uScreen.dimension / 2.0) / uScreen.dimension.x * s, -1.0);
	//vec3 cam = vec3(fUV * 2.0 - vec2(1.0), -1);
	vec3 dir = normalize(cam.x * u + cam.y * v + cam.z * w);
	Ray ray = Ray(uCamera.origin, dir);

	float distanceThroughAtmosphere = hitHemisphere(uGround, uAtmosphere, ray).t;

	vec3 originalColor = texture(uOriginalColor, fUV).rgb;
	/*if (distanceThroughAtmosphere != INFINITY) {
		//vec3 pointInAtmosphere = ray.origin + ray.direction * (distanceThroughAtmosphere + EPSILON);
		vec3 light = calculateLight(Ray(ray.origin, ray.direction), distanceThroughAtmosphere - EPSILON * 2.0, originalColor);

		outColor = vec4(light, 1.0);
	} else {
		outColor = vec4(originalColor, 1.0);
	}*/

	/*if (distanceThroughAtmosphere != INFINITY)
		originalColor *= 0.6;

	outColor = vec4(originalColor, 1.0);*/
	
	//outColor = vec4(dir / 2.0 + vec3(0.5), 1.0);

	vec2 iMouse = uScreen.dimension / 2.0;
	vec2 position = gl_FragCoord.xy / max(uScreen.dimension.x, uScreen.dimension.y) * 2.0;
	vec2 lightPosition = iMouse.xy / uScreen.dimension.xy * 2.0 + ((iMouse.x + iMouse.y) == 0.0 ? vec2(1.0, 0.4) : vec2(0.0));

	vec3 color = originalColor*getAtmosphericScattering(position, lightPosition) * pi;
	color = jodieReinhardTonemap(color);
	color = pow(color, vec3(2.2)); //Back to linear

	outColor = vec4(color, 1.0);
}