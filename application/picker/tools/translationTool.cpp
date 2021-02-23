#include "core.h"

#include "translationTool.h"

#include "application.h"
#include "view/screen.h"
#include "shader/shaders.h"

#include "../physics/misc/shapeLibrary.h"
#include "../physics/math/rotation.h"
#include "../graphics/visualShape.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"
#include "../selectionContext.h"
#include "ecs/components.h"

namespace P3D::Application {
	
	constexpr static Rotation transformations[] {
		Rotation::Predefined::IDENTITY,
		Rotation::Predefined::Z_270,
		Rotation::Predefined::X_90,
		Rotation::Predefined::IDENTITY
	};

	static LinePrimitive* line = nullptr;
	static Quad* quad = nullptr;
	static IndexedMesh* centerMesh;
	static VisualShape centerShape;
	static IndexedMesh* handleMesh;
	static VisualShape handleShape;

	static Polyhedron createArrow(float arrowHeadLength, float arrowHeadRadius, float stickRadius) {
		Vec2f contour[] {
			{ 0.0f, stickRadius },
			{1.0f - arrowHeadLength, stickRadius},
			{1.0f - arrowHeadLength, arrowHeadRadius}
		};
		
		return Library::createRevolvedShape(0.0f, contour, 3, 1.0f, 24);
	}
	
	void TranslationTool::onRegister() {
		using namespace Graphics;

		// Load icon
		std::string path = "../res/textures/icons/" + getName() + ".png";
		ResourceManager::add<TextureResource>(getName(), path);

		// Create alignment line
		line = new LinePrimitive();
		line->resize(Vec3f(0, -100000, 0), Vec3f(0, 100000, 0));

		// Create handle shapes
		handleShape = VisualShape::generateSplitNormalsShape(createArrow(0.3f, 0.07f, 0.03f).rotated(Rotationf::Predefined::X_270));
		handleMesh = new IndexedMesh(handleShape);
		centerShape = VisualShape::generateSmoothNormalsShape(Library::createSphere(0.13f, 3));
		centerMesh = new IndexedMesh(centerShape);

		// Reset points
		selectedPoint = Vec3f();
		intersectedPoint = Vec3f();
		
		// Set idle status
		setToolStatus(kIdle);
	}

	void TranslationTool::onDeregister() {
		// Todo remove icon
		// Todo remove line

		centerMesh->close();
		handleMesh->close();
	}

	void TranslationTool::onRender() {
		using namespace Graphics;
		if (!screen.selectedEntity)
			return;

		if (getToolStatus() == kIdle)
			return;
		
		Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(screen.selectedEntity);
		if (transform.invalid())
			return;

		Mat4f modelY = transform->getModelMatrix(false);
		Mat4f modelX = modelY * joinDiagonal(Mat3f(transformations[1].asRotationMatrix()), 1.0f);
		Mat4f modelZ = modelY * joinDiagonal(Mat3f(transformations[2].asRotationMatrix()), 1.0f);
		switch (getToolStatus()) {
			case kEditY:
				Shaders::maskShader.updateModel(modelY);
				Shaders::maskShader.updateColor(COLOR::RGB_G);
				line->render();
				break;
			case kEditX:
				Shaders::maskShader.updateModel(modelX);
				Shaders::maskShader.updateColor(COLOR::RGB_R);
				line->render();
				break;
			case kEditZ:
				Shaders::maskShader.updateModel(modelZ);
				Shaders::maskShader.updateColor(COLOR::RGB_B);
				line->render();
				break;
			default:
				break;
		}

		Shaders::basicShader.updateModel(modelY);
		Shaders::basicShader.updateMaterial(Comp::Material(COLOR::WHITE));
		centerMesh->render();

		// Y
		Shaders::basicShader.updateModel(modelY);
		Shaders::basicShader.updateMaterial(Comp::Material(COLOR::RGB_G));
		handleMesh->render();

		// X
		Shaders::basicShader.updateMaterial(Comp::Material(COLOR::RGB_R));
		Shaders::basicShader.updateModel(modelX);
		handleMesh->render();

		// Z
		Shaders::basicShader.updateMaterial(Comp::Material(COLOR::RGB_B));
		Shaders::basicShader.updateModel(modelZ);
		handleMesh->render();
	}

	void TranslationTool::onUpdate() {
		if (!screen.selectedEntity)
			return;

		Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(screen.selectedEntity);
		if (transform.invalid())
			return;

		float closestToolDistance = INFINITY;
		Engine::ToolStatus closestToolDirection = kIdle;

		VisualShape shape = centerShape;
		for (std::size_t index = 0; index < 4; index++) {
			GlobalCFrame frame = transform->getCFrame();
			frame.rotation = frame.getRotation() * transformations[index];

			//float distance = shape.getIntersectionDistance(frame.globalToLocal(ray.start), frame.relativeToLocal(ray.direction));

			/*if (distance < closestToolDistance && distance > 0) {
				closestToolDistance = distance;
				//closestToolDirection = index;
			}*/
			
			shape = handleShape;
		}
	}

}
