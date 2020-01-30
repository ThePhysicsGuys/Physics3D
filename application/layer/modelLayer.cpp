#include "core.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "modelLayer.h"

#include "view/screen.h"
#include "shader/shaders.h"
#include "extendedPart.h"
#include "worlds.h"

#include "ecs/light.h"
#include "ecs/material.h"
#include "ecs/model.h"

#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/meshLibrary.h"
#include "../graphics/debug/visualDebug.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/sharedLockGuard.h"
#include "../physics/misc/filters/visibilityFilter.h"

namespace Application {

// Light uniforms
Vec3f sunDirection;
Vec3f sunColor;

enum class RelationToSelectedPart {
	NONE,
	SELF,
	DIRECT_ATTACH,
	MAINPART,
	PHYSICAL_ATTACH,
	MAINPHYSICAL_ATTACH
};

static RelationToSelectedPart getRelationToSelectedPart(const Part* selectedPart, const Part* testPart) {
	if (selectedPart == nullptr) 
		return RelationToSelectedPart::NONE;

	if (testPart == selectedPart)
		return RelationToSelectedPart::SELF;

	if (selectedPart->parent != nullptr && testPart->parent != nullptr) {
		if (testPart->parent == selectedPart->parent) {
			if (testPart->isMainPart()) 
				return RelationToSelectedPart::MAINPART;
			else 
				return RelationToSelectedPart::DIRECT_ATTACH;
		} else if (testPart->parent->mainPhysical == selectedPart->parent->mainPhysical) {
			if (testPart->parent->isMainPhysical()) 
				return RelationToSelectedPart::MAINPHYSICAL_ATTACH;
			else 
				return RelationToSelectedPart::PHYSICAL_ATTACH;
		}
	}

	return RelationToSelectedPart::NONE;
}

static Color getAmbientForPartForSelected(Screen* screen, Part* part) {
	switch (getRelationToSelectedPart(screen->selectedPart, part)) {
		case RelationToSelectedPart::NONE: 
			return Color(0.0f, 0, 0, 0);
		case RelationToSelectedPart::SELF:
			return Color(0.5f, 0, 0, 0);
		case RelationToSelectedPart::DIRECT_ATTACH: 
			return Color(0, 0.25f, 0, 0);
		case RelationToSelectedPart::MAINPART: 
			return Color(0, 1.0f, 0, 0);
		case RelationToSelectedPart::PHYSICAL_ATTACH: 
			return Color(0, 0, 0.25f, 0);
		case RelationToSelectedPart::MAINPHYSICAL_ATTACH: 
			return Color(0, 0, 1.0f, 0);
	}

	return Color(0, 0, 0, 0);
}

static Color getAmbientForPart(Screen* screen, Part* part) {
	Color computedAmbient = getAmbientForPartForSelected(screen, part);
	if (part == screen->intersectedPart) 
		computedAmbient += Vec4f(-0.1f, -0.1f, -0.1f, 0);
	
	return computedAmbient;
}

void ModelLayer::onInit() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Attenuation attenuation = { 0, 0, 0.5 };
	screen->entities.push_back(new Entity({ new Light(Vec3f(10, 5, -10), Color3(1, 0.84f, 0.69f), 6, attenuation) }));
	screen->entities.push_back(new Entity({ new Light(Vec3f(10, 5, 10), Color3(1, 0.84f, 0.69f), 6, attenuation) }));
	screen->entities.push_back(new Entity({ new Light(Vec3f(-10, 5, -10), Color3(1, 0.84f, 0.69f), 6, attenuation) }));
	screen->entities.push_back(new Entity({ new Light(Vec3f(-10, 5, 10), Color3(1, 0.84f, 0.69f), 6, attenuation) }));
	screen->entities.push_back(new Entity({ new Light(Vec3f(0, 5, 0), Color3(1, 0.90f, 0.75f), 10, attenuation) }));
}

void ModelLayer::onUpdate() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	std::vector<Light*> lights;
	for (Entity* entity : screen->entities) {
		Light* light = entity->getComponent<Light>();

		if (light == nullptr)
			continue;

		lights.push_back(light);
	}

	ApplicationShaders::basicShader.updateLight(lights);
}

void ModelLayer::onEvent(Event& event) {

}

void ModelLayer::onRender() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	graphicsMeasure.mark(GraphicsProcess::UPDATE);
	ApplicationShaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	ApplicationShaders::maskShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);

	std::vector<ExtendedPart*> visibleParts;
	graphicsMeasure.mark(GraphicsProcess::PHYSICALS);
	screen->world->syncReadOnlyOperation([this, &visibleParts, screen] () {
		VisibilityFilter filter = VisibilityFilter::forWindow(screen->camera.cframe.position, screen->camera.getForwardDirection(), screen->camera.getUpDirection(), screen->camera.fov, screen->camera.aspect, screen->camera.zfar);
		for (ExtendedPart& part : screen->world->iterPartsFiltered(filter, ALL_PARTS))
			visibleParts.push_back(&part);
	});

	std::map<double, ExtendedPart*> transparentMeshes;
	for (ExtendedPart* part : visibleParts) {
		Material material = part->material;
		material.ambient += getAmbientForPart(screen, part);

		if (material.ambient.w < 1) {
			transparentMeshes[lengthSquared(Vec3(screen->camera.cframe.position - part->getPosition()))] = part;
			continue;
		}

		if (part->visualData.drawMeshId == -1)
			continue;

		ApplicationShaders::basicShader.updateMaterial(material);
		ApplicationShaders::basicShader.updatePart(*part);
		Screen::meshes[part->visualData.drawMeshId]->render(part->renderMode);
	}

	for (auto iterator = transparentMeshes.rbegin(); iterator != transparentMeshes.rend(); ++iterator) {
		ExtendedPart* part = (*iterator).second;

		Material material = part->material;
		material.ambient += getAmbientForPart(screen, part);

		if (part->visualData.drawMeshId == -1)
			continue;

		ApplicationShaders::basicShader.updateMaterial(material);
		ApplicationShaders::basicShader.updatePart(*part);
		Screen::meshes[part->visualData.drawMeshId]->render(part->renderMode);
	}
}

void ModelLayer::onClose() {

}

};