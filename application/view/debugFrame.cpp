#include "core.h"

#include "debugFrame.h"


#include "imgui/imgui.h"
#include "../graphics/debug/visualDebug.h"

namespace P3D::Application {

void DebugFrame::onInit(Engine::Registry64& registry) {
	
}

void DebugFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Debug");
	
	using namespace P3D::Debug;
	using namespace P3D::Graphics::VisualDebug;
	ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Vectors")) {
		ImGui::Checkbox("Info", &vectorDebugEnabled[INFO_VEC]);
		ImGui::Checkbox("Position", &vectorDebugEnabled[POSITION]);
		ImGui::Checkbox("Velocity", &vectorDebugEnabled[VELOCITY]);
		ImGui::Checkbox("Acceleration", &vectorDebugEnabled[ACCELERATION]);
		ImGui::Checkbox("Moment", &vectorDebugEnabled[MOMENT]);
		ImGui::Checkbox("Force", &vectorDebugEnabled[FORCE]);
		ImGui::Checkbox("Angular impulse", &vectorDebugEnabled[ANGULAR_IMPULSE]);
		ImGui::Checkbox("Impulse", &vectorDebugEnabled[IMPULSE]);

		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Points")) {
		ImGui::Checkbox("Info", &pointDebugEnabled[INFO_POINT]);
		ImGui::Checkbox("Center of mass", &pointDebugEnabled[CENTER_OF_MASS]);
		ImGui::Checkbox("Intersections", &pointDebugEnabled[INTERSECTION]);

		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Render")) {
		ImGui::Checkbox("Render pies", &renderPiesEnabled);
		if (ImGui::Button("Switch collision sphere render mode")) colissionSpheresMode = static_cast<SphereColissionRenderMode>((static_cast<int>(colissionSpheresMode) + 1) % 3);

		ImGui::TreePop();
	}

	ImGui::End();
}

	
}
