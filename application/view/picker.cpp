#include "picker.h"

#include "material.h"
#include "gui\gui.h"

#include "../application.h"
#include "../import.h"

#include "../engine/physical.h"
#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h";
#include "../engine/math/vec4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/sharedLockGuard.h"
#include "../engine/geometry/shape.h"

#include "../util/log.h"

namespace Picker {

	struct Ray {
		Vec3f start;
		Vec3f direction;
	};

	EditMode editMode = EditMode::TRANSLATE;

	IndexedMesh* lineMesh = nullptr;

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

	int intersectedToolDirection;
	Vec3f intersectedToolPoint;

	Mat3f transformations[] = { Mat3f(), Mat3f().rotate(-3.14159265359/2.0, 0, 0, 1), Mat3f().rotate(3.14159265359/2.0, 1, 0, 0), Mat3f() };

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

	void init() {
		// Edit tools init
		lineMesh = new IndexedMesh(OBJImport::load("../res/models/gui/line.obj"));

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
	}

	void render(Screen& screen, BasicShader& shader) {
		if (screen.selectedPart) {
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
			Mat4f modelMatrix = CFrameToMat4(cframe);
			shader.updateModelMatrix(modelMatrix);

			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			// Center
			shader.updateMaterial(Material(GUI::COLOR::WHITE));
			if (center) center->render();

			// Y
			shader.updateMaterial(Material(GUI::COLOR::G));
			if (intersectedToolDirection == 0 && editMode != EditMode::ROTATE) lineMesh->render(GL_LINE);
			shaft->render();

			// X
			shader.updateMaterial(Material(GUI::COLOR::R));
			shader.updateModelMatrix(modelMatrix * transformations[1]);
			if (intersectedToolDirection == 1 && editMode != EditMode::ROTATE) lineMesh->render(GL_LINE);
			shaft->render();

			// Z
			shader.updateMaterial(Material(GUI::COLOR::B));
			shader.updateModelMatrix(modelMatrix * transformations[2]);
			if (intersectedToolDirection == 2 && editMode != EditMode::ROTATE) lineMesh->render(GL_LINE);
			shaft->render();

			glDisable(GL_DEPTH_TEST);
		}
	}

	float intersect(const Ray& ray, const VisualShape& shape, const CFramef& cframe) {
		return shape.getIntersectionDistance(cframe.globalToLocal(ray.start), cframe.relativeToLocal(ray.direction));
	}

	void update(Screen& screen, Vec2 mousePosition) {
		// Calculate ray
		Vec3f direction = calcRay(screen, mousePosition);
		Vec3f start = screen.camera.cframe.position;
		Ray ray = { start, direction };

		// Intersect edit tools
		if (screen.selectedPart) {
			VisualShape* tool[2];
	
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
			int closestToolDirection = -1;
	
			// Check shafts
			for (int i = 0; i < (tool[1]? 4 : 3); i++) {
				CFrame frame = screen.selectedPart->cframe;
				frame.rotate(transformations[i]);
				float distance = intersect(ray, *tool[i / 3], frame);

				if (distance < closestToolDistance && distance > 0) {
					closestToolDistance = distance;
					closestToolDirection = i;
				}
			}

			if (closestToolDistance != INFINITY) {
				intersectedToolPoint = ray.start + ray.direction * closestToolDistance;
				intersectedToolDirection = closestToolDirection;
				return;
			} else {
				intersectedToolPoint = ray.start;
				intersectedToolDirection = -1;
			}
		}

		// Intersect physcials
		SharedLockGuard guard(screen.world->lock);

		ExtendedPart* closestIntersectedPart = nullptr;
		Vec3f closestIntersectedPoint = Vec3f();
		float closestIntersectDistance = INFINITY;

		screen.ray = ray.direction;
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

		(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
	}

	void press(Screen& screen) {
		(*screen.eventHandler.partClickHandler) (screen, screen.intersectedPart, screen.intersectedPoint);
		if (screen.intersectedPart) {
			screen.world->localSelectedPoint = screen.selectedPart->cframe.globalToLocal(screen.intersectedPoint);
		}
	}

	void release(Screen& screen) {}

	void moveGrabbedPhysicalLateral(Screen& screen) {
		if (screen.selectedPart == nullptr) return;

		Mat3 cameraFrame = (screen.camera.cframe.rotation).transpose();
		Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);

		double distance = (screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Vec3 planeIntersection = screen.camera.cframe.position + distance * screen.ray;

	if (isPaused()) {
		Vec3 translation = planeIntersection - screen.selectedPoint;
		screen.selectedPoint += translation;
		screen.selectedPart->parent->setCFrame(screen.selectedPart->parent->cframe + translation);
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
			screen.selectedPart->parent->setCFrame(screen.selectedPart->parent->cframe + translation);
		} else {
			screen.world->selectedPart = screen.selectedPart;
			screen.world->magnetPoint += translation;
		}
	}
}