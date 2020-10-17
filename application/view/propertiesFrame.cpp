#include "core.h"

#include "propertiesFrame.h"

#include "screen.h"
#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "imgui/imgui.h"
#include "../physics/misc/toString.h"
#include "../physics/physical.h"

namespace P3D::Application {

Vec3f PropertiesFrame::position = Vec3f(0, 0, 0);

void PropertiesFrame::onInit(Engine::Registry64& registry) {
	
}

void PropertiesFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Properties");
	
	ExtendedPart* sp = screen.selectedPart;

	ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("General")) {
		std::string name = registry.getOr<Comp::Tag>((sp) ? sp->entity : 0, (sp) ? std::to_string(sp->entity) : "-").name;
		ImGui::Text("Name: %s", name.c_str());
		//ImGui::Text("Mesh ID: %s", (sp) ? str(sp->visualData.id).c_str() : "-");

		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Physical")) {
		if (sp) {
			// Position
			position = castPositionToVec3(sp->getPosition());
			if (ImGui::InputFloat3("Position: ", position.data, 3)) {
				GlobalCFrame frame = sp->getCFrame();
				frame.position = castVec3fToPosition(position);
				sp->setCFrame(frame);
			}
		} else {
			// Position
			position = Vec3f(0, 0, 0);
			ImGui::InputFloat3("Position", position.data, 3, ImGuiInputTextFlags_ReadOnly);
		}

		ImGui::Text("Velocity: %s", (sp) ? str(sp->getMotion().getVelocity()).c_str() : "-");
		ImGui::Text("Acceleration: %s", (sp) ? str(sp->getMotion().getAcceleration()).c_str() : "-");
		ImGui::Text("Angular velocity: %s", (sp) ? str(sp->getMotion().getAngularVelocity()).c_str() : "-");
		ImGui::Text("Angular acceleration: %s", (sp) ? str(sp->getMotion().getAngularAcceleration()).c_str() : "-");
		ImGui::Text("Mass: %s", (sp) ? str(sp->getMass()).c_str() : "-");
		ImGui::Text("Friction: %s", (sp) ? str(sp->properties.friction).c_str() : "-");
		ImGui::Text("Density: %s", (sp) ? str(sp->properties.density).c_str() : "-");
		ImGui::Text("Bouncyness: %s", (sp) ? str(sp->properties.bouncyness).c_str() : "-");
		ImGui::Text("Conveyor: %s", (sp) ? str(sp->properties.conveyorEffect).c_str() : "-");
		ImGui::Text("Inertia: %s", (sp) ? str(sp->getInertia()).c_str() : "-");


		if (sp != nullptr && sp->parent != nullptr) {
			const MotorizedPhysical* phys = sp->parent->mainPhysical;
			ImGui::Text("Physical Info");

			ImGui::Text("Total impulse: %s", str(phys->getTotalImpulse()).c_str());
			ImGui::Text("Total angular momentum: %s", str(phys->getTotalAngularMomentum()).c_str());
			ImGui::Text("motion of COM: %s", str(phys->getMotionOfCenterOfMass()).c_str());
		}

		static volatile ExtendedPart* selectedPart = nullptr;
		if (sp != nullptr) {
			selectedPart = sp;
		}

		if (ImGui::Button("Debug This Part")) {
			Log::debug("Debugging part %d", reinterpret_cast<uint64_t>(selectedPart));
#ifdef _MSC_VER
			__debugbreak();
#else
			Log::warn("Debug breaking is not supported on non-linux platforms");
#endif
		}

		ImGui::TreePop();
		}

	ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Material")) {
		if (sp) {
			Ref<Comp::Material> material = registry.getOrAdd<Comp::Material>(sp->entity);
			ImGui::ColorEdit4("Albedo", material->albedo.data);
			ImGui::SliderFloat("Metalness", &material->metalness, 0, 1);
			ImGui::SliderFloat("Roughness", &material->roughness, 0, 1);
			ImGui::SliderFloat("Ambient occlusion", &material->ao, 0, 1);
			//if (ImGui::Button(sp ? (sp->renderMode == Renderer::FILL ? "Render mode: fill" : "Render mode: wireframe") : "l"))
			//	if (sp) sp->renderMode = sp->renderMode == Renderer::FILL ? Renderer::WIREFRAME : Renderer::FILL;
		} else {
			ImGui::Text("No part selected");
		}

		ImGui::TreePop();
	}

	ImGui::End();
}
	
}
