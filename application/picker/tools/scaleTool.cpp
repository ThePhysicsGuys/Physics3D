#include "core.h"

#include "scaleTool.h"

#include "worlds.h"
#include "application.h"
#include "selectionTool.h"
#include "translationTool.h"
#include <Physics3D/misc/toString.h>
#include "view/screen.h"
#include "shader/shaders.h"

#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/math/rotation.h>
#include <Physics3D/threading/upgradeableMutex.h>
#include "../graphics/visualShape.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"
#include "imgui/imgui.h"

namespace P3D::Application {
	
	constexpr static Rotation transformations[] {
		Rotation::Predefined::Y_90,	// X, XY
		Rotation::Predefined::X_270, // Y			
		Rotation::Predefined::IDENTITY, // Z, YZ
		Rotation::Predefined::Z_270 // XZ
	};

	constexpr static std::size_t mapping[] {
		0, // X
		1, // Y
		2, // Z
		2, // C
		0, // XY
		3, // XZ
		2  // YZ
	};

	static LinePrimitive* line = nullptr;
	static IndexedMesh* quadMesh;
	static VisualShape quadShape;
	static IndexedMesh* handleMesh;
	static VisualShape handleShape;
	static IndexedMesh* centerMesh;
	static VisualShape centerShape;

	static Polyhedron createBoxOnStick(float boxSide, float stickRadius) {
		Vec2f vecs[] { { 0.0f, stickRadius }, { 1.0f - boxSide, stickRadius }, { 1.0f - boxSide, boxSide / sqrtf(2.0f) }, { 1.0f, boxSide / sqrtf(2.0f) }};
		return ShapeLibrary::createRevolvedShape(0.0f, vecs, 4, 1.0f, 4).rotated(Rotation::rotZ(3.14159265359 / 4));
	}
	
	void ScaleTool::onRegister() {
		using namespace Graphics;

		// Load icon
		std::string path = "../res/textures/icons/" + getName() + ".png";
		ResourceManager::add<TextureResource>(getName(), path);

		// Create alignment line
		line = new LinePrimitive();
		line->resize(Vec3f(0, 0, -100000), Vec3f(0, 0, 100000));
		
		// Create handle shapes
		handleShape = VisualShape::generateSplitNormalsShape(createBoxOnStick(0.2f, 0.03f));
		handleMesh = new IndexedMesh(handleShape);
		centerShape = VisualShape::generateSplitNormalsShape(ShapeLibrary::createCube(0.2f));
		centerMesh = new IndexedMesh(centerShape);
		quadShape = VisualShape::generateSplitNormalsShape(ShapeLibrary::createBox(0.02, 0.25, 0.25).translated({ 0, 0.5, 0.5 }));
		quadMesh = new IndexedMesh(quadShape);
		
		// Set idle status
		setToolStatus(kIdle);
	}

	void ScaleTool::onDeregister() {
		// Todo remove icon
		// Todo remove line

		handleMesh->close();
		centerMesh->close();
		quadMesh->close();
	}

	void ScaleTool::onRender() {
		using namespace Graphics;
		
		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;

		Mat4f model = cframe->asMat4();
		Mat4f modelX = model * joinDiagonal(Mat3f(transformations[0].asRotationMatrix()), 1.0f);
		Mat4f modelY = model * joinDiagonal(Mat3f(transformations[1].asRotationMatrix()), 1.0f);
		Mat4f modelZ = model * joinDiagonal(Mat3f(transformations[2].asRotationMatrix()), 1.0f);
		Mat4f modelXZ = model * joinDiagonal(Mat3f(transformations[3].asRotationMatrix()), 1.0f);

		auto status = getToolStatus();		
		if (status == kScaleX || status == kScaleXY || status == kScaleXZ || status == kScaleXYZ) {
			Shaders::maskShader->updateModel(modelX);
			Shaders::maskShader->updateColor(Colors::RGB_R);
			line->render();
		}

		if (status == kScaleY || status == kScaleXY || status == kScaleYZ || status == kScaleXYZ) {
			Shaders::maskShader->updateModel(modelY);
			Shaders::maskShader->updateColor(Colors::RGB_G);
			line->render();
		}

		if (status == kScaleZ || status == kScaleXZ || status == kScaleYZ || status == kScaleXYZ) {
			Shaders::maskShader->updateModel(modelZ);
			Shaders::maskShader->updateColor(Colors::RGB_B);
			line->render();
		}

		Shaders::basicShader->updateModel(model);
		Shaders::basicShader->updateMaterial(Comp::Material(Colors::WHITE));
		centerMesh->render();

		// X, XY
		Shaders::basicShader->updateMaterial(Comp::Material(Colors::RGB_R));
		Shaders::basicShader->updateModel(modelX);
		handleMesh->render();
		Shaders::basicShader->updateMaterial(Comp::Material(Colors::RGB_B));
		quadMesh->render();

		// Y, XZ
		Shaders::basicShader->updateModel(modelY);
		Shaders::basicShader->updateMaterial(Comp::Material(Colors::RGB_G));
		handleMesh->render();
		Shaders::basicShader->updateModel(modelXZ);
		quadMesh->render();

		// Z, YZ
		Shaders::basicShader->updateMaterial(Comp::Material(Colors::RGB_B));
		Shaders::basicShader->updateModel(modelZ);
		handleMesh->render();
		Shaders::basicShader->updateMaterial(Comp::Material(Colors::RGB_R));
		quadMesh->render();
	}

	void ScaleTool::onUpdate() {
		// Keep the tool status if the tool is active
		if (this->active)
			return;

		// Reset tool status
		setToolStatus(kIdle);
		SelectionTool::intersectedPoint = std::nullopt;

		// The intersection distance to find
		std::optional<double> closestIntersectionDistance;

		// Look for edit tool intersections if the selection has a cframe
		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;
		
		GlobalCFrame frame = *cframe;
		for (char status = kScaleX; status <= kScaleYZ; status++) {
			VisualShape shape;
			switch (status) {
				case kScaleXYZ:
					shape = centerShape;
					break;
				case kScaleX:
				case kScaleY:
				case kScaleZ:
					shape = handleShape;
					break;
				case kScaleXY:
				case kScaleXZ:
				case kScaleYZ:
					shape = quadShape;
					break;
				default:
					continue;
			}
			
			frame.rotation = cframe->getRotation() * transformations[mapping[status - 1]];
			std::optional<double> distance = SelectionTool::intersect(frame, shape);

			if (!distance.has_value())
				continue;

			if (!closestIntersectionDistance.has_value() || 0.0 < distance && distance < closestIntersectionDistance) {
				closestIntersectionDistance = distance;
				setToolStatus(status);
			}
		}

		if (closestIntersectionDistance.has_value())
			SelectionTool::intersectedPoint = SelectionTool::ray.origin + SelectionTool::ray.direction * *closestIntersectionDistance;
	}

	void ScaleTool::onEvent(Engine::Event& event) {
		using namespace Engine;

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MousePressEvent>(EVENT_BIND(ScaleTool::onMousePress));
		dispatcher.dispatch<MouseReleaseEvent>(EVENT_BIND(ScaleTool::onMouseRelease));
		dispatcher.dispatch<MouseDragEvent>(EVENT_BIND(ScaleTool::onMouseDrag));
	}

	bool ScaleTool::onMousePress(Engine::MousePressEvent& event) {
		using namespace Engine;
		if (event.getButton() != Mouse::LEFT)
			return false;

		// Auto select an entity if the selection status is idle, in that case no edit tool or entity is intersected
		auto status = getToolStatus();
		if (status == kIdle) {
			// Find the intersected entity
			auto intersectedEntity = SelectionTool::getIntersectedEntity();
					
			// If control is pressed, add to selection, but don't change the tool status 
			if (event.getModifiers().isCtrlPressed()) {
				// No intersection causes nothing
				if (!intersectedEntity.has_value())
					return false;
			} else {
				// Clear the selection before selecting the new entity
				SelectionTool::selection.clear();

				// No intersection causes only deselection
				if (!intersectedEntity.has_value())
					return false;

				// If an intersection is found, select it and behave like center edit
				setToolStatus(kTranslateC);
				SelectionTool::intersectedPoint = intersectedEntity->second;
			}

			SelectionTool::select(intersectedEntity->first);
		}

		// Set the selected point 
		SelectionTool::selectedPoint = SelectionTool::intersectedPoint;

		// Set edit status to active
		this->active = true;

		return false;
	}

	bool ScaleTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
		using namespace Engine;
		if (event.getButton() != Mouse::LEFT)
			return false;

		this->active = false;

		TranslationTool::magnet.selectedPart = nullptr;

		return false;
	};

	bool ScaleTool::onMouseDrag(Engine::MouseDragEvent& event) {
		if (!this->active)
			return false;
		
		auto status = getToolStatus();
		if (status == kIdle)
			return false;

		bool clamp = ImGui::GetIO().KeyAlt;

		std::unique_lock<UpgradeableMutex> worldLock(*screen.worldMutex);
		switch (status) {
			case kScaleX:
				scaleAlongLine({ 1, 0, 0 });
				break;
			case kScaleY:
				scaleAlongLine({ 0, 1, 0 });
				break;
			case kScaleZ:
				scaleAlongLine({ 0, 0, 1 });
				break;
			case kScaleXYZ:
				scaleXYZ();
				break;
			case kScaleXY:
				scaleInPlane({ 0, 0, 1 });
				break;
			case kScaleXZ:
				scaleInPlane({ 0, 1, 0 });
				break;
			case kScaleYZ:
				scaleInPlane({ 1, 0, 0 });
				break;
			case kTranslateC:
				TranslationTool::translateInPlane(screen.camera.cframe.rotation * Vec3(0, 0, 1), clamp, false);
				break;
			default:
				return false;
		}
		return true;
	}

	void ScaleTool::scaleAlongLine(const Vec3& direction) {
		// Closest point on ray1 (A + s * a) from ray2 (B + t * b). Ray1 is the ray from the parts' center in the direction of the edit tool, ray2 is the mouse ray. Directions a and b are normalized. Only s is calculated.
		if (SelectionTool::selection.empty())
			return;

		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;

		// Rotate direction according to model rotation
		Ray ray1 = { cframe->getPosition(), cframe->localToRelative(direction) };
		Ray ray2 = SelectionTool::ray;

		// Calculate s
		Vec3 c = ray2.origin - ray1.origin;
		double ab = ray1.direction * ray2.direction;
		double bc = ray2.direction * c;
		double ac = ray1.direction * c;
		double s = (ac - ab * bc) / (1.0 - ab * ab);

		// Translation, relative to tool intersection
		Vec3 translationCorrection = ray1.direction * (ray1.direction * (*SelectionTool::selectedPoint - cframe->getPosition()));
		Vec3 translation = s * ray1.direction - translationCorrection;

		*SelectionTool::selectedPoint += translation;
		SelectionTool::selection.scale(Vec3(1.0, 1.0, 1.0) + translation);
	}

	void ScaleTool::scaleInPlane(const Vec3& normal) {
		if (SelectionTool::selection.empty())
			return;
		
		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;

		Vec3 direction = cframe->localToRelative(normal);
		double distance = (*SelectionTool::selectedPoint - SelectionTool::ray.origin) * direction / (SelectionTool::ray.direction * direction);
		Position planeIntersection = SelectionTool::ray.origin + SelectionTool::ray.direction * distance;

		Vec3 translation = planeIntersection - *SelectionTool::selectedPoint;
		*SelectionTool::selectedPoint += translation;

		SelectionTool::selection.scale(Vec3(1.0, 1.0, 1.0) + translation);
		
	}

	void ScaleTool::scaleXYZ() {
		if (SelectionTool::selection.empty())
			return;

		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;

		std::optional<Shape> hitbox = SelectionTool::selection.getHitbox();
		if (!hitbox.has_value())
			return;
		
		Vec3 direction = normalize(Vec3(screen.camera.cframe.position - cframe->getPosition()));
		double distance = (*SelectionTool::selectedPoint - SelectionTool::ray.origin) * direction / (SelectionTool::ray.direction * direction);
		Position planeIntersection = SelectionTool::ray.origin + SelectionTool::ray.direction * distance;

		double amount = lengthSquared(castPositionToVec3(*SelectionTool::selectedPoint)) - lengthSquared(castPositionToVec3(planeIntersection));
		*SelectionTool::selectedPoint = planeIntersection;
		
		
		SelectionTool::selection.scale({ 1.0 + amount, 1.0 + amount, 1.0 + amount });
	}


}
