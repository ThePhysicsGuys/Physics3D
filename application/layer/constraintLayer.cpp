#include "core.h"
#include "constraintLayer.h"

#include <typeindex>

#include "../engine/meshRegistry.h"

#include "worlds.h"
#include "../view/screen.h"
#include "../physics/physical.h"
#include "../physics/geometry/shapeClass.h"
#include "../physics/misc/shapeLibrary.h"

#include "../shader/shaders.h"
#include "../physics/misc/toString.h"

#include "../graphics/debug/debug.h"

#include "../physics/constraints/sinusoidalPistonConstraint.h"
#include "../physics/constraints/motorConstraint.h"
#include "../physics/constraints/fixedConstraint.h"

namespace Application {
void ConstraintLayer::onAttach() {
	
}
void ConstraintLayer::onDetach() {

}

void ConstraintLayer::onInit() {
	this->hexagon = Engine::MeshRegistry::addMeshShape(VisualShape(Library::createPrism(6, 0.5, 1.0)));

}
void ConstraintLayer::onUpdate() {

}

static void renderObj(const VisualData& shape, const GlobalCFrame& cf, const DiagonalMat3f& scale, const Material& material) {
	ApplicationShaders::basicShader.updateMaterial(material);
	ApplicationShaders::basicShader.updateIncludeNormalsAndUVs(false, false);
	ApplicationShaders::basicShader.updateModel(cf, scale);
	shape.render(0x1B02); // GL_FILL
}

static void renderConstraintLineBetween(Position p1, Position p2) {
	Vec3 delta = p2 - p1;

	if(lengthSquared(delta) < 0.0001) return;

	Position center = p1 + delta/2;

	Rotation rot = Rotation::faceY(delta);

	renderObj(Engine::MeshRegistry::box, GlobalCFrame(center, rot), DiagonalMat3f{0.2f, float(length(delta) / 2), 0.2f}, Material(Color(1.0f, 0.0f, 0.0f, 1.0f)));
	renderObj(Engine::MeshRegistry::box, GlobalCFrame(p1, rot), DiagonalMat3f{0.25f, 0.25f, 0.25f}, Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
	renderObj(Engine::MeshRegistry::box, GlobalCFrame(p2, rot), DiagonalMat3f{0.25f, 0.25f, 0.25f}, Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
}

static void renderBar(GlobalCFrame cf1, Vec3 delta, float thickness, Color color) {
	if(lengthSquared(delta) < 0.0001) return;

	Rotation rot = Rotation::faceZ(delta);

	renderObj(Engine::MeshRegistry::box, cf1.localToGlobal(CFrame(delta/2, rot)), DiagonalMat3f{thickness, thickness, float(length(delta) / 2)}, Material(color));
}

static void renderPiston(const ConstraintLayer* cl, const SinusoidalPistonConstraint* piston, const GlobalCFrame& start, const GlobalCFrame& end, int segments, float minThickness, float maxThickness) {
	Vec3 delta = start.globalToLocal(end).getPosition();
	Vec3 step = delta / segments;

	if(lengthSquared(delta) < 0.0001) return;

	Rotation rot = Rotation::faceZ(delta);

	for(int i = 0; i < segments; i++) {
		Vec3 center = step * (i + 0.5);

		float thickness = i / (segments - 1.0f) * (maxThickness - minThickness) + minThickness;

		Material mat = (i%2 == 0) ? Material(Color(1.0f, 0.8f, 0.1f, 1.0f), Color(1.0f), Color(1.0f), 0.0f) : Material(Color(0.9f, 0.9f, 0.9f, 1.0f), Color(1.0f), Color(1.0f), 1.0f);

		renderObj(Engine::MeshRegistry::cylinder, start.localToGlobal(CFrame(center, rot)), DiagonalMat3f{thickness, thickness, float(length(step) / 2)}, mat);
	}

	renderObj(Engine::MeshRegistry::sphere, start, DiagonalMat3f::IDENTITY() * minThickness * 1.2f, Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
	renderObj(Engine::MeshRegistry::sphere, end, DiagonalMat3f::IDENTITY() * maxThickness * 1.2f, Material(Color(0.0f, 1.0f, 0.0f, 1.0f)));
}

static void renderMotor(const ConstraintLayer* cl, const MotorConstraint* motor, const GlobalCFrame& start, const GlobalCFrame& end) {
	NormalizedVec3 motorDir = motor->getDirection();
	if(sumElements(motorDir) < 0) motorDir = -motorDir;
	Rotation motorRot = Rotation::faceY(motorDir);
	renderObj(cl->hexagon, start.localToGlobal(CFrame(motorDir * 0.05, motorRot)), DiagonalMat3f{0.2f, 0.1f, 0.2f}, Material(Color(1.0f, 1.0f, 0.0f, 1.0f)));
	renderObj(cl->hexagon, end.localToGlobal(CFrame(-motorDir * 0.05, motorRot)), DiagonalMat3f{0.2f, 0.1f, 0.2f}, Material(Color(0.7f, 0.7f, 0.0f, 1.0f)));
}

static void renderConstraintBars(const ConstraintLayer* cl, const GlobalCFrame& cframeOfFirst, const GlobalCFrame& cframeOfSecond, const CFrame& attachOnFirst, const CFrame& attachOnSecond) {
	renderBar(cframeOfFirst, attachOnFirst.getPosition(), 0.02f, Color(1.0f, 0.0f, 0.0f, 1.0f));
	renderBar(cframeOfSecond, attachOnSecond.getPosition(), 0.02f, Color(1.0f, 0.0f, 0.0f, 1.0f));
}

static void renderBallConstraint(const ConstraintLayer* cl, const GlobalCFrame& cframeOfFirst, const GlobalCFrame& cframeOfSecond, const CFrame& attachOnFirst, const CFrame& attachOnSecond, float innerBallThickness, float outerBallThickness) {
	renderConstraintBars(cl, cframeOfFirst, cframeOfSecond, attachOnFirst, attachOnSecond);

	renderObj(Engine::MeshRegistry::sphere, cframeOfFirst.localToGlobal(attachOnFirst.getPosition()), DiagonalMat3f::IDENTITY() * innerBallThickness, Material(Color(0.0f, 0.0f, 1.0f, 1.0f)));
	renderObj(Engine::MeshRegistry::sphere, cframeOfSecond.localToGlobal(attachOnSecond.getPosition()), DiagonalMat3f::IDENTITY() * outerBallThickness, Material(Color(0.0f, 0.0f, 1.0f, 0.7f)));
}

static void renderHardConstraint(const ConstraintLayer* cl, const ConnectedPhysical& conPhys) {
	GlobalCFrame cframeOfConPhys = conPhys.getCFrame();
	GlobalCFrame cframeOfParent = conPhys.parent->getCFrame();

	renderConstraintBars(cl, cframeOfConPhys, cframeOfParent, conPhys.connectionToParent.attachOnChild, conPhys.connectionToParent.attachOnParent);

	const HardConstraint* constraint = conPhys.connectionToParent.constraintWithParent.get();

	GlobalCFrame startOfConstraint = cframeOfConPhys.localToGlobal(conPhys.connectionToParent.attachOnChild);
	GlobalCFrame endOfConstraint = cframeOfParent.localToGlobal(conPhys.connectionToParent.attachOnParent);
	auto& info(typeid(*constraint));
	if(info == typeid(SinusoidalPistonConstraint)) {
		renderPiston(cl, static_cast<const SinusoidalPistonConstraint*>(constraint), startOfConstraint, endOfConstraint, 3, 0.1f, 0.12f);
	} else if(info == typeid(MotorConstraint)) {
		renderMotor(cl, static_cast<const MotorConstraint*>(constraint), startOfConstraint, endOfConstraint);
	}
}

static void recurseRenderHardConstraints(const ConstraintLayer* cl, const Physical& physical) {
	for(const ConnectedPhysical& conPhys : physical.childPhysicals) {
		renderHardConstraint(cl, conPhys);

		recurseRenderHardConstraints(cl, conPhys);
	}
}

void ConstraintLayer::onRender() {
	PlayerWorld* world = screen->world;
	ApplicationShaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	ApplicationShaders::maskShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);

	for(MotorizedPhysical* phys : world->iterPhysicals()) {
		recurseRenderHardConstraints(this, *phys);
	}

	for(const ConstraintGroup& g : world->constraints) {
		for(const BallConstraint& constraint : g.ballConstraints) {
			renderBallConstraint(this, constraint.a->getCFrame(), constraint.b->getCFrame(), CFrame(constraint.attachA), CFrame(constraint.attachB), 0.13f, 0.15f);

		}
	}
}
void ConstraintLayer::onClose() {

}

void ConstraintLayer::onEvent(Event& event) {

}
};