#include "core.h"

#include "translationTool.h"

#include "worlds.h"
#include "application.h"
#include "selectionTool.h"
#include "../../../physics/misc/toString.h"
#include "view/screen.h"
#include "shader/shaders.h"

#include "../physics/geometry/shapeLibrary.h"
#include "../physics/math/rotation.h"
#include "../graphics/visualShape.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"

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
	static IndexedMesh* centerMesh;
	static VisualShape centerShape;
	static IndexedMesh* handleMesh;
	static VisualShape handleShape;

	static Polyhedron createArrow(float arrowHeadLength, float arrowHeadRadius, float stickRadius) {
		Vec2f contour[] {
			{ 0.0f, stickRadius },
			{ 1.0f - arrowHeadLength, stickRadius },
			{ 1.0f - arrowHeadLength, arrowHeadRadius }
		};
		
		return ShapeLibrary::createRevolvedShape(0.0f, contour, 3, 1.0f, 24);
	}
	
	void TranslationTool::onRegister() {
		using namespace Graphics;

		// Load icon
		std::string path = "../res/textures/icons/" + getName() + ".png";
		ResourceManager::add<TextureResource>(getName(), path);

		// Create alignment line
		line = new LinePrimitive();
		line->resize(Vec3f(0, 0, -100000), Vec3f(0, 0, 100000));
		
		// Create handle shapes
		handleShape = VisualShape::generateSplitNormalsShape(createArrow(0.3f, 0.07f, 0.03f));
		handleMesh = new IndexedMesh(handleShape);
		centerShape = VisualShape::generateSmoothNormalsShape(ShapeLibrary::createSphere(0.13f, 3));
		centerMesh = new IndexedMesh(centerShape);
		quadShape = VisualShape::generateSplitNormalsShape(ShapeLibrary::createBox(0.02, 0.25, 0.25).translated({0, 0.5, 0.5}));
		quadMesh = new IndexedMesh(quadShape);

		// Set idle status
		setToolStatus(kIdle);
	}

	void TranslationTool::onDeregister() {
		// Todo remove icon
		// Todo remove line

		centerMesh->close();
		handleMesh->close();
		quadMesh->close();
	}

	void TranslationTool::onRender() {
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
		if (status == kTranslateX || status == kTranslateXY || status == kTranslateXZ) {
			Shaders::maskShader.updateModel(modelX);
			Shaders::maskShader.updateColor(Colors::RGB_R);
			line->render();
		}

		if (status == kTranslateY || status == kTranslateXY || status == kTranslateYZ) {
			Shaders::maskShader.updateModel(modelY);
			Shaders::maskShader.updateColor(Colors::RGB_G);
			line->render();
		}

		if (status == kTranslateZ || status == kTranslateXZ || status == kTranslateYZ) {
			Shaders::maskShader.updateModel(modelZ);
			Shaders::maskShader.updateColor(Colors::RGB_B);
			line->render();
		}

		Shaders::basicShader.updateModel(model);
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::WHITE));
		centerMesh->render();

		// X, XY
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_R));
		Shaders::basicShader.updateModel(modelX);   
		handleMesh->render();
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_B));
		quadMesh->render();
		
		// Y, XZ
		Shaders::basicShader.updateModel(modelY);
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_G));
		handleMesh->render();
		Shaders::basicShader.updateModel(modelXZ);
		quadMesh->render();

		// Z, YZ
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_B));
		Shaders::basicShader.updateModel(modelZ);
		handleMesh->render();
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_R));
		quadMesh->render();
		
	}

	void TranslationTool::onUpdate() {
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
		for (char status = kTranslateX; status <= kTranslateYZ; status++) {
			VisualShape shape;
			switch (status) {
				case kTranslateC:
					shape = centerShape;
					break;
				case kTranslateX:
				case kTranslateY:
				case kTranslateZ:
					shape = handleShape;
					break;
				case kTranslateXY:
				case kTranslateXZ:
				case kTranslateYZ:
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

	void TranslationTool::onEvent(Engine::Event& event) {
		using namespace Engine;

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MousePressEvent>(EVENT_BIND(TranslationTool::onMousePress));
		dispatcher.dispatch<MouseReleaseEvent>(EVENT_BIND(TranslationTool::onMouseRelease));
		dispatcher.dispatch<MouseDragEvent>(EVENT_BIND(TranslationTool::onMouseDrag));
	}

	bool TranslationTool::onMousePress(Engine::MousePressEvent& event) {
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

	bool TranslationTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
		using namespace Engine;
		if (event.getButton() != Mouse::LEFT)
			return false;

		// Reset magnet point
		screen.world->selectedPart = nullptr;

		this->active = false;

		return false;
	}

	bool TranslationTool::onMouseDrag(Engine::MouseDragEvent& event) {
		if (!this->active)
			return false;
		
		auto status = getToolStatus();
		if (status == kIdle)
			return false;

		screen.world->asyncModification([&] () {
			switch (status) {
				case kTranslateX:
					translateAlongLine({ 1, 0, 0 });
					break;
				case kTranslateY:
					translateAlongLine({ 0, 1, 0 });
					break;
				case kTranslateZ:
					translateAlongLine({ 0, 0, 1 });
					break;
				case kTranslateC:
					translateInPlane(screen.camera.cframe.rotation * Vec3(0, 0, 1), false);
					break;
				case kTranslateXY:
					translateInPlane({ 0, 0, 1 });
					break;
				case kTranslateXZ:
					translateInPlane({ 0, 1, 0 });
					break;
				case kTranslateYZ:
					translateInPlane({ 1, 0, 0 });
					break;
				default:
					return false;
			}

			return true;
		});

		return false;
	}

	void TranslationTool::translateInPlane(const Vec3& normal, bool local) {
		if (SelectionTool::selection.empty())
			return;

		Vec3 direction;
		if (local) {
			std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
			if (!cframe.has_value())
				return;

			direction = cframe->localToRelative(normal);
		} else {
			direction = normal;
		}
		
		double distance = (*SelectionTool::selectedPoint - SelectionTool::ray.origin) * direction / (SelectionTool::ray.direction * direction);
		Position planeIntersection = SelectionTool::ray.origin + SelectionTool::ray.direction * distance;

		if (isPaused()) {
			Vec3 translation = planeIntersection - *SelectionTool::selectedPoint;
			*SelectionTool::selectedPoint += translation;
			
			SelectionTool::selection.translate(translation);
		} else {
			// Only allow single selection
			if (SelectionTool::selection.size() > 1)
				return;

			Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(SelectionTool::selection[0]);
			if (transform.invalid())
				return;
			if (!transform->isPartAttached())
				return;
			
			screen.world->selectedPart = transform->getPart();
			screen.world->magnetPoint = planeIntersection;
		}
	}

	void TranslationTool::translateAlongLine(const Vec3& direction, bool local) {
		// Closest point on ray1 (A + s * a) from ray2 (B + t * b). Ray1 is the ray from the parts' center in the direction of the edit tool, ray2 is the mouse ray. Directions a and b are normalized. Only s is calculated.
		if (SelectionTool::selection.empty())
			return;
		
		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;
		
		// Rotate direction according to model rotation
		Ray ray1 = { cframe->getPosition(), local ? cframe->localToRelative(direction) : direction };
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
		SelectionTool::selection.translate(translation);
	}

}
