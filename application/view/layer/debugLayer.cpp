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
	Position p = bounds.getCenter();
	renderBox(GlobalCFrame(p), diagonal.x, diagonal.y, diagonal.z, color);
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

void DebugLayer::init() {

	// Origin init
	float originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, Renderer::POINT);


	// Vector init
	vectorMesh = new VectorMesh(new float[128 * 9], 128);


	// Point init
	float* buf = new float[5 * 10];
	for (int i = 0; i < 5; i++) {
		buf[i * 10 + 0] = 0.5f + i;
		buf[i * 10 + 1] = 1.0f; // position
		buf[i * 10 + 2] = 0.5f;

		buf[i * 10 + 3] = 0.02f * i; // size

		buf[i * 10 + 4] = 1.0f;
		buf[i * 10 + 5] = 1.0f; // color 1 orange
		buf[i * 10 + 6] = 0.0f;

		buf[i * 10 + 7] = 0.0f;
		buf[i * 10 + 8] = 0.0f; // color 2 white
		buf[i * 10 + 9] = 0.0f;

	}

	pointMesh = new PointMesh(buf, 5);

}

void DebugLayer::update() {
	
}

void DebugLayer::render() {
	// Initialize vector log buffer
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	AddableBuffer<AppDebug::ColoredVector>& vecLog = AppDebug::getVectorBuffer();
	AddableBuffer<AppDebug::ColoredPoint>& pointLog = AppDebug::getPointBuffer();

	for (const Physical& p : screen->world->iterPhysicals()) {
		pointLog.add(AppDebug::ColoredPoint(p.getCenterOfMass(), Debug::CENTER_OF_MASS));
	}

	for (const ConstraintGroup& c : screen->world->constraints) {
		for (const BallConstraint& bc : c.ballConstraints) {
			vecLog.add(AppDebug::ColoredVector(bc.a->getCFrame().getPosition(), bc.a->getCFrame().localToRelative(bc.attachA), Debug::INFO_VEC));
			vecLog.add(AppDebug::ColoredVector(bc.b->getCFrame().getPosition(), bc.b->getCFrame().localToRelative(bc.attachB), Debug::INFO_VEC));
		}
	}

	if (screen->selectedPart != nullptr) {
		GlobalCFrame selectedCFrame(screen->selectedPart->cframe);
		for (const Vec3f& corner : screen->selectedPart->hitbox.iterVertices()) {
			vecLog.add(AppDebug::ColoredVector(selectedCFrame.localToGlobal(corner), screen->selectedPart->parent->getVelocityOfPoint(Vec3(selectedCFrame.localToRelative(corner))), Debug::VELOCITY));
		}

		if (colissionSpheresMode == SphereColissionRenderMode::SELECTED) {
			Physical& selectedPhys = *screen->selectedPart->parent;

			for (Part& part : selectedPhys) {
				Vec4f yellow = GUI::COLOR::YELLOW;
				yellow.w = 0.5;
				BoundingBox localBounds = screen->selectedPart->localBounds;
				renderBox(screen->selectedPart->cframe.localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.cframe.getPosition(), green);
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
				renderBox(part.cframe.localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.cframe.getPosition(), green);
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

	switch (renderColTree) {
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
	updateVecMesh(vectorMesh, vecLog.data, vecLog.size);
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
	Shaders::originShader.updateProjection(screen->camera.viewMatrix, Mat3f(screen->camera.cframe.rotation), screen->camera.projectionMatrix, screen->camera.orthoMatrix, screen->camera.cframe.position);
	originMesh->render();
}

void DebugLayer::close() {

}
