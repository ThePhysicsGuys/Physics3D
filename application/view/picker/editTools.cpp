#include "editTools.h"

#include "../engine/math/vec.h"
#include "../engine/math/mat3.h"

#include "picker.h"

#include "../mesh/indexedMesh.h"
#include "../mesh/primitive.h"

#include "../screen.h"
#include "../renderUtils.h"
#include "../shaderProgram.h"

#include "../application/io/import.h"

#include "../../visualShape.h"

#include "../gui/gui.h"


Mat3 transformations[] = {
	Mat3(),
	Mat3().rotate(-3.14159265359/2.0, 0, 0, 1),
	Mat3().rotate(3.14159265359/2.0, 1, 0, 0),
	Mat3()
};


// Render variables
Line* line = nullptr;

IndexedMesh* rotateMesh = nullptr;
VisualShape rotateShape;

IndexedMesh* translateCenterMesh = nullptr;
VisualShape translateCenterShape;
IndexedMesh* translateMesh = nullptr;
VisualShape translateShape;

IndexedMesh* scaleCenterMesh = nullptr;
VisualShape scaleCenterShape;
IndexedMesh* scaleMesh = nullptr;
VisualShape scaleShape;


void EditTools::init() {

	// Edit line init
	line = new Line();
	line->resize(Vec3f(0, -100000, 0), Vec3f(0, 100000, 0));

	// Rotate shape init
	rotateShape = OBJImport::load("../res/models/gui/rotate.obj");
	rotateMesh = new IndexedMesh(rotateShape);

	// Scale shape init
	scaleShape = OBJImport::load("../res/models/gui/scale_shaft.obj");
	scaleCenterShape = OBJImport::load("../res/models/gui/scale_center.obj");
	scaleMesh = new IndexedMesh(scaleShape);
	scaleCenterMesh = new IndexedMesh(scaleCenterShape);

	// Translate shape init
	translateShape = OBJImport::load("../res/models/gui/translate_shaft.obj");
	translateCenterShape = OBJImport::load("../res/models/gui/translate_center.obj");
	translateMesh = new IndexedMesh(translateShape);
	translateCenterMesh = new IndexedMesh(translateCenterShape);

	// Intersected tool init
	intersectedEditDirection = EditDirection::NONE;
	intersectedPoint = Vec3f();

	// Selected tool init
	selectedEditDirection = EditDirection::NONE;
	selectedPoint = Vec3f();
}


void EditTools::render(Screen& screen) {
	IndexedMesh* shaft = nullptr;
	IndexedMesh* center = nullptr;

	// Select correct render meshes
	switch (editMode) {
	case EditMode::TRANSLATE:
		shaft = translateMesh;
		center = translateCenterMesh;
		break;
	case EditMode::ROTATE:
		shaft = rotateMesh;
		center = nullptr;
		break;
	case EditMode::SCALE:
		shaft = scaleMesh;
		center = scaleCenterMesh;
		break;
	}

	GlobalCFrame cframe = screen.selectedPart->cframe;
	Mat4 modelMatrix = CFrameToMat4(cframe);

	Renderer::clearDepth();
	Renderer::enableDepthTest();

	// Center, only rendered if editMode != EditMode::ROTATE
	if (center) {
		Shaders::basicShader.updateModel(modelMatrix);
		Shaders::basicShader.updateMaterial(Material(GUI::COLOR::WHITE));
		center->render();
	}

	if (selectedEditDirection != EditDirection::NONE) {
		switch (selectedEditDirection) {
		case EditDirection::Y:
			Shaders::maskShader.updateModel(modelMatrix);
			Shaders::maskShader.updateColor(GUI::COLOR::G);
			break;
		case EditDirection::X:
			Shaders::maskShader.updateModel(modelMatrix * transformations[1]);
			Shaders::maskShader.updateColor(GUI::COLOR::R);
			break;
		case EditDirection::Z:
			Shaders::maskShader.updateModel(modelMatrix * transformations[2]);
			Shaders::maskShader.updateColor(GUI::COLOR::B);
			break;
		}
		line->render();
	}

	// Y
	Shaders::basicShader.updateModel(modelMatrix);
	Shaders::basicShader.updateMaterial(Material(GUI::COLOR::G));
	shaft->render();

	// X
	Shaders::basicShader.updateMaterial(Material(GUI::COLOR::R));
	Shaders::basicShader.updateModel(modelMatrix * transformations[1]);
	shaft->render();

	// Z
	Shaders::basicShader.updateMaterial(Material(GUI::COLOR::B));
	Shaders::basicShader.updateModel(modelMatrix * transformations[2]);
	shaft->render();

	Renderer::disableDepthTest();
}


float EditTools::intersect(Screen& screen, const Ray& ray) {
	VisualShape* tool[2];

	// Select correct tools
	switch (editMode) {
	case EditMode::TRANSLATE:
		tool[0] = &translateShape;
		tool[1] = &translateCenterShape;
		break;
	case EditMode::ROTATE:
		tool[0] = &rotateShape;
		tool[1] = nullptr;
		break;
	case EditMode::SCALE:
		tool[0] = &scaleShape;
		tool[1] = &scaleCenterShape;
		break;
	}

	float closestToolDistance = INFINITY;
	EditDirection closestToolDirection = EditDirection::NONE;

	// Check intersections of selected tool
	for (int i = 0; i < (tool[1] ? 4 : 3); i++) {
		GlobalCFrame frame = screen.selectedPart->cframe;

		frame.rotation = frame.getRotation() * transformations[i];

		VisualShape& shape = *tool[i / 3];

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
		intersectedPoint = (ray.start + ray.direction * closestToolDistance) - screen.selectedPart->cframe.position;
		return closestToolDistance;
	} else {
		intersectedEditDirection = EditDirection::NONE;
		intersectedPoint = Vec3f();
		return -1;
	}
}


void EditTools::press(Screen& screen) {
	selectedEditDirection = intersectedEditDirection;
	selectedPoint = intersectedPoint;
}


void EditTools::release(Screen& screen) {
	selectedEditDirection = EditDirection::NONE;
}

void EditTools::drag(Screen& screen) {
	if (screen.selectedPart == nullptr) 
		return;
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
}


// Drag behaviour of rotate tool
void EditTools::dragRotateTool(Screen& screen) {
	// Plane of edit tool, which can be expressed as all points p where (p - p0) * n = 0. Where n is the edit direction and p0 the center of the selected part
	Vec3 n;
	Position p0 = screen.selectedPart->cframe.position;
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
	n = screen.selectedPart->cframe.localToRelative(n);

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
	double a;

	if (abs(cosa) < 1)
		a = sign * acos(cosa);
	else
		return; // No rotation when vectors coincide

	// Update last intersectionVector
	selectedPoint = intersectionVector;

	// Apply rotation
	Mat3 rotation = rotateAround(a, n);
	screen.selectedPart->cframe.rotate(rotation);
}

// Drag behaviour of scale tool
void EditTools::dragScaleTool(Screen& screen) {
	Vec3 ray = screen.ray;
	Vec3 deltaPos = screen.camera.cframe.position - screen.selectedPart->cframe.position;
	double distance = length(deltaPos - (deltaPos * ray) * ray) / length(selectedPoint);

	Vec3 a;
	switch (selectedEditDirection) {
	case EditDirection::X:
		screen.selectedPart->scale(distance / screen.selectedPart->localBounds.getWidth() * 2, 1, 1);
		break;
	case EditDirection::Y:
		screen.selectedPart->scale(1, distance / screen.selectedPart->localBounds.getHeight() * 2, 1);
		break;
	case EditDirection::Z:
		screen.selectedPart->scale(1, 1, distance / screen.selectedPart->localBounds.getDepth() * 2);
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
		screen.selectedPoint = screen.selectedPart->cframe.position + selectedPoint;
		Picker::moveGrabbedPhysicalLateral(screen);
	} else {
		// Closest point on ray1 (A + s * a) from ray2 (B + t * b). Ray1 is the ray from the parts' center in the direction of the edit tool, ray2 is the mouse ray. Directions a and b are normalized. Only s is calculated.
		Position B = screen.camera.cframe.position;
		Vec3 b = normalize(screen.ray);
		Position A = screen.selectedPart->cframe.position;
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
		a = screen.selectedPart->cframe.localToRelative(a);

		// Calculate s
		Vec3 c = B - A;
		double ab = a * b;
		double bc = b * c;
		double ac = a * c;
		double s = (ac - ab * bc) / (1 - ab * ab);

		// Translation, relative to tool intersection
		Vec3 translationCorrection = a * (a * selectedPoint);
		Vec3 translation = s * a - translationCorrection;

		screen.selectedPart->parent->translate(translation);
	}
}
