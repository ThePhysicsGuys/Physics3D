#include <core.h>
#include "pathTool.h"

#include "application/application.h"
#include "application/view/screen.h"
#include "selectionTool.h"
#include "util/resource/resourceManager.h"
#include "graphics/resource/textureResource.h"
#include "application/shader/shaders.h"
#include "graphics/meshRegistry.h"
#include "graphics/mesh/primitive.h"

namespace P3D::Application {

static URef<LinePrimitive> deltaLine = nullptr;

void PathTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);

	deltaLine = std::make_unique<LinePrimitive>();
}

void PathTool::onDeregister() {

}

void PathTool::onRender() {
	Renderer::enableDepthTest();

	int divisions = 20;
	double size = 1.0;
	Vec3 right = screen.camera.getRightDirection();
	Vec3 up = screen.camera.getUpDirection();
	Vec3 forward = screen.camera.getForwardDirection();
	Vec3 center = castPositionToVec3(screen.camera.cframe.getPosition() + forward * distance);
	Shaders::maskShader->updateColor(Colors::RED);
	Shaders::maskShader->updateModel(GlobalCFrame().asMat4());
	for (int index = -divisions / 2; index <= divisions / 2; index++) {
		Vec3 dx = right * index * size;
		Vec3 dy = up * index * size;
		Vec3 mx = right * divisions / 2 * size;
		Vec3 my = up * divisions / 2 * size;
		deltaLine->resize(center - mx + dy, center + mx + dy);
		deltaLine->render();
		deltaLine->resize(center + dx - my, center + dx + my);
		deltaLine->render();
	}

	Shaders::maskShader->updateColor(Colors::RED);
	for (std::size_t index = 0; index < nodes.size(); index++) {
		Shaders::maskShader->updateModel(GlobalCFrame(castVec3ToPosition(nodes[index])).asMat4WithPreScale(DiagonalMat3({0.1, 0.1, 0.1})));
		MeshRegistry::get(MeshRegistry::sphere)->render();

		if (index > 0) {
			Shaders::maskShader->updateModel(GlobalCFrame().asMat4());
			deltaLine->resize(nodes[index], nodes[index - 1]);
			deltaLine->render();
		}
	}

	Renderer::disableDepthTest();
}

void PathTool::onUpdate() {

}

void PathTool::onEvent(Engine::Event& event) {
	using namespace Engine;

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MousePressEvent>(EVENT_BIND(PathTool::onMousePress));
	dispatcher.dispatch<MouseReleaseEvent>(EVENT_BIND(PathTool::onMouseRelease));
	dispatcher.dispatch<MouseDragEvent>(EVENT_BIND(PathTool::onMouseDrag));
}

Vec3 intersect(double distance) {
	Vec3 normal = -screen.camera.getForwardDirection();
	Position center = screen.camera.cframe.getPosition() - normal * distance;
	double t = (center - SelectionTool::ray.origin) * normal / (SelectionTool::ray.direction * normal);

	return castPositionToVec3(SelectionTool::ray.origin) + t * SelectionTool::ray.direction;
}

bool PathTool::onMousePress(Engine::MousePressEvent& event) {
	using namespace Engine;
	if (event.getButton() != Mouse::LEFT)
		return false;

	nodes.push_back(intersect(distance));

	active = true;
}

bool PathTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
	using namespace Engine;
	if (event.getButton() != Mouse::LEFT)
		return false;
	
}

bool PathTool::onMouseDrag(Engine::MouseDragEvent& event) {
	using namespace Engine;
	if (event.getButton() != Mouse::LEFT)
		return false;

	nodes.push_back(intersect(distance));

	active = false;
}

}
