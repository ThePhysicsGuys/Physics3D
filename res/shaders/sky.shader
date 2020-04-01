[common]

#version 330 core

///////////////////////////////////////////////////////////////////////////////////////

[vertex]

layout(location = 0) in vec3 vposition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fposition;

void main() {
	vec4 position = projectionMatrix * vec4((viewMatrix * vec4(vposition, 0.0)).xyz, 1);
	fposition = normalize(vposition);
	gl_Position = position;
}

///////////////////////////////////////////////////////////////////////////////////////

[fragment]

out vec4 outColor;
in vec3 fposition;

uniform float time;
uniform samplerCube tex;

uniform vec3 viewPosition = vec3(100000, -40000, 0);
uniform vec3 sunPosition = vec3(0, -700000, 0);

float turbidity = 2.0;
float rayleigh = 1.0;
float luminance = 1.0;
float mieCoefficient = 0.005;
float mieDirectionalG = 0.8;

// Depolarization factor for air wavelength of primaries
float depolarizationFactor = 0.035;
vec3 primaries = vec3(6.8e-7, 5.5e-7, 4.5e-7);

// Mie, K coefficient for the primaries
vec3 mieKCoefficient = vec3(0.686, 0.678, 0.666);
float mieV = 4.0;

// Optical length at zenith for molecules
float rayleighZenithLength = 8.4e3;
float mieZenithLength = 1.25e3;

// Number of molecules per unit volume for air at 288.15K and 1013mb (sea level -45 celsius)
float numMolecules = 2.542e25;

// refractive index of air
float refractiveIndex = 1.0003;

// Visual size of sun
float sunAngularDiameterDegrees = 0.0093333;

// Sun intensity factors
float sunIntensityFactor = 1000.0;
float sunIntensityFalloffSteepness = 1.5;

// W factor in tonemap calculation
float tonemapWeighting = 9.5;

const float PI = 3.141592653589793238462643383279502884197169;
const vec3 UP = vec3(0.0, 1.0, 0.0);
const vec3 FRONT = vec3(0.0, 0.0, 1.0);

vec3 totalRayleigh(vec3 lambda) {
	return (8.0 * pow(PI, 3.0) * pow(pow(refractiveIndex, 2.0) - 1.0, 2.0) * (6.0 + 3.0 * depolarizationFactor)) / (3.0 * numMolecules * pow(lambda, vec3(4.0)) * (6.0 - 7.0 * depolarizationFactor));
}

vec3 totalMie(vec3 lambda, vec3 K, float T) {
	float c = 0.2 * T * 10e-18;
	return 0.434 * c * PI * pow((2.0 * PI) / lambda, vec3(mieV - 2.0)) * K;
}

float rayleighPhase(float cosTheta) {
	return (3.0 / (16.0 * PI)) * (1.0 + pow(cosTheta, 2.0));
}

float henyeyGreensteinPhase(float cosTheta, float g) {
	return (1.0 / (4.0 * PI)) * ((1.0 - pow(g, 2.0)) / pow(1.0 - 2.0 * g * cosTheta + pow(g, 2.0), 1.5));
}

float sunIntensity(float zenithAngleCos) {
	float cutoffAngle = PI / 1.95; // Earth shadow hack
	return sunIntensityFactor * max(0.0, 1.0 - exp(-((cutoffAngle - acos(zenithAngleCos)) / sunIntensityFalloffSteepness)));
}

// Whitescale tonemapping calculation, see http://filmicgames.com/archives/75
// Also see http://blenderartists.org/forum/showthread.php?321110-Shaders-and-Skybox-madness
const float A = 0.15; // Shoulder strength
const float B = 0.50; // Linear strength
const float C = 0.10; // Linear angle
const float D = 0.20; // Toe strength
const float E = 0.02; // Toe numerator
const float F = 0.30; // Toe denominator

vec3 Uncharted2Tonemap(vec3 W) {
	return ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
}

void main() {
	// Rayleigh coefficient
	float sunfade = 1.0 - clamp(1.0 - exp((sunPosition.y / 450000.0)), 0.0, 1.0);
	float rayleighCoefficient = rayleigh - (1.0 * (1.0 - sunfade));
	vec3 betaR = totalRayleigh(primaries) * rayleighCoefficient;

	// Mie coefficient
	vec3 betaM = totalMie(primaries, mieKCoefficient, turbidity) * mieCoefficient;

	// Optical length, cutoff angle at 90 to avoid singularity
	float zenithAngle = acos(max(0.0, dot(UP, normalize(fposition - viewPosition))));
	float denom = cos(zenithAngle) + 0.15 * pow(93.885 - ((zenithAngle * 180.0) / PI), -1.253);
	float sR = rayleighZenithLength / denom;
	float sM = mieZenithLength / denom;

	// Combined extinction factor
	vec3 Fex = exp(-(betaR * sR + betaM * sM));

	// In-scattering
	vec3 sunDirection = normalize(sunPosition);
	float cosTheta = dot(normalize(fposition - viewPosition), sunDirection);
	vec3 betaRTheta = betaR * rayleighPhase(cosTheta * 0.5 + 0.5);
	vec3 betaMTheta = betaM * henyeyGreensteinPhase(cosTheta, mieDirectionalG);
	float sunE = sunIntensity(dot(sunDirection, UP));
	vec3 Lin = pow(sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * (1.0 - Fex), vec3(1.5));
	Lin *= mix(vec3(1.0), pow(sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * Fex, vec3(0.5)), clamp(pow(1.0 - dot(UP, sunDirection), 5.0), 0.0, 1.0));

	// Composition + solar disc
	float sunAngularDiameterCos = cos(sunAngularDiameterDegrees);
	float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta);
	vec3 L0 = vec3(0.1) * Fex;
	L0 += sunE * 19000.0 * Fex * sundisk;
	vec3 texColor = Lin + L0;
	texColor *= 0.04;
	texColor += vec3(0.0, 0.001, 0.0025) * 0.3;

	// Tonemapping
	vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(tonemapWeighting));
	vec3 curr = Uncharted2Tonemap((log2(2.0 / pow(luminance, 4.0))) * texColor);
	vec3 color = curr * whiteScale;
	vec3 retColor = pow(color, vec3(1.0 / (1.2 + (1.2 * sunfade))));

	//outColor = vec4(retColor, 1.0);
	outColor = mix(texture(tex, fposition), vec4(fposition,1), 0.5);

	float d1 = dot(UP, fposition);
	float d2 = dot(FRONT, fposition);
	float d3 = dot(vec3(1,0,0), fposition);
	if (mod(d1, 0.2) < 0.001 || mod(d2, 0.2) < 0.001 || mod(d3, 0.2) < 0.001)
		outColor = vec4(1);
}