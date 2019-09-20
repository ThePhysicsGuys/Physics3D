#include "core.h"

#include "debugLayer.h"

#include "../mesh/arrayMesh.h"
#include "../mesh/pointMesh.h"
#include "../mesh/vectorMesh.h"
#include "../mesh/indexedMesh.h"

#include "../engine/datastructures/boundsTree.h"
#include "../engine/math/mathUtil.h"
#include "../engine/math/bounds.h"
#include "../engine/physical.h"

#include "../screen.h"
#include "../material.h"
#include "../renderUtils.h"
#include "../shaderProgram.h"

#include "../debug/profilerUI.h"
#include "../debug/visualDebug.h"

#include "../gui/gui.h"

#include "../../worlds.h"
#include "../../meshLibrary.h"

Vec4f colors[] {
	GUI::COLOR::BLUE,
	GUI::COLOR::GREEN,
	GUI::COLOR::YELLOW,
	GUI::COLOR::ORANGE,
	GUI::COLOR::RED,
	GUI::COLOR::PURPLE
};

void renderSphere(double radius, Position position, Vec4f color) {
	Shaders::basicShader.updateMaterial(Material(color));
	Shaders::basicShader.updateModel(CFrameToMat4(GlobalCFrame(position, DiagonalMat3(1, 1, 1) * radius)));

	Library::sphere->render();
}

void renderBox(const GlobalCFrame& cframe, double width, double height, double depth, Vec4f color) {
	Shaders::basicShader.updateMaterial(Material(color));
	Shaders::basicShader.updateModel(CFrameToMat4(GlobalCFrame(cframe.getPosition(), cframe.getRotation() * DiagonalMat3(width, height, depth))));

	Library::cube->render();
}

void renderBounds(const Bounds& bounds, const Vec4f& color) {
	Vec3Fix diagonal = bounds.getDiagonal();
	Position position = bounds.getCenter();
	renderBox(GlobalCFrame(position), diagonal.x, diagonal.y, diagonal.z, color);
}

void recursiveRenderColTree(const TreeNode& node, int depth) {
	if (node.isLeafNode()) {
		//renderBounds(node.bounds, GUI::COLOR::AQUA);
	} else {
		for (const TreeNode& node : node) {
			recursiveRenderColTree(node, depth + 1);
		}
	}

	Vec4f color = colors[depth % 6];
	color.w = 0.3f;

	renderBounds(node.bounds.expanded((5 - depth) * 0.002), color);
}

bool recursiveColTreeForOneObject(const TreeNode& node, const Physical* physical, const Bounds& bounds) {
	if (node.isLeafNode()) {
		if (node.object == physical)
			return true;
		/*for (const BoundedPhysical& p : *node.physicals) {
			if (p.object == physical) {
				return true;
			}
		}*/
	} else {
		//if (!intersects(node.bounds, bounds)) return false;
		for (const TreeNode& subNode : node) {
			if (recursiveColTreeForOneObject(subNode, physical, bounds)) {
				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.3f;

				renderBounds(node.bounds, green);
				return true;
			}
		}
	}
	return false;
}

DebugLayer::DebugLayer() {

}

DebugLayer::DebugLayer(Screen* screen, char flags) : Layer("Debug layer", screen, flags) {

}

void DebugLayer::onInit() {

	// Origin init
	float originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, Renderer::POINT);


	// Vector init
	vectorMesh = new VectorMesh(new float[128 * 9], 128);


	// Point init
	float* buffer = new float[5 * 10];
	for (int i = 0; i < 5; i++) {
		buffer[i * 10 + 0] = 0.5f + i;
		buffer[i * 10 + 1] = 1.0f; // position
		buffer[i * 10 + 2] = 0.5f;

		buffer[i * 10 + 3] = 0.02f * i; // size

		buffer[i * 10 + 4] = 1.0f;
		buffer[i * 10 + 5] = 1.0f; // color 1 orange
		buffer[i * 10 + 6] = 0.0f;

		buffer[i * 10 + 7] = 0.0f;
		buffer[i * 10 + 8] = 0.0f; // color 2 white
		buffer[i * 10 + 9] = 0.0f;

	}

	pointMesh = new PointMesh(buffer, 5);

}


void DebugLayer::onUpdate() {
	
}

void DebugLayer::onEvent(Event& event) {

}

void DebugLayer::onRender() {
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	
	using namespace Debug;
	using namespace AppDebug;

	// Initialize vector log buffer
	AddableBuffer<ColoredVector>& vecLog = getVectorBuffer();
	AddableBuffer<ColoredPoint>& pointLog = getPointBuffer();

	for (const Physical& physical : screen->world->iterPhysicals()) {
		pointLog.add(ColoredPoint(physical.getCenterOfMass(), CENTER_OF_MASS));
	}

	for (const ConstraintGroup& constraintGroup : screen->world->constraints) {
		for (const BallConstraint& ballConstraint : constraintGroup.ballConstraints) {
			vecLog.add(ColoredVector(ballConstraint.a->getCFrame().getPosition(), ballConstraint.a->getCFrame().localToRelative(ballConstraint.attachA), INFO_VEC));
			vecLog.add(ColoredVector(ballConstraint.b->getCFrame().getPosition(), ballConstraint.b->getCFrame().localToRelative(ballConstraint.attachB), INFO_VEC));
		}
	}

	if (screen->selectedPart != nullptr) {
		const GlobalCFrame& selectedCFrame = screen->selectedPart->getCFrame();
		for (const Vec3f& corner : screen->selectedPart->hitbox.iterVertices()) {
			vecLog.add(ColoredVector(selectedCFrame.localToGlobal(corner), screen->selectedPart->parent->getVelocityOfPoint(Vec3(selectedCFrame.localToRelative(corner))), VELOCITY));
		}

		if (colissionSpheresMode == SphereColissionRenderMode::SELECTED) {
			Physical& selectedPhys = *screen->selectedPart->parent;

			for (Part& part : selectedPhys) {
				Vec4f yellow = GUI::COLOR::YELLOW;
				yellow.w = 0.5;
				BoundingBox localBounds = screen->selectedPart->localBounds;
				renderBox(screen->selectedPart->getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.getPosition(), green);
			}

			Vec4f red = GUI::COLOR::RED;
			red.w = 0.5;
			BoundingBox localBounds = selectedPhys.localBounds;
			renderBox(selectedPhys.getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), red);

			Vec4f blue = GUI::COLOR::BLUE;
			blue.w = 0.5;
			renderSphere(selectedPhys.circumscribingSphere.radius * 2, selectedPhys.circumscribingSphere.origin, blue);
		}
	}

	if (colissionSpheresMode == SphereColissionRenderMode::ALL) {
		for (Physical& phys : screen->world->iterPhysicals()) {
			for (Part& part : phys) {
				Vec4f yellow = GUI::COLOR::YELLOW;
				yellow.w = 0.5;
				BoundingBox localBounds = part.localBounds;
				renderBox(part.getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.getPosition(), green);
			}
		}

		for (Physical& phys : screen->world->iterPhysicals()) {
			Vec4f red = GUI::COLOR::RED;
			red.w = 0.5;
			BoundingBox localBounds = phys.localBounds;
			renderBox(phys.getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), red);

			Vec4f blue = GUI::COLOR::BLUE;
			blue.w = 0.5;
			renderSphere(phys.circumscribingSphere.radius * 2, phys.circumscribingSphere.origin, blue);
		}
	}

	switch (colTreeRenderMode) {
		case ColTreeRenderMode::ALL:
			recursiveRenderColTree(screen->world->objectTree.rootNode, 0);
			break;
		case ColTreeRenderMode::SELECTED:
			if (screen->selectedPart != nullptr)
				recursiveColTreeForOneObject(screen->world->objectTree.rootNode, screen->selectedPart->parent, screen->selectedPart->parent->getStrictBounds());
			break;
	}

	Renderer::disableDepthTest();

	// Update debug meshes
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	updateVectorMesh(vectorMesh, vecLog.data, vecLog.size);
	updatePointMesh(pointMesh, pointLog.data, pointLog.size);


	// Render vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	Shaders::vectorShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	vectorMesh->render();


	// Render point mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	Shaders::pointShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	pointMesh->render();

	Renderer::enableDepthTest();

	// Render origin mesh
	graphicsMeasure.mark(GraphicsProcess::ORIGIN);
	Shaders::originShader.updateProjection(screen->camera.viewMatrix, Matrix<float, 4, 4>(Matrix<double, 3, 3>(screen->camera.cframe.rotation), 1.0f), screen->camera.projectionMatrix, screen->camera.orthoMatrix, screen->camera.cframe.position);
	originMesh->render();
}

void DebugLayer::onClose() {

}
