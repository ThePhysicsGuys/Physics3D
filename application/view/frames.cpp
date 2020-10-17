#include "core.h"
#include "frames.h"

#include "resourceFrame.h"
#include "layerFrame.h"
#include "ecsFrame.h"
#include "debugFrame.h"
#include "environmentFrame.h"
#include "propertiesFrame.h"

namespace P3D::Application {


void Frames::onInit(Engine::Registry64& registry) {
	ECSFrame::onInit(registry);
	PropertiesFrame::onInit(registry);
	LayerFrame::onInit(registry);
	ResourceFrame::onInit(registry);
	EnvironmentFrame::onInit(registry);
	DebugFrame::onInit(registry);
}

void Frames::onRender(Engine::Registry64& registry) {
	//ImGui::ShowDemoWindow();

	ECSFrame::onRender(registry);
	PropertiesFrame::onRender(registry);
	LayerFrame::onRender(registry);
	ResourceFrame::onRender(registry);
	EnvironmentFrame::onRender(registry);
	DebugFrame::onRender(registry);
}

}
