#pragma once

#include "../physics/misc/toString.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/shader/shaderProgram.h"
#include "../graphics/renderUtils.h"
#include "../graphics/texture.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"
#include "../util/resource/resource.h"
#include "shader/shaders.h"
#include "extendedPart.h"
#include "application.h"
#include "worlds.h"
#include "screen.h"

#include "imgui/imgui.h"

// Environment frame

struct EnvironmentFrame  {

	static bool hdr;
	static float gamma;
	static float exposure;
	static Color3 sunColor;

	static void render() {
		ImGui::Begin("Environment");

		// Hdr checkbox
		if (ImGui::Checkbox("HDR", &hdr)) {
			ApplicationShaders::basicShader.updateHDR(hdr);

			/*if (hdr) {
				exposureSlider->enable();
				exposureValueLabel->enable();
				exposureLabel->enable();
			} else {
				exposureSlider->enable();
				exposureValueLabel->enable();
				exposureLabel->enable();
			}*/
		}
		
		// Gamma slider
		if (ImGui::SliderFloat("Gamma", &gamma, 0, 3))
			ApplicationShaders::basicShader.updateGamma(gamma);

		// Exposure slider
		if (ImGui::SliderFloat("Exposure", &exposure, 0, 2))
			ApplicationShaders::basicShader.updateExposure(exposure);

		if (ImGui::ColorEdit3("Sun color", sunColor.data))
			ApplicationShaders::basicShader.updateSunColor(sunColor);

		ImGui::End();
	}
};

struct FrameBlueprint {
	virtual void init() = 0;
	virtual void update() = 0;
};

// Debug frame

struct DebugFrame {
	
	// Todo remove
	static bool renderSpheres;

	static void render() {
		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Vectors")) {
			ImGui::Checkbox("Info", &Debug::vectorDebugEnabled[Debug::INFO_VEC]);
			ImGui::Checkbox("Position", &Debug::vectorDebugEnabled[Debug::POSITION]);
			ImGui::Checkbox("Velocity", &Debug::vectorDebugEnabled[Debug::VELOCITY]);
			ImGui::Checkbox("Acceleration", &Debug::vectorDebugEnabled[Debug::ACCELERATION]);
			ImGui::Checkbox("Moment", &Debug::vectorDebugEnabled[Debug::MOMENT]);
			ImGui::Checkbox("Force", &Debug::vectorDebugEnabled[Debug::FORCE]);
			ImGui::Checkbox("Angular impulse", &Debug::vectorDebugEnabled[Debug::ANGULAR_IMPULSE]);
			ImGui::Checkbox("Impulse", &Debug::vectorDebugEnabled[Debug::IMPULSE]);
		}

		if (ImGui::CollapsingHeader("Points")) {
			ImGui::Checkbox("Info", &Debug::pointDebugEnabled[Debug::INFO_POINT]);
			ImGui::Checkbox("Center of mass", &Debug::pointDebugEnabled[Debug::CENTER_OF_MASS]);
			ImGui::Checkbox("Intersections", &Debug::pointDebugEnabled[Debug::INTERSECTION]);
		}

		if (ImGui::CollapsingHeader("Render")) {
			ImGui::Checkbox("Render pies", &Debug::renderPiesEnabled);
			if (ImGui::Button("Switch collision sphere render mode")) Debug::colissionSpheresMode = static_cast<Debug::SphereColissionRenderMode>((static_cast<int>(Debug::colissionSpheresMode) + 1) % 3);
		}

		ImGui::End();
	}
};


// Properties frame

struct PropertiesFrame {

	static float position[3];

	static void render() {
		ImGui::Begin("Properties");

		ImGuiInputTextFlags flags = 0;
		ExtendedPart* sp = screen.selectedPart;

		if (ImGui::CollapsingHeader("General")) {
			ImGui::Text("Name: %s", (sp)? sp->name.c_str() : "-");
			ImGui::Text("Mesh ID: %s", (sp)? str(sp->visualData.drawMeshId) : "-");
		}

		if (ImGui::CollapsingHeader("Physical")) {
			if (sp) {
				// Position
				position[0] = sp->getPosition()[0];
				position[1] = sp->getPosition()[1];
				position[2] = sp->getPosition()[2];
				if (ImGui::InputFloat3("Position: ", position, 3)) {
					GlobalCFrame frame = sp->getCFrame();
					frame.position = Position(position[0], position[1], position[2]);
					sp->setCFrame(frame);
				}
			} else {
				// Position
				position[0] = 0;
				position[1] = 0;
				position[2] = 0;
				ImGui::InputFloat3("Position", position, 3, ImGuiInputTextFlags_ReadOnly);
			}
	
			ImGui::Text("Velocity: %s", (sp) ? str(sp->getMotion().velocity).c_str() : "-");
			ImGui::Text("Acceleration: %s", (sp) ? str(sp->getMotion().acceleration).c_str() : "-");
			ImGui::Text("Angular velocity: %s", (sp) ? str(sp->getMotion().angularVelocity).c_str() : "-");
			ImGui::Text("Angular acceleration: %s", (sp) ? str(sp->getMotion().angularAcceleration).c_str() : "-");
			ImGui::Text("Kinetic energy: %s", (sp) ? str(sp->parent->getKineticEnergy()).c_str() : "-");
			ImGui::Text("Potential energy: %s", (sp) ? str(screen.world->getPotentialEnergyOfPhysical(*sp->parent->mainPhysical)).c_str() : "-");
			ImGui::Text("Total energy: %s", (sp) ? str(screen.world->getPotentialEnergyOfPhysical(*sp->parent->mainPhysical) + sp->parent->getKineticEnergy()).c_str() : "-");
			ImGui::Text("Mass: %s", (sp) ? str(sp->getMass()).c_str() : "-");
			ImGui::Text("Friction: %s", (sp) ? str(sp->properties.friction).c_str() : "-");
			ImGui::Text("Density: %s", (sp) ? str(sp->properties.density).c_str() : "-");
			ImGui::Text("Bouncyness: %s", (sp) ? str(sp->properties.bouncyness).c_str() : "-");
			ImGui::Text("Conveyor: %s", (sp) ? str(sp->properties.conveyorEffect).c_str() : "-");
			ImGui::Text("Inertia: %s", (sp) ? str(sp->getInertia()).c_str() : "-");
		}

		if (ImGui::CollapsingHeader("Material")) {
			if (sp) {
				ImGui::ColorEdit4("Ambient", sp->material.ambient.data);
				ImGui::ColorEdit3("Diffuse", sp->material.diffuse.data);
				ImGui::ColorEdit3("Specular", sp->material.specular.data);
				ImGui::SliderFloat("Reflectance", &sp->material.reflectance, 0, 1);
				if (ImGui::Button(sp ? (sp->renderMode == Renderer::FILL ? "Render mode: fill" : "Render mode: wireframe") : "l"))
					if (sp) sp->renderMode = sp->renderMode == Renderer::FILL ? Renderer::WIREFRAME : Renderer::FILL;
			}
		}

		ImGui::End();
	}
};