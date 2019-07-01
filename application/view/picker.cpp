#include "picker.h"

#include "material.h"
#include "renderUtils.h"
#include "quad.h"

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

	enum class EditDirection {
		NONE = -1,
		Y = 0,
		X = 1,
		Z = 2,
		CENTER = 3
	};

	struct Tool {
		EditDirection editDirection;
		Vec3f intersectedPoint;
	};

	struct Ray {
		Vec3f start;
		Vec3f direction;
	};

	EditMode editMode = EditMode::TRANSLATE;
	Tool intersectedTool;
	Tool selectedTool;

	IndexedMesh* line = nullptr;

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


	// Init

	void init() {
		// Edit tools init
		line = new IndexedMesh(OBJImport::load("../res/models/gui/line.obj"));

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

	void renderEditTools(Screen& screen, BasicShader& shader) {
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

		Renderer::clearDepth();
		Renderer::enableDepthTest();

		// Center
		shader.updateMaterial(Material(GUI::COLOR::WHITE));
		if (center) 
			center->render();

		// Y
		shader.updateMaterial(Material(GUI::COLOR::G));
		if (intersectedTool.editDirection == EditDirection::Y && editMode != EditMode::ROTATE)
			line->render();
		shaft->render();

		// X
		shader.updateMaterial(Material(GUI::COLOR::R));
		shader.updateModelMatrix(modelMatrix * transformations[1]);
		if (intersectedTool.editDirection == EditDirection::X && editMode != EditMode::ROTATE)
			line->render();
		shaft->render();

		// Z
		shader.updateMaterial(Material(GUI::COLOR::B));
		shader.updateModelMatrix(modelMatrix * transformations[2]);
		if (intersectedTool.editDirection == EditDirection::Z && editMode != EditMode::ROTATE)
			line->render();
		shaft->render();

		Renderer::disableDepthTest();
	}

	void render(Screen& screen, BasicShader& shader) {
		if (screen.selectedPart) {
			renderEditTools(screen, shader);
		}
	}


	// Intersections

	float intersect(const Ray& ray, const VisualShape& shape, const CFramef& cframe) {
		return shape.getIntersectionDistance(cframe.globalToLocal(ray.start), cframe.relativeToLocal(ray.direction));
	}

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
			frame.rotate(transformations[i]);
			float distance = intersect(ray, *tool[i / 3], frame);

			if (distance < closestToolDistance && distance > 0) {
				closestToolDistance = distance;
				closestToolDirection = static_cast<EditDirection>(i);
			}
		}

		// Update intersected tool
		if (closestToolDistance != INFINITY) {
			intersectedTool.editDirection = closestToolDirection;
			intersectedTool.intersectedPoint = ray.start + ray.direction * closestToolDistance;
			return;
		} else {
			intersectedTool.editDirection = EditDirection::NONE;
			intersectedTool.intersectedPoint = Vec3f();
		}
	}

	void intersectPhysicals(Screen& screen, const Ray& ray) {
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

		// Update intersected part
		screen.intersectedPart = closestIntersectedPart;
		screen.intersectedPoint = closestIntersectedPoint;

		// Call callback
		// (*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
	}


	// Update

	void update(Screen& screen, Vec2 mousePosition) {
		// Calculate ray
		Vec3f direction = calcRay(screen, mousePosition);
		Vec3f start = screen.camera.cframe.position;
		Ray ray = { start, direction };

		// Intersect edit tools
		if (screen.selectedPart) {
			intersectTools(screen, ray);
		}
			   
		// Intersect physcials
		intersectPhysicals(screen, ray);
	}


	// Events

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

	void release(Screen& screen) {
		// Reset selectedpart
		screen.world->selectedPart = nullptr;

		// Reset selected tool
		selectedTool.editDirection = EditDirection::NONE;
	}
	
	void drag(Screen& screen) {
		if (selectedTool.editDirection != EditDirection::NONE) {
			Log::debug("moving edit");
		} else {
			if (screen.selectedPart) {
				Picker::moveGrabbedPhysicalLateral(screen);
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

