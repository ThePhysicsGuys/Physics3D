#include "core.h"

#include "environmentFrame.h"

#include "imgui/imgui.h"
#include "shader/shaders.h"

namespace P3D::Application {

float EnvironmentFrame::hdr = 1.0f;
float EnvironmentFrame::gamma = 1.0f;
float EnvironmentFrame::exposure = 1.0f;
Color3 EnvironmentFrame::sunColor = Color3::full(1);

void EnvironmentFrame::onInit(Engine::Registry64& registry) {
	
}

void EnvironmentFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Environment");
	
	if (ImGui::SliderFloat("HDR", &hdr, 0, 1)) {
		Shaders::basicShader.updateHDR(hdr);
		Shaders::instanceShader.updateHDR(hdr);
	}

	if (ImGui::SliderFloat("Gamma", &gamma, 0, 3)) {
		Shaders::basicShader.updateGamma(gamma);
		Shaders::instanceShader.updateGamma(gamma);
	}

	if (ImGui::SliderFloat("Exposure", &exposure, 0, 2)) {
		Shaders::basicShader.updateExposure(exposure);
		Shaders::instanceShader.updateExposure(exposure);
	}

	if (ImGui::ColorEdit3("Sun color", sunColor.data)) {
		Shaders::basicShader.updateSunColor(sunColor);
		Shaders::instanceShader.updateSunColor(sunColor);
	}

	ImGui::End();
}
	
}
