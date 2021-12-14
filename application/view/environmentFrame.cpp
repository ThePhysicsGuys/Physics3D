#include "core.h"

#include "environmentFrame.h"

#include "imgui/imgui.h"
#include "shader/shaders.h"

// include this because we have a bit of code that uses it. 
#include "../layer/skyboxLayer.h"


namespace P3D::Application {

float EnvironmentFrame::hdr = 1.0f;
float EnvironmentFrame::gamma = 0.8f;
float EnvironmentFrame::exposure = 0.8f;
Color EnvironmentFrame::sunColor = Color(1);

void EnvironmentFrame::onInit(Engine::Registry64& registry) {
	
}

void EnvironmentFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Environment");
	
	if (ImGui::SliderFloat("HDR", &hdr, 0, 1)) {
		Shaders::basicShader->updateHDR(hdr);
		Shaders::instanceShader->updateHDR(hdr);
	}

	if (ImGui::SliderFloat("Gamma", &gamma, 0, 3)) {
		Shaders::basicShader->updateGamma(gamma);
		Shaders::instanceShader->updateGamma(gamma);
	}

	if (ImGui::SliderFloat("Exposure", &exposure, 0, 2)) {
		Shaders::basicShader->updateExposure(exposure);
		;
	}

	if (ImGui::ColorEdit3("Sun color", sunColor.data)) {
		Shaders::basicShader->updateSunColor(sunColor);
		Shaders::instanceShader->updateSunColor(sunColor);
	}

	ImGui::End();


	ImGui::Begin("Skybox");
	ImGui::Checkbox("New sky", &useNewSky);
	ImGui::Text("Time: %f", time);
	ImGui::Checkbox("Pauze", &pauze);
	ImGui::End();
}
}
