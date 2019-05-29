#include "picker.h"

#include "material.h"

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

	EditMode editMode = EditMode::TRANSLATE;

	IndexedMesh* rotateX = nullptr;
	IndexedMesh* rotateY = nullptr;
	IndexedMesh* rotateZ = nullptr;

	IndexedMesh* translateCenter = nullptr;
	IndexedMesh* translateX = nullptr;
	IndexedMesh* translateY = nullptr;
	IndexedMesh* translateZ = nullptr;

	IndexedMesh* scaleCenter = nullptr;
	IndexedMesh* scaleX = nullptr;
	IndexedMesh* scaleY = nullptr;
	IndexedMesh* scaleZ = nullptr;

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
		Shape rotateModel = OBJImport::load("../res/models/gui/rotate.obj");
		rotateY = new IndexedMesh(rotateModel);
		rotateX = new IndexedMesh(rotateModel.rotated(fromEulerAngles(0.0, 0.0, -3.14159265359/2.0), new Vec3f[rotateModel.vertexCount]));
		rotateZ = new IndexedMesh(rotateModel.rotated(fromEulerAngles(3.14159265359/2.0, 0.0, 0.0), new Vec3f[rotateModel.vertexCount]));

		Shape scaleShaftModel = OBJImport::load("../res/models/gui/scale_shaft.obj");
		scaleCenter = new IndexedMesh(OBJImport::load("../res/models/gui/scale_center.obj"));
		scaleY = new IndexedMesh(scaleShaftModel);
		scaleX = new IndexedMesh(scaleShaftModel.rotated(fromEulerAngles(0.0, 0.0, -3.14159265359/2.0), new Vec3f[rotateModel.vertexCount]));
		scaleZ = new IndexedMesh(scaleShaftModel.rotated(fromEulerAngles(3.14159265359/2.0, 0.0, 0.0), new Vec3f[rotateModel.vertexCount]));

		Shape translateShaftModel = OBJImport::load("../res/models/gui/translate_shaft.obj");
		translateCenter = new IndexedMesh(OBJImport::load("../res/models/gui/translate_center.obj"));
		translateY = new IndexedMesh(translateShaftModel);
		translateX = new IndexedMesh(translateShaftModel.rotated(fromEulerAngles(0.0, 0.0, -3.14159265359/2.0), new Vec3f[rotateModel.vertexCount]));
		translateZ = new IndexedMesh(translateShaftModel.rotated(fromEulerAngles(3.14159265359/2.0, 0.0, 0.0), new Vec3f[rotateModel.vertexCount]));
	}

	void render(Screen& screen, BasicShader& shader) {
		if (screen.selectedPart) {
			CFrame cframe = screen.selectedPart->cframe;
			Mat4f modelMatrix = CFrameToMat4(cframe);
			shader.updateModelMatrix(modelMatrix);

			IndexedMesh* x = nullptr;
			IndexedMesh* y = nullptr;
			IndexedMesh* z = nullptr;
			IndexedMesh* c = nullptr;

			switch (editMode) {
				case Picker::EditMode::TRANSLATE:
					x = translateX;
					y = translateY;
					z = translateZ;
					c = translateCenter;
					break;
				case Picker::EditMode::ROTATE:
					x = rotateX;
					y = rotateY;
					z = rotateZ;
					break;
				case Picker::EditMode::SCALE:
					x = scaleX;
					y = scaleY;
					z = scaleZ;
					c = scaleCenter;
					break;
			}

			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			shader.updateMaterial(Material(Vec4f(1, 0, 0, 1)));
			x->render();

			shader.updateMaterial(Material(Vec4f(0, 1, 0, 1)));
			y->render();

			shader.updateMaterial(Material(Vec4f(0, 0, 1, 1)));
			z->render();

			shader.updateMaterial(Material(Vec4f(1, 1, 1, 1)));
			if (c) c->render();

			glDisable(GL_DEPTH_TEST);
		}
	}

	void update(Screen& screen, Vec2 mousePosition) {
		// Intersect edit tools
		if (screen.selectedPart) {
			switch (editMode) {
				case Picker::EditMode::TRANSLATE:
					break;
				case Picker::EditMode::ROTATE:
					break;
				case Picker::EditMode::SCALE:
					break;
				default:
					break;
			}
		}

		// Intersect physcials
		SharedLockGuard guard(screen.world->lock);

		ExtendedPart* closestIntersectedPart = nullptr;
		Vec3f closestIntersectedPoint = Vec3f();
		float closestIntersectDistance = INFINITY;

		Vec3f ray = calcRay(screen, mousePosition);
		Vec3f rayStart = screen.camera.cframe.position;
		screen.ray = ray;
		for (ExtendedPart& part : *screen.world) {
			Vec3 relPos = part.cframe.position - rayStart;
			if (ray.pointToLineDistanceSquared(relPos) > part.maxRadius * part.maxRadius)
				continue;

			CFramef cframe = CFramef(part.cframe);

			float distance = part.hitbox.getIntersectionDistance(cframe.globalToLocal(rayStart), cframe.relativeToLocal(ray));

			if (distance < closestIntersectDistance && distance > 0) {
				closestIntersectDistance = distance;
				closestIntersectedPart = &part;
			}
		}

		if (closestIntersectDistance == INFINITY) {
			closestIntersectedPart = nullptr;
			closestIntersectedPoint = screen.camera.cframe.position;
		} else {
			closestIntersectedPoint = screen.camera.cframe.position + screen.ray * closestIntersectDistance;
		}

		(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
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
			screen.selectedPart->cframe.translate(translation);
		} else {
			screen.world->selectedPart = screen.selectedPart;
			screen.world->magnetPoint += translation;
		}
	}
}