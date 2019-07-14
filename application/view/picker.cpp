#include "picker.h"

#include "material.h"
#include "renderUtils.h"
#include "primitive.h"
#include "shaderProgram.h"

#include "gui\gui.h"

#include "../application.h"
#include "../import.h"
#include "indexedMesh.h"

#include "../engine/physical.h"
#include "../engine/math/mat3.h"
#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h";
#include "../engine/math/vec4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/sharedLockGuard.h"
#include "../engine/geometry/shape.h"

#include "../util/log.h"

namespace Picker {

	enum class EditDirection {
		NONE = -1,
		Y = 0,
		X = 1,
		Z = 2,
		CENTER = 3
	};

	struct Tool {
		EditDirection editDirection;
		Vec3f relativeIntersectedPoint;
	};

	struct Ray {
		Vec3f start;
		Vec3f direction;
	};

	EditMode editMode = EditMode::TRANSLATE;
	Tool intersectedTool;
	Tool selectedTool;

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

	Mat3 transformations[] = { Mat3(), Mat3().rotate(-3.14159265359/2.0, 0, 0, 1), Mat3().rotate(3.14159265359/2.0, 1, 0, 0), Mat3() };

	Vec2f getNormalizedDeviceSpacePosition(Vec2f viewportSpacePosition, Vec2f screenSize) {
		float x = 2 * viewportSpacePosition.x / screenSize.x - 1;
		float y = 2 * viewportSpacePosition.y / screenSize.y - 1;
		return Vec2f(x, -y);
	}

	Vec3f calcRay(Screen& screen, Vec2f mousePosition) {
		Vec2f normalizedDeviceSpacePosition = getNormalizedDeviceSpacePosition(mousePosition, screen.dimension);
		Vec4f clipSpacePosition = Vec4f(normalizedDeviceSpacePosition.x, normalizedDeviceSpacePosition.y, -1.0f, 1.0f);
		Vec4f eyeCoordinates = screen.camera.invertedProjectionMatrix * clipSpacePosition;
		eyeCoordinates = Vec4f(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
		Vec4f rayWorld = screen.camera.invertedViewMatrix * eyeCoordinates;
		Vec3f rayDirection = Vec3f(rayWorld.x, rayWorld.y, rayWorld.z).normalize();

		return rayDirection;
	}


	// Init

	void init() {
		// Edit tools init
		line = new Line();
		line->resize(Vec3f(0, -100000, 0), Vec3f(0, 100000, 0));

		rotateShape = OBJImport::load("../res/models/gui/rotate.obj");
		rotateMesh = new IndexedMesh(rotateShape);

		scaleShape = OBJImport::load("../res/models/gui/scale_shaft.obj");
		scaleCenterShape = OBJImport::load("../res/models/gui/scale_center.obj");
		scaleMesh = new IndexedMesh(scaleShape);
		scaleCenterMesh = new IndexedMesh(scaleCenterShape);

		translateShape = OBJImport::load("../res/models/gui/translate_shaft.obj");
		translateCenterShape = OBJImport::load("../res/models/gui/translate_center.obj");
		translateMesh = new IndexedMesh(translateShape);
		translateCenterMesh = new IndexedMesh(translateCenterShape);

		intersectedTool = { EditDirection::NONE, Vec3f() };
		selectedTool = { EditDirection::NONE, Vec3f() };
	}


	// Render
	// Rendering of edit tools
	void renderEditTools(Screen& screen) {
		IndexedMesh* shaft = nullptr;
		IndexedMesh* center = nullptr;

		switch (editMode) {
		case Picker::EditMode::TRANSLATE:
			shaft = translateMesh;
			center = translateCenterMesh;
			break;
		case Picker::EditMode::ROTATE:
			shaft = rotateMesh;
			center = nullptr;
			break;
		case Picker::EditMode::SCALE:
			shaft = scaleMesh;
			center = scaleCenterMesh;
			break;
		}

		CFrame cframe = screen.selectedPart->cframe;
		Mat4 modelMatrix = normalizedCFrameToMat4(cframe);

		Renderer::clearDepth();
		Renderer::enableDepthTest();

		// Center, only rendered if editMode != EditMode::ROTATE
		if (center) {
			Shaders::basicShader.updateModel(modelMatrix);
			Shaders::basicShader.updateMaterial(Material(GUI::COLOR::WHITE));
			center->render();
		}

		if (selectedTool.editDirection != EditDirection::NONE) {
			switch (selectedTool.editDirection) {
			case EditDirection::Y:
				Shaders::lineShader.updateModel(modelMatrix);
				Shaders::lineShader.updateColor(GUI::COLOR::G);
				break;
			case EditDirection::X:
				Shaders::lineShader.updateModel(modelMatrix * transformations[1]);
				Shaders::lineShader.updateColor(GUI::COLOR::R);
				break;
			case EditDirection::Z:
				Shaders::lineShader.updateModel(modelMatrix * transformations[2]);
				Shaders::lineShader.updateColor(GUI::COLOR::B);
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

	// Picker render function
	void render(Screen& screen) {
		if (screen.selectedPart) {
			renderEditTools(screen);
		}
	}


	// Intersections
	// Intersection distance of the given ray with the given shape, transformed with the given cframe. 
	float intersect(const Ray& ray, const VisualShape& shape, const CFramef& cframe) {
		return shape.getIntersectionDistance(cframe.globalToLocal(ray.start), cframe.relativeToLocal(ray.direction));
	}

	// Calculates intersections of the given ray with the edit tools, tranformed with the cframe of the current selected part
	void intersectTools(Screen& screen, const Ray& ray) {
		VisualShape* tool[2];

		// Select correct tools
		switch (editMode) {
			case Picker::EditMode::TRANSLATE:
				tool[0] = &translateShape;
				tool[1] = &translateCenterShape;
				break;
			case Picker::EditMode::ROTATE:
				tool[0] = &rotateShape;
				tool[1] = nullptr;
				break;
			case Picker::EditMode::SCALE:
				tool[0] = &scaleShape;
				tool[1] = &scaleCenterShape;
				break;
		}

		float closestToolDistance = INFINITY;
		EditDirection closestToolDirection = EditDirection::NONE;

		// Check intersections of selected tool
		for (int i = 0; i < (tool[1] ? 4 : 3); i++) {
			CFrame frame = screen.selectedPart->cframe;

			frame.rotation = frame.getRotation() * transformations[i];

			float distance = intersect(ray, *tool[i / 3], frame);

			if (distance < closestToolDistance && distance > 0) {
				closestToolDistance = distance;
				closestToolDirection = static_cast<EditDirection>(i);
			}
		}

		// Update intersected tool
		if (closestToolDistance != INFINITY) {
			intersectedTool.editDirection = closestToolDirection;
			intersectedTool.relativeIntersectedPoint = (ray.start + ray.direction * closestToolDistance) - screen.selectedPart->cframe.position;
			return;
		} else {
			intersectedTool.editDirection = EditDirection::NONE;
			intersectedTool.relativeIntersectedPoint = Vec3f();
		}
	}

	// Calculates the closest intersection of the given ray with the physicals in the world
	void intersectPhysicals(Screen& screen, const Ray& ray) {
		SharedLockGuard guard(screen.world->lock);

		ExtendedPart* closestIntersectedPart = nullptr;
		Vec3f closestIntersectedPoint = Vec3f();
		float closestIntersectDistance = INFINITY;

		for (ExtendedPart& part : *screen.world) {
			Vec3 relPos = part.cframe.position - ray.start;
			if (ray.direction.pointToLineDistanceSquared(relPos) > part.maxRadius * part.maxRadius)
				continue;

			float distance = intersect(ray, part.visualShape, part.cframe);

			if (distance < closestIntersectDistance && distance > 0) {
				closestIntersectDistance = distance;
				closestIntersectedPart = &part;
			}
		}

		if (closestIntersectDistance == INFINITY) {
			closestIntersectedPart = nullptr;
			closestIntersectedPoint = ray.start;
		} else {
			closestIntersectedPoint = ray.start + ray.direction * closestIntersectDistance;
		}

		// Update intersected part
		screen.intersectedPart = closestIntersectedPart;
		screen.intersectedPoint = closestIntersectedPoint;

		// Call callback
		(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
	}


	// Update
	// Calculates the mouse ray and the tool & physical intersections
	void update(Screen& screen, Vec2 mousePosition) {
		// Calculate ray
		Vec3f direction = calcRay(screen, mousePosition);
		Vec3f start = screen.camera.cframe.position;
		Ray ray = { start, direction };
		
		// Update screen ray
		screen.ray = ray.direction;

		// Intersect edit tools
		if (screen.selectedPart) {
			intersectTools(screen, ray);
			// No physical intersection check needed if tool is being intersected
			if (intersectedTool.editDirection != EditDirection::NONE)
				return;
		}

		// Intersect physcials
		intersectPhysicals(screen, ray);
	}


	// Events
	// Mouse press behaviour
	void press(Screen& screen) {
		// Check if users pressed on tool
		if (intersectedTool.editDirection != EditDirection::NONE) {
			selectedTool = intersectedTool;
		} else { // Keep current part selected as long as tool is being used
			// Update selected part
			screen.selectedPart = screen.intersectedPart;
			screen.selectedPoint = screen.intersectedPoint;

			// Update intersected point if a physical has been intersected and move physical
			if (screen.intersectedPart) {
				screen.world->localSelectedPoint = screen.selectedPart->cframe.globalToLocal(screen.intersectedPoint);
				moveGrabbedPhysicalLateral(screen);
			}
		}
	}

	// Mouse release behaviour
	void release(Screen& screen) {
		// Reset selectedpart
		screen.world->selectedPart = nullptr;

		// Reset selected tool
		selectedTool.editDirection = EditDirection::NONE;
	}

	// Drag behaviour of translate tool
	void dragTranslateTool(Screen& screen) {
		if (selectedTool.editDirection == EditDirection::CENTER) {
			screen.selectedPoint = screen.selectedPart->cframe.position + selectedTool.relativeIntersectedPoint;
			moveGrabbedPhysicalLateral(screen);
		} else {
			// Closest point on ray1 (A + s * a) from ray2 (B + t * b). Ray1 is the ray from the parts' center in the direction of the edit tool, ray2 is the mouse ray. Directions a and b are normalized. Only s is calculated.
			Vec3 B = screen.camera.cframe.position;
			Vec3 b = screen.ray.normalize();
			Vec3 A = screen.selectedPart->cframe.position;
			Vec3 a;
			switch (selectedTool.editDirection) {
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
			Vec3 translationCorrection = a * (a * selectedTool.relativeIntersectedPoint);
			Vec3 translation = s * a - translationCorrection;

			screen.selectedPart->parent->translate(translation);
		}
	}

	// Drag behaviour of rotate tool
	void dragRotateTool(Screen& screen) {
		// Plane of edit tool, which can be expressed as all points p where (p - p0) * n = 0. Where n is the edit direction and p0 the center of the selected part
		Vec3 n;
		Vec3 p0 = screen.selectedPart->cframe.position;
		switch (selectedTool.editDirection) {
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
		Vec3 l0 = screen.camera.cframe.position;
		Vec3 l = screen.ray;

		// Calculate intersection of mouse ray and edit plane
		double ln = l * n;
		if (ln == 0)
			return; // No rotation if plane is perpendicular to mouse ray
		Vec3 intersection = l0 + ((p0 - l0) * n) / (l * n) * l;

		// Vector from part center to intersection
		Vec3 intersectionVector = intersection - p0;

		// Length check
		double length1sq = intersectionVector.lengthSquared();
		double length2sq = selectedTool.relativeIntersectedPoint.lengthSquared();
		if (length1sq == 0 || length2sq == 0)
			return; // Prevent errors when vector is the zero vector

		// Triple product to find angle sign
		double triple = selectedTool.relativeIntersectedPoint * (intersectionVector % n);
		double sign = (triple > 0) ? 1 : -1;

		// Get angle between last intersectionVector and new one
		double cosa = (selectedTool.relativeIntersectedPoint * intersectionVector) / sqrt(length1sq * length2sq);
		double a;

		if (abs(cosa) < 1)
			a = sign * acos(cosa);
		else
			return; // No rotation when vectors coincide

		// Update last intersectionVector
		selectedTool.relativeIntersectedPoint = intersectionVector;

		// Apply rotation
		Mat3 rotation = rotateAround(a, n);
		screen.selectedPart->cframe.rotate(rotation);
	}

	// Drag behaviour of scale tool
	void dragScaleTool(Screen& screen) {
		// Not completely supported by physics engine
		Vec3 a;
		switch (selectedTool.editDirection) {
			case EditDirection::X:
				a = Vec3(1, 0, 0);
				break;
			case EditDirection::Y:
				a = Vec3(0, 1, 0);
				break;
			case EditDirection::Z:
				a = Vec3(0, 0, 1);
				break;
			case EditDirection::CENTER:
				a = Vec3(1.0/1.73205, 1.0/1.73205, 1/1.73205);
				break;
		}

		Vec3 A = screen.selectedPart->cframe.position;
		Vec3 B = screen.camera.cframe.position;
		Vec3 b = screen.ray;
		Vec3 AB = B - A;

		double distance = (AB - (AB * b) * b).length();
		Vec3 scale = distance * a / selectedTool.relativeIntersectedPoint.length();

		Mat3 scaledRotation = screen.selectedPart->cframe.rotation;

		double sx = (scale.x == 0) ? 1 : scale.x / Vec3(scaledRotation.m00, scaledRotation.m01, scaledRotation.m02).length();
		double sy = (scale.y == 0) ? 1 : scale.y / Vec3(scaledRotation.m10, scaledRotation.m11, scaledRotation.m12).length();
		double sz = (scale.z == 0) ? 1 : scale.z / Vec3(scaledRotation.m20, scaledRotation.m21, scaledRotation.m22).length();
		
		screen.selectedPart->cframe.rotation = scaledRotation.scale(sx, sy, sz);
	}

	// Mouse drag behaviour
	void drag(Screen& screen) {
		if (selectedTool.editDirection != EditDirection::NONE) {
			switch (editMode) {
				case Picker::EditMode::TRANSLATE:
					dragTranslateTool(screen);
					break;
				case Picker::EditMode::ROTATE:
					dragRotateTool(screen);
					break;
				case Picker::EditMode::SCALE:
					dragScaleTool(screen);
					break;
			}
		} else {
			if (screen.selectedPart) {
				moveGrabbedPhysicalLateral(screen);
			}
		}
	}

	void moveGrabbedPhysicalLateral(Screen& screen) {
		if (screen.selectedPart == nullptr) return;

		Mat3 cameraFrame = (screen.camera.cframe.rotation).transpose();
		Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);

		double distance = (screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Vec3 planeIntersection = screen.camera.cframe.position + distance * screen.ray;

		if (isPaused()) {
			Vec3 translation = planeIntersection - screen.selectedPoint;
			screen.selectedPoint += translation;
			screen.selectedPart->parent->translate(translation);
		} else {
			screen.world->selectedPart = screen.selectedPart;
			screen.world->magnetPoint = planeIntersection;
		}
	}

	void moveGrabbedPhysicalTransversal(Screen& screen, double dz) {
		if (screen.selectedPart == nullptr) return;

		Mat3 cameraFrame = screen.camera.cframe.rotation.transpose();
		Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);
		Vec3 cameraYDirection = Vec3(cameraDirection.x, 0, cameraDirection.z).normalize();

		double distance = (screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Vec3 planeIntersection = screen.camera.cframe.position + distance * screen.ray;

		Vec3 translation = -cameraYDirection * dz;
		if (isPaused()) {
			screen.selectedPoint += translation;
			screen.selectedPart->parent->translate(translation);
		} else {
			screen.world->selectedPart = screen.selectedPart;
			screen.world->magnetPoint += translation;
		}
	}
}

