#include "core.h"

#include "editTools.h"

#include "picker.h"
#include "extendedPart.h"
#include "worlds.h"
#include "view/screen.h"
#include "shader/shaders.h"

#include "../engine/io/import.h"
#include "../engine/io/export.h"

#include "../graphics/renderer.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/visualShape.h"
#include "ecs/material.h"
#include "../graphics/gui/gui.h"

#include "../util/resource/resourceManager.h"
#include "../engine/resource/meshResource.h"

namespace Application {

Rotation transformations[] {
	Rotation::Predefined::IDENTITY,
	Rotation::rotZ(-3.14159265359 / 2.0),
	Rotation::rotX(3.14159265359 / 2.0),
	Rotation::Predefined::IDENTITY
};

// Render variables
Graphics::LinePrimitive* line = nullptr;

MeshResource* rotateMesh = nullptr;
MeshResource* translateCenterMesh = nullptr;
MeshResource* translateMesh = nullptr;
MeshResource* scaleCenterMesh = nullptr;
MeshResource* scaleMesh = nullptr;

void EditTools::onInit() {

	// TODO fix duplicate loading

	// Edit line init
	line = new Graphics::LinePrimitive();
	line->resize(Vec3f(0, -100000, 0), Vec3f(0, 100000, 0));

	// Rotate shape init
	rotateMesh = ResourceManager::add<MeshResource>("rotate", "../res/models/gui/rotate.bobj");

	// Scale shape init
	scaleMesh = ResourceManager::add<MeshResource>("scale", "../res/models/gui/scale_shaft.bobj");
	scaleCenterMesh = ResourceManager::add<MeshResource>("scale center", "../res/models/gui/scale_center.bobj");

	// Translate shape init
	translateMesh = ResourceManager::add<MeshResource>("translate", "../res/models/gui/translate_shaft.bobj");
	translateCenterMesh = ResourceManager::add<MeshResource>("translate center", "../res/models/gui/translate_center.bobj");

	// Intersected tool init
	intersectedEditDirection = EditDirection::NONE;
	intersectedPoint = Vec3f();

	// Selected tool init
	selectedEditDirection = EditDirection::NONE;
	selectedPoint = Vec3f();
}


void EditTools::onRender(Screen& screen) {
	using namespace Graphics;

	IndexedMesh* shaft = nullptr;
	IndexedMesh* center = nullptr;

	// Select correct render meshes
	switch (editMode) {
		case EditMode::TRANSLATE:
			shaft = translateMesh->getMesh();
			center = translateCenterMesh->getMesh();
			break;
		case EditMode::ROTATE:
			shaft = rotateMesh->getMesh();
			center = nullptr;
			break;
		case EditMode::SCALE:
			shaft = scaleMesh->getMesh();
			center = scaleCenterMesh->getMesh();
			break;
	}

	GlobalCFrame selFrame = screen.selectedPart->getCFrame();
	Mat4 modelMatrix(selFrame.getRotation().asRotationMatrix(), selFrame.getPosition() - Position(0,0,0), Vec3(0,0,0), 1);

	// Center, only rendered if editMode != EditMode::ROTATE
	if (center) {
		ApplicationShaders::basicShader.updateModel(modelMatrix);
		ApplicationShaders::basicShader.updateMaterial(Material(COLOR::WHITE));
		center->render();
	}

	if (selectedEditDirection != EditDirection::NONE) {
		switch (selectedEditDirection) {
			case EditDirection::Y:
				ApplicationShaders::maskShader.updateModel(modelMatrix);
				ApplicationShaders::maskShader.updateColor(COLOR::G);
				break;
			case EditDirection::X:
				ApplicationShaders::maskShader.updateModel(modelMatrix * Mat4(Matrix<double, 3, 3>(transformations[1]), 1.0f));
				ApplicationShaders::maskShader.updateColor(COLOR::R);
				break;
			case EditDirection::Z:
				ApplicationShaders::maskShader.updateModel(modelMatrix * Mat4(Matrix<double, 3, 3>(transformations[2]), 1.0f));
				ApplicationShaders::maskShader.updateColor(COLOR::B);
				break;
		}
		line->render();
	}

	// Y
	ApplicationShaders::basicShader.updateModel(modelMatrix);
	ApplicationShaders::basicShader.updateMaterial(Material(COLOR::G));
	shaft->render();

	// X
	ApplicationShaders::basicShader.updateMaterial(Material(COLOR::R));
	ApplicationShaders::basicShader.updateModel(modelMatrix * Mat4(Matrix<double, 3, 3>(transformations[1]), 1.0f));
	shaft->render();

	// Z
	ApplicationShaders::basicShader.updateMaterial(Material(COLOR::B));
	ApplicationShaders::basicShader.updateModel(modelMatrix * Mat4(Matrix<double, 3, 3>(transformations[2]), 1.0f));
	shaft->render();
}

void EditTools::onClose() {
	rotateMesh->close();
	translateCenterMesh->close();
	translateMesh->close();
	scaleCenterMesh->close();
	scaleMesh->close();
}

float EditTools::intersect(Screen& screen, const Ray& ray) {
	Graphics::VisualShape tool[2];

	// Select correct tools
	switch (editMode) {
		case EditMode::TRANSLATE:
			tool[0] = translateMesh->getShape();
			tool[1] = translateCenterMesh->getShape();
			break;
		case EditMode::ROTATE:
			tool[0] = rotateMesh->getShape();
			tool[1] = Graphics::VisualShape();
			break;
		case EditMode::SCALE:
			tool[0] = scaleMesh->getShape();
			tool[1] = scaleCenterMesh->getShape();
			break;
		default:
			throw "Error: Impossible!";
	}

	float closestToolDistance = INFINITY;
	EditDirection closestToolDirection = EditDirection::NONE;

	// Check intersections of selected tool
	for (int i = 0; i < 4; i++) {
		GlobalCFrame frame = screen.selectedPart->getCFrame();
		frame.rotation = frame.getRotation() * transformations[i];

		Graphics::VisualShape shape = tool[i / 3];
		Position rayStart = ray.start;

		float distance = shape.getIntersectionDistance(frame.globalToLocal(ray.start), frame.relativeToLocal(ray.direction));

		if (distance < closestToolDistance && distance > 0) {
			closestToolDistance = distance;
			closestToolDirection = static_cast<EditDirection>(i);
		}
	}

	// Update intersected tool
	if (closestToolDistance != INFINITY) {
		intersectedEditDirection = closestToolDirection;
		intersectedPoint = (ray.start + ray.direction * closestToolDistance) - screen.selectedPart->getPosition();
		return closestToolDistance;
	} else {
		intersectedEditDirection = EditDirection::NONE;
		intersectedPoint = Vec3f();
		return -1;
	}
}


void EditTools::onMousePress(Screen& screen) {
	selectedEditDirection = intersectedEditDirection;
	selectedPoint = intersectedPoint;
}

void EditTools::onMouseRelease(Screen& screen) {
	selectedEditDirection = EditDirection::NONE;
}

void EditTools::onMouseDrag(Screen& screen) {
	if (screen.selectedPart == nullptr)
		return;

	screen.world->asyncModification([&screen, this] () {
		switch (editMode) {
			case EditMode::TRANSLATE:
				dragTranslateTool(screen);
				break;
			case EditMode::ROTATE:
				dragRotateTool(screen);
				break;
			case EditMode::SCALE:
				dragScaleTool(screen);
				break;
		}
		});
}


// Drag behaviour of rotate tool
void EditTools::dragRotateTool(Screen& screen) {
	// Plane of edit tool, which can be expressed as all points p where (p - p0) * n = 0. Where n is the edit direction and p0 the center of the selected part
	Vec3 n;
	Position p0 = screen.selectedPart->getPosition();
	switch (selectedEditDirection) {
		case EditDirection::X:
			n = Vec3(1, 0, 0);
			break;
		case EditDirection::Y:
			n = Vec3(0, 1, 0);
			break;
		case EditDirection::Z:
			n = Vec3(0, 0, 1);
			break;
	}

	// Apply model matrix
	n = screen.selectedPart->getCFrame().localToRelative(n);

	// Mouse ray, can be expressed as alls points p where p = l0 + d * l, where l0 is the camera position and l is the mouse ray.
	Position l0 = screen.camera.cframe.position;
	Vec3 l = screen.ray;

	// Calculate intersection of mouse ray and edit plane
	double ln = l * n;
	if (ln == 0)
		return; // No rotation if plane is perpendicular to mouse ray
	Position intersection = l0 + (Vec3(p0 - l0) * n) / (l * n) * l;

	// Vector from part center to intersection
	Vec3 intersectionVector = intersection - p0;

	// Length check
	double length1sq = lengthSquared(intersectionVector);
	double length2sq = lengthSquared(selectedPoint);
	if (length1sq == 0 || length2sq == 0)
		return; // Prevent errors when vector is the zero vector

	// Triple product to find angle sign
	double triple = selectedPoint * (intersectionVector % n);
	double sign = (triple > 0) ? 1 : -1;

	// Get angle between last intersectionVector and new one
	double cosa = (selectedPoint * intersectionVector) / sqrt(length1sq * length2sq);
	
	if(!(abs(cosa) < 1)) return; // No rotation when vectors coincide
	
	double a = sign * acos(cosa);

	// Update last intersectionVector
	selectedPoint = intersectionVector;

	// Apply rotation
	screen.selectedPart->setCFrame(screen.selectedPart->getCFrame().rotated(Rotation::fromRotationVec(a*n)));
}

// Drag behaviour of scale tool
void EditTools::dragScaleTool(Screen& screen) {
	Vec3 ray = screen.ray;
	Vec3 deltaPos = screen.camera.cframe.position - screen.selectedPart->getPosition();
	double distance = length(deltaPos - (deltaPos * ray) * ray) / length(selectedPoint);

	Log::debug("dist : %f", distance);

	switch (selectedEditDirection) {
		case EditDirection::X:
			screen.selectedPart->setWidth(distance * 2);
			break;
		case EditDirection::Y:
			screen.selectedPart->setHeight(distance * 2);
			break;
		case EditDirection::Z:
			screen.selectedPart->setDepth(distance * 2);
			break;
		case EditDirection::CENTER:
			double amount = distance / screen.selectedPart->maxRadius / sqrt(3.0);
			screen.selectedPart->scale(amount, amount, amount);
			break;
	}
}


// Drag behaviour of translate tool
void EditTools::dragTranslateTool(Screen& screen) {
	if (selectedEditDirection == EditDirection::CENTER) {
		screen.selectedPoint = screen.selectedPart->getPosition() + selectedPoint;
		screen.world->asyncModification([&screen] () {
			Picker::moveGrabbedPhysicalLateral(screen);
			});
	} else {
		// Closest point on ray1 (A + s * a) from ray2 (B + t * b). Ray1 is the ray from the parts' center in the direction of the edit tool, ray2 is the mouse ray. Directions a and b are normalized. Only s is calculated.
		Position B = screen.camera.cframe.position;
		Vec3 b = normalize(screen.ray);
		Position A = screen.selectedPart->getPosition();
		Vec3 a;
		switch (selectedEditDirection) {
			case EditDirection::X:
				a = Vec3(1, 0, 0);
				break;
			case EditDirection::Y:
				a = Vec3(0, 1, 0);
				break;
			case EditDirection::Z:
				a = Vec3(0, 0, 1);
				break;
		}

		// Rotate a according to model rotation
		a = screen.selectedPart->getCFrame().localToRelative(a);

		// Calculate s
		Vec3 c = B - A;
		double ab = a * b;
		double bc = b * c;
		double ac = a * c;
		double s = (ac - ab * bc) / (1 - ab * ab);

		// Translation, relative to tool intersection
		Vec3 translationCorrection = a * (a * selectedPoint);
		Vec3 translation = s * a - translationCorrection;

		screen.selectedPart->translate(translation);
	}
}

};