#include "core.h"

#include "rotationTool.h"

#include "worlds.h"
#include "application.h"
#include "selectionTool.h"
#include "translationTool.h"
#include "view/screen.h"
#include "shader/shaders.h"

#include "../physics/misc/toString.h"
#include "../physics/geometry/shapeLibrary.h"
#include "../physics/math/rotation.h"
#include "../graphics/visualShape.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"

namespace P3D::Application {
	
	constexpr static Rotation transformations[] {
		Rotation::Predefined::Y_90,	// X
		Rotation::Predefined::X_270, // Y			
		Rotation::Predefined::IDENTITY // Z
	};

	static LinePrimitive* line = nullptr;
	static IndexedMesh* handleMesh;
	static VisualShape handleShape;
	
	void RotationTool::onRegister() {
		using namespace Graphics;

		// Load icon
		std::string path = "../res/textures/icons/" + getName() + ".png";
		ResourceManager::add<TextureResource>(getName(), path);

		// Create alignment line
		line = new LinePrimitive();
		line->resize(Vec3f(0, 0, -100000), Vec3f(0, 0, 100000));
		
		// Create handle shapes
		handleShape = VisualShape::generateSmoothNormalsShape(ShapeLibrary::createTorus(1.0f, 0.03f, 80, 12));
		handleMesh = new IndexedMesh(handleShape);

		// Set idle status
		setToolStatus(kIdle);
	}

	void RotationTool::onDeregister() {
		// Todo remove icon
		// Todo remove line

		handleMesh->close();
	}

	void RotationTool::onRender() {
		using namespace Graphics;
		
		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;

		Mat4f model = cframe->asMat4();
		Mat4f modelX = model * joinDiagonal(Mat3f(transformations[0].asRotationMatrix()), 1.0f);
		Mat4f modelY = model * joinDiagonal(Mat3f(transformations[1].asRotationMatrix()), 1.0f);
		Mat4f modelZ = model * joinDiagonal(Mat3f(transformations[2].asRotationMatrix()), 1.0f);
		Mat4f modelC = GlobalCFrame(cframe->getPosition(), Rotation::fromDirection(Vec3(screen.camera.cframe.position - cframe->getPosition()))).asMat4();

		auto status = getToolStatus();		
		if (status == kRotateX) {
			Shaders::maskShader.updateModel(modelX);
			Shaders::maskShader.updateColor(Colors::RGB_R);
			line->render();
		}

		if (status == kRotateY) {
			Shaders::maskShader.updateModel(modelY);
			Shaders::maskShader.updateColor(Colors::RGB_G);
			line->render();
		}

		if (status == kRotateZ) {
			Shaders::maskShader.updateModel(modelZ);
			Shaders::maskShader.updateColor(Colors::RGB_B);
			line->render();
		}

		Shaders::basicShader.updateModel(modelC);
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::YELLOW));
		handleMesh->render();

		// X
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_R));
		Shaders::basicShader.updateModel(modelX);   
		handleMesh->render();
		
		// Y
		Shaders::basicShader.updateModel(modelY);
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_G));
		handleMesh->render();

		// Z
		Shaders::basicShader.updateMaterial(Comp::Material(Colors::RGB_B));
		Shaders::basicShader.updateModel(modelZ);
		handleMesh->render();
		
	}

	void RotationTool::onUpdate() {
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
		for (char status = kRotateX; status <= kRotateC; status++) {
			if (status == kRotateC) {
				frame.rotation = Rotation::fromDirection(Vec3(screen.camera.cframe.position - cframe->getPosition()));
			} else {
				frame.rotation = cframe->getRotation() * transformations[status - 1];
			}
			
			std::optional<double> distance = SelectionTool::intersect(frame, handleShape);

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

	void RotationTool::onEvent(Engine::Event& event) {
		using namespace Engine;

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MousePressEvent>(EVENT_BIND(RotationTool::onMousePress));
		dispatcher.dispatch<MouseReleaseEvent>(EVENT_BIND(RotationTool::onMouseRelease));
		dispatcher.dispatch<MouseDragEvent>(EVENT_BIND(RotationTool::onMouseDrag));
	}

	bool RotationTool::onMousePress(Engine::MousePressEvent& event) {
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

	bool RotationTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
		using namespace Engine;
		if (event.getButton() != Mouse::LEFT)
			return false;

		// Reset magnet point
		screen.world->selectedPart = nullptr;

		this->active = false;

		return false;
	};

	bool RotationTool::onMouseDrag(Engine::MouseDragEvent& event) {
		if (!this->active)
			return false;
		
		auto status = getToolStatus();
		if (status == kIdle)
			return false;

		screen.world->asyncModification([&] () {
			switch (status) {
				case kRotateX:
					rotateAroundLine({ 1, 0, 0 });
					break;
				case kRotateY:
					rotateAroundLine({ 0, 1, 0 });
					break;
				case kRotateZ:
					rotateAroundLine({ 0, 0, 1 });
					break;
				case kRotateC:
					rotateAroundLine(screen.camera.cframe.rotation * Vec3(0, 0, 1), false);
					break;
				case kTranslateC:
					TranslationTool::translateInPlane(screen.camera.cframe.rotation * Vec3(0, 0, 1), false);
					break;
				default:
					return false;
			}

			return true;
		});

		return false;
	}

	void RotationTool::rotateAroundLine(const Vec3& direction, bool local) {
		if (SelectionTool::selection.empty())
			return;

		std::optional<GlobalCFrame> cframe = SelectionTool::selection.getCFrame();
		if (!cframe.has_value())
			return;
		
		// Plane of edit tool, which can be expressed as all points p where (p - position) * normal = 0. Where n is the edit direction and p0 the center of the selected part
		Position position = cframe->getPosition();

		// Apply model matrix
		Vec3 normal = local ? cframe->localToRelative(direction) : direction;

		// Calculate intersection of mouse ray and edit plane
		double ln = SelectionTool::ray.direction * normal;
		if (ln == 0.0)
			return; // No rotation if plane is perpendicular to mouse ray

		// Vector from part center to intersection
		Position intersection = SelectionTool::ray.origin + (position - SelectionTool::ray.origin) * normal / (SelectionTool::ray.direction * normal) * SelectionTool::ray.direction;
		Vec3 intersectionVector = intersection - position;

		// Length check
		Vec3 relativeSelectedPoint = *SelectionTool::selectedPoint - position;
		double length1sq = lengthSquared(intersectionVector);
		double length2sq = lengthSquared(relativeSelectedPoint);
		if (length1sq == 0.0 || length2sq == 0.0)
			return; // Prevent errors when vector is the zero vector

		// Triple product to find angle sign
		double triple = relativeSelectedPoint * (intersectionVector % normal);
		double sign = triple > 0.0 ? 1.0 : -1.0;

		// Get angle between last intersectionVector and new one
		double cosa = relativeSelectedPoint * intersectionVector / sqrt(length1sq * length2sq);

		// No rotation when vectors coincide
		if (!(std::abs(cosa) < 1))
			return;

		double angle = sign * acos(cosa);

		// Update last intersectionVector
		SelectionTool::selectedPoint = position + intersectionVector;

		// Apply rotation
		SelectionTool::selection.rotate(normal, angle);
	}
}
