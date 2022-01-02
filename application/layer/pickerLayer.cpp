#include "core.h"

#include "pickerLayer.h"

#include "../application.h"
#include "../graphics/debug/visualDebug.h"
#include "view/screen.h"
#include "input/standardInputHandler.h"

#include "../engine/tool/toolManager.h"
#include "../graphics/renderer.h"
#include "../picker/tools/selectionTool.h"
#include "../picker/tools/regionSelectionTool.h"
#include "../picker/tools/translationTool.h"
#include "../picker/tools/rotationTool.h"
#include "../picker/tools/scaleTool.h"
#include "../picker/ray.h"
#include "picker/tools/attachmentTool.h"
#include "picker/tools/elasticLinkTool.h"
#include "picker/tools/fixedConstraintTool.h"
#include "picker/tools/magneticLinkTool.h"
#include "picker/tools/motorConstraintTool.h"
#include "picker/tools/pistonConstraintTool.h"
#include "picker/tools/springLinkTool.h"
#include "picker/tools/toolSpacing.h"

namespace P3D::Application {

std::vector<Engine::ToolManager> PickerLayer::toolManagers;
	
void PickerLayer::onInit(Engine::Registry64& registry) {
	Engine::ToolManager toolManager;
	toolManager.registerTool<SelectionTool>();
	toolManager.registerTool<RegionSelectionTool>();
	toolManager.registerTool<TranslationTool>();
	toolManager.registerTool<RotationTool>();
	toolManager.registerTool<ScaleTool>();
	
	toolManager.registerTool<ToolSpacing>();

	toolManager.registerTool<AttachmentTool>();
	toolManager.registerTool<FixedConstraintTool>();
	toolManager.registerTool<MotorConstraintTool>();
	toolManager.registerTool<PistonConstraintTool>();

	toolManager.registerTool<ToolSpacing>();
	
	toolManager.registerTool<MagneticLinkTool>();
	toolManager.registerTool<SpringLinkTool>();
	toolManager.registerTool<ElasticLinkTool>();
	
	toolManager.selectTool<TranslationTool>();

	toolManagers.push_back(std::move(toolManager));
}

void PickerLayer::onUpdate(Engine::Registry64& registry) {
	Screen* screen = static_cast<Screen*>(this->ptr);
	
	Graphics::graphicsMeasure.mark(Graphics::GraphicsProcess::PICKER);

	// Update selection context
	SelectionTool::mouse = handler->mousePosition;
	SelectionTool::ray = getMouseRay(*screen, handler->mousePosition);

	if (!isPaused())
		SelectionTool::selection.recalculateSelection();

	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onUpdate();
}

void PickerLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onEvent(event);
}

void PickerLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	clearDepth();
	enableDepthTest();

	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onRender();

	disableDepthTest();
	endScene();
}

void PickerLayer::onClose(Engine::Registry64& registry) {
	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onClose();
}

};

/*
void moveGrabbedEntityLateral(Screen& screen) {
	if (screen.world->selectedPart == nullptr)
		return;
if (!screen.selectedEntity)
return;

Vec3 cameraDirection = screen.camera.cframe.rotation * Vec3(0, 0, 1);

double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (SelectionTool::ray.direction * cameraDirection);
Position planeIntersection = screen.camera.cframe.position + SelectionTool::ray.direction * distance;

if (isPaused()) {
	Vec3 translation = planeIntersection - screen.selectedPoint;
	screen.selectedPoint += translation;
	Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(screen.selectedEntity);
	if (transform.valid())
		transform->translate(translation);
} else {
	screen.world->selectedPart = screen.selectedPart;
	screen.world->magnetPoint = planeIntersection;
}
}

void moveGrabbedEntityTransversal(Screen& screen, double dz) {
	if (screen.world->selectedPart == nullptr)
		return;
	if (!screen.selectedEntity)
		return;

	Vec3 cameraDirection = screen.camera.cframe.rotation * Vec3(0, 0, 1);
	Vec3 cameraYDirection = normalize(Vec3(cameraDirection.x, 0, cameraDirection.z));

	double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (SelectionTool::ray.direction * cameraDirection);
	Position planeIntersection = screen.camera.cframe.position + SelectionTool::ray.direction * distance;

	Vec3 translation = -cameraYDirection * dz;
	if (isPaused()) {
		screen.selectedPoint += translation;
		Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(screen.selectedEntity);
		if (transform.valid())
			transform->translate(translation);
	} else {
		screen.world->selectedPart = screen.selectedPart;
		screen.world->magnetPoint += translation;
	}
}
*/