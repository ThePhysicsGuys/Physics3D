#include "core.h"
#include "constraintLayer.h"

#include "worlds.h"
#include "../view/screen.h"
#include "../shader/shaders.h"
#include "../graphics/renderer.h"
#include "../graphics/meshRegistry.h"
#include <Physics3D/constraints/ballConstraint.h>
#include <Physics3D/constraints/hingeConstraint.h>
#include <Physics3D/constraints/barConstraint.h>
#include <Physics3D/hardconstraints/sinusoidalPistonConstraint.h>
#include <Physics3D/hardconstraints/motorConstraint.h>
#include <Physics3D/misc/toString.h>
#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/physical.h>

#include <typeindex>

namespace P3D::Application {

void ConstraintLayer::onInit(Engine::Registry64& registry) {
	Graphics::VisualShape prismShape(ShapeLibrary::createPrism(6, 0.5, 1.0));
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

static Color constraintBarColor = Colors::RGB_R;
constexpr static float constraintBarThickness = 0.02f;
constexpr static float innerBallThickness = 0.06f;
constexpr static float outerBallThickness = 0.07f;
static Comp::Material innerConstraintColor = Comp::Material(Color(0.0f, 0.0f, 1.0f, 1.0f));
static Comp::Material outerConstraintColor = Comp::Material(Color(0.0f, 0.0f, 1.0f, 0.7f));


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

static void renderBallConstraint(const ConstraintLayer* cl, const GlobalCFrame& cframeA, const GlobalCFrame& cframeB, const BallConstraint* bc) {
	renderBar(cframeA, bc->attachA, constraintBarThickness, constraintBarColor);
	renderBar(cframeB, bc->attachB, constraintBarThickness, constraintBarColor);

	renderObject(Graphics::MeshRegistry::sphere, cframeA.localToGlobal(CFrame(bc->attachA)), DiagonalMat3f::IDENTITY() * innerBallThickness, innerConstraintColor);
	renderObject(Graphics::MeshRegistry::sphere, cframeB.localToGlobal(CFrame(bc->attachB)), DiagonalMat3f::IDENTITY() * outerBallThickness, outerConstraintColor);
}

static void renderHingeConstraint(const ConstraintLayer* cl, const GlobalCFrame& cframeA, const GlobalCFrame& cframeB, const HingeConstraint* hc) {
	renderBar(cframeA, hc->attachA, constraintBarThickness, constraintBarColor);
	renderBar(cframeB, hc->attachB, constraintBarThickness, constraintBarColor);

	CFrame atA(hc->attachA, Rotation::faceZ(hc->axisA));
	CFrame atB(hc->attachB, Rotation::faceZ(hc->axisB));
	renderObject(Graphics::MeshRegistry::cylinder, cframeA.localToGlobal(atA), DiagonalMat3f::IDENTITY() * innerBallThickness, innerConstraintColor);
	renderObject(Graphics::MeshRegistry::cylinder, cframeB.localToGlobal(atB), DiagonalMat3f::IDENTITY() * outerBallThickness, outerConstraintColor);
}

static void renderBarConstraint(const ConstraintLayer* cl, const GlobalCFrame& cframeA, const GlobalCFrame& cframeB, const BarConstraint* bc) {
	renderBar(cframeA, bc->attachA, constraintBarThickness, constraintBarColor);
	renderBar(cframeB, bc->attachB, constraintBarThickness, constraintBarColor);
	Position globalA = cframeA.localToGlobal(bc->attachA);
	Position globalB = cframeB.localToGlobal(bc->attachB);

	Position barCenter = avg(globalA, globalB);
	Vec3 bar;
	if(globalA != globalB) {
		bar = withLength(Vec3(globalB - globalA), bc->barLength);
	} else {
		bar = Vec3(bc->barLength, 0, 0);
	}

	renderBar(barCenter - bar/2, bar, constraintBarThickness, constraintBarColor);

	renderObject(Graphics::MeshRegistry::sphere, cframeA.localToGlobal(CFrame(bc->attachA)), DiagonalMat3f::IDENTITY() * innerBallThickness, innerConstraintColor);
	renderObject(Graphics::MeshRegistry::sphere, cframeB.localToGlobal(CFrame(bc->attachB)), DiagonalMat3f::IDENTITY() * outerBallThickness, outerConstraintColor);

	renderObject(Graphics::MeshRegistry::sphere, barCenter - bar / 2, DiagonalMat3f::IDENTITY() * innerBallThickness, innerConstraintColor);
	renderObject(Graphics::MeshRegistry::sphere, barCenter + bar / 2, DiagonalMat3f::IDENTITY() * outerBallThickness, outerConstraintColor);
}

static void renderHardConstraint(const ConstraintLayer* cl, const ConnectedPhysical& conPhys) {
	GlobalCFrame cframeOfConPhys = conPhys.getCFrame();
	GlobalCFrame cframeOfParent = conPhys.parent->getCFrame();


	renderBar(cframeOfConPhys, conPhys.connectionToParent.attachOnChild.position, constraintBarThickness, constraintBarColor);
	renderBar(cframeOfParent, conPhys.connectionToParent.attachOnParent.position, constraintBarThickness, constraintBarColor);

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
	GlobalCFrame cfA = constraint.physA->getCFrame();
	GlobalCFrame cfB = constraint.physB->getCFrame();
	if(info == typeid(BallConstraint)) {
		renderBallConstraint(cl, cfA, cfB, static_cast<const BallConstraint*>(constraint.constraint));
	} else if(info == typeid(HingeConstraint)) {
		renderHingeConstraint(cl, cfA, cfB, static_cast<const HingeConstraint*>(constraint.constraint));
	} else if(info == typeid(BarConstraint)) {
		renderBarConstraint(cl, cfA, cfB, static_cast<const BarConstraint*>(constraint.constraint));
	}
}

static void renderSpringLink(const GlobalCFrame& start) {
	renderObject(MeshRegistry::sphere, start.localToGlobal(CFrame(Vec3(0, 0, 0.05))), DiagonalMat3f{ 0.2f, 0.2f, 0.1f }, Comp::Material(Color(1.0f, 1.0f, 0.0f, 1.0f)));
	
}

/*static void renderSoftLinkConstraint(const ConstraintLayer* cl, const SoftLink* link) {
	renderConstraintLineBetween(link->getGlobalPositionOfAttach2(), link->getGlobalPositionOfAttach1());
}*/

void ConstraintLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);
	PlayerWorld* world = screen->world;
	
	beginScene();
	Renderer::enableBlending();

	Shaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::maskShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	world->syncReadOnlyOperation([&]() {
		for(MotorizedPhysical* phys : world->physicals) {
			recurseRenderHardConstraints(this, *phys);
		}

		for(const ConstraintGroup& g : world->constraints) {
			for(const PhysicalConstraint& constraint : g.constraints) {
				renderConstraint(this, constraint);
			}
		}

		/*for (const SoftLink* springLink : world->springLinks) {
			renderSoftLinkConstraint(this, springLink);
		}*/
	});

	endScene();
}

void ConstraintLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}

void ConstraintLayer::onClose(Engine::Registry64& registry) {

}

};