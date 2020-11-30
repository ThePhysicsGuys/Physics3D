#include "core.h"
#include "constraintLayer.h"

#include <typeindex>

#include "../graphics/meshRegistry.h"

#include "worlds.h"
#include "../view/screen.h"
#include "../physics/physical.h"
#include "../physics/geometry/shapeClass.h"
#include "../physics/misc/shapeLibrary.h"

#include "../shader/shaders.h"
#include "../physics/misc/toString.h"

#include "../graphics/debug/guiDebug.h"
#include "../graphics/renderer.h"


#include "../physics/softconstraints/softConstraint.h"
#include "../physics/softconstraints/ballConstraint.h"


#include "../physics/constraints/sinusoidalPistonConstraint.h"
#include "../physics/constraints/motorConstraint.h"
#include "../physics/constraints/fixedConstraint.h"
#include "../graphics/meshRegistry.h"




namespace P3D::Application {

void ConstraintLayer::onInit(Engine::Registry64& registry) {
	Graphics::VisualShape prismShape(Library::createPrism(6, 0.5, 1.0));
	this->hexagon = Graphics::MeshRegistry::addMeshShape(prismShape);

}
void ConstraintLayer::onUpdate(Engine::Registry64& registry) {

}

static void renderObject(const VisualData& shape, const GlobalCFrame& cframe, const DiagonalMat3f& scale, const Comp::Material& material) {
	Shaders::basicShader.updateMaterial(material);
	Shaders::basicShader.updateTexture(false);
	Shaders::basicShader.updateModel(cframe, scale);
	Graphics::MeshRegistry::meshes[shape.id]->render();
}

static void renderConstraintLineBetween(Position p1, Position p2) {
	Vec3 delta = p2 - p1;

	if (lengthSquared(delta) < 0.0001) 
		return;

	Position center = p1 + delta/2;

	Rotation rot = Rotation::faceY(delta);

	renderObject(Graphics::MeshRegistry::box, GlobalCFrame(center, rot), DiagonalMat3f{0.2f, float(length(delta) / 2), 0.2f}, Comp::Material(Color(1.0f, 0.0f, 0.0f, 1.0f)));
	renderObject(Graphics::MeshRegistry::box, GlobalCFrame(p1, rot), DiagonalMat3f{0.25f, 0.25f, 0.25f}, Comp::Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
	renderObject(Graphics::MeshRegistry::box, GlobalCFrame(p2, rot), DiagonalMat3f{0.25f, 0.25f, 0.25f}, Comp::Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
}

static void renderBar(GlobalCFrame cframe, Vec3 delta, float thickness, Color color) {
	if (lengthSquared(delta) < 0.0001) 
		return;

	Rotation rotation = Rotation::faceZ(delta);

	renderObject(Graphics::MeshRegistry::box, cframe.localToGlobal(CFrame(delta/2, rotation)), DiagonalMat3f{thickness, thickness, float(length(delta) / 2)}, Comp::Material(color));
}

static void renderPiston(const ConstraintLayer* cl, const SinusoidalPistonConstraint* piston, const GlobalCFrame& start, const GlobalCFrame& end, int segments, float minThickness, float maxThickness) {
	Vec3 delta = start.globalToLocal(end).getPosition();
	Vec3 step = delta / segments;

	if (lengthSquared(delta) < 0.0001)
		return;

	Rotation rot = Rotation::faceZ(delta);

	for (int i = 0; i < segments; i++) {
		Vec3 center = step * (i + 0.5);

		float thickness = i / (segments - 1.0f) * (maxThickness - minThickness) + minThickness;

		Comp::Material mat = (i%2 == 0) ? Comp::Material(Color(1.0f, 0.8f, 0.1f, 1.0f)) : Comp::Material(Color(0.9f, 0.9f, 0.9f, 1.0f));

		renderObject(Graphics::MeshRegistry::cylinder, start.localToGlobal(CFrame(center, rot)), DiagonalMat3f{thickness, thickness, float(length(step) / 2)}, mat);
	}

	renderObject(Graphics::MeshRegistry::sphere, start, DiagonalMat3f::IDENTITY() * minThickness * 1.2f, Comp::Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
	renderObject(Graphics::MeshRegistry::sphere, end, DiagonalMat3f::IDENTITY() * maxThickness * 1.2f, Comp::Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
}

static void renderMotor(const ConstraintLayer* cl, const ConstantSpeedMotorConstraint* motor, const GlobalCFrame& start, const GlobalCFrame& end) {
	renderObject(cl->hexagon, start.localToGlobal(CFrame(Vec3(0, 0, 0.05))), DiagonalMat3f{0.2f, 0.2f, 0.1f}, Comp::Material(Color(1.0f, 1.0f, 0.0f, 1.0f)));
	renderObject(cl->hexagon, end.localToGlobal(CFrame(Vec3(0, 0, -0.05))), DiagonalMat3f{0.2f, 0.2f, 0.1f}, Comp::Material(Color(0.7f, 0.7f, 0.0f, 1.0f)));
}

static void renderConstraintBars(const ConstraintLayer* cl, const GlobalCFrame& cframeOfFirst, const GlobalCFrame& cframeOfSecond, const CFrame& attachOnFirst, const CFrame& attachOnSecond) {
	renderBar(cframeOfFirst, attachOnFirst.getPosition(), 0.02f, Color(1.0f, 0.0f, 0.0f, 1.0f));
	renderBar(cframeOfSecond, attachOnSecond.getPosition(), 0.02f, Color(1.0f, 0.0f, 0.0f, 1.0f));
}

static void renderBallConstraint(const ConstraintLayer* cl, const Physical* physA, const Physical* physB, const BallConstraint* bc, float innerBallThickness, float outerBallThickness) {
	renderConstraintBars(cl, physA->getCFrame(), physB->getCFrame(), CFrame(bc->attachA), CFrame(bc->attachB));
	renderObject(Graphics::MeshRegistry::sphere, physA->getCFrame().localToGlobal(bc->attachA), DiagonalMat3f::IDENTITY() * innerBallThickness, Comp::Material(Color(0.0f, 0.0f, 1.0f, 1.0f)));
	renderObject(Graphics::MeshRegistry::sphere, physB->getCFrame().localToGlobal(bc->attachB), DiagonalMat3f::IDENTITY() * outerBallThickness, Comp::Material(Color(0.0f, 0.0f, 1.0f, 0.7f)));
}

static void renderHardConstraint(const ConstraintLayer* cl, const ConnectedPhysical& conPhys) {
	GlobalCFrame cframeOfConPhys = conPhys.getCFrame();
	GlobalCFrame cframeOfParent = conPhys.parent->getCFrame();

	renderConstraintBars(cl, cframeOfConPhys, cframeOfParent, conPhys.connectionToParent.attachOnChild, conPhys.connectionToParent.attachOnParent);

	const HardConstraint* constraint = conPhys.connectionToParent.constraintWithParent.get();

	GlobalCFrame startOfConstraint = cframeOfConPhys.localToGlobal(conPhys.connectionToParent.attachOnChild);
	GlobalCFrame endOfConstraint = cframeOfParent.localToGlobal(conPhys.connectionToParent.attachOnParent);
	const auto& info(typeid(*constraint));
	if (info == typeid(SinusoidalPistonConstraint)) {
		renderPiston(cl, static_cast<const SinusoidalPistonConstraint*>(constraint), startOfConstraint, endOfConstraint, 3, 0.1f, 0.12f);
	} else if (info == typeid(ConstantSpeedMotorConstraint)) {
		renderMotor(cl, static_cast<const ConstantSpeedMotorConstraint*>(constraint), startOfConstraint, endOfConstraint);
	}
}

static void recurseRenderHardConstraints(const ConstraintLayer* cl, const Physical& physical) {
	for(const ConnectedPhysical& conPhys : physical.childPhysicals) {
		renderHardConstraint(cl, conPhys);
		recurseRenderHardConstraints(cl, conPhys);
	}
}

static void renderConstraint(const ConstraintLayer* cl, const PhysicalConstraint& constraint) {
	const auto& info(typeid(*constraint.constraint));
	if(info == typeid(BallConstraint)) {
		renderBallConstraint(cl, constraint.physA, constraint.physB, static_cast<const BallConstraint*>(constraint.constraint), 0.13f, 0.15f);
	}
}

static void renderSpringLink(const GlobalCFrame& start) {
	renderObject(Engine::MeshRegistry::sphere, start.localToGlobal(CFrame(Vec3(0, 0, 0.05))), DiagonalMat3f{ 0.2f, 0.2f, 0.1f }, Comp::Material(Color(1.0f, 1.0f, 0.0f, 1.0f)));
	
}

static void renderSoftLinkConstraint(const ConstraintLayer* cl, const SoftLink* link) {
	renderConstraintLineBetween(link->getGlobalPositionOfAttach2(), link->getGlobalPositionOfAttach1());
}

void ConstraintLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);
	PlayerWorld* world = screen->world;
	
	beginScene();
	Renderer::enableBlending();

	Shaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::maskShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	for (MotorizedPhysical* phys : world->iterPhysicals()) {
		recurseRenderHardConstraints(this, *phys);
	}

	for (const ConstraintGroup& g : world->constraints) {
		for(const PhysicalConstraint& constraint : g.constraints) {
			renderConstraint(this, constraint);
		}
	}
	
	for (const SoftLink* springLink : world->springLinks) {
		renderSoftLinkConstraint(this, springLink);
	}

	endScene();
}

void ConstraintLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}

void ConstraintLayer::onClose(Engine::Registry64& registry) {

}

};