#include "core.h"

#include "debugLayer.h"

#include "../graphics/renderer.h"
#include "../graphics/mesh/arrayMesh.h"
#include "../graphics/mesh/pointMesh.h"
#include "../graphics/mesh/vectorMesh.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/debug/visualDebug.h"

#include <Physics3D/threading/upgradeableMutex.h>
#include <Physics3D/boundstree/boundsTree.h>
#include <Physics3D/geometry/polyhedron.h>
#include <Physics3D/math/bounds.h>
#include <Physics3D/physical.h>
#include <Physics3D/layer.h>

#include "worlds.h"
#include "view/screen.h"
#include "ecs/material.h"
#include "graphics/meshRegistry.h"
#include "shader/shaders.h"

namespace P3D::Application {

Vec4f colors[] {
	Colors::BLUE,
	Colors::GREEN,
	Colors::YELLOW,
	Colors::ORANGE,
	Colors::RED,
	Colors::PURPLE
};

void renderSphere(double radius, const Position& position, const Color& color) {
	Shaders::basicShader->updateMaterial(Comp::Material(color));
	Shaders::basicShader->updateModel(join(Mat3f::IDENTITY() * float(radius), castPositionToVec3f(position), Vec3f(0.0f,0.0f,0.0f),1.0f));

	Graphics::MeshRegistry::get(Graphics::MeshRegistry::sphere)->render();
}

void renderBox(const GlobalCFrame& cframe, double width, double height, double depth, const Color& color) {
	Shaders::basicShader->updateMaterial(Comp::Material(color));
	Shaders::basicShader->updateModel(Mat4f(cframe.asMat4WithPreScale(DiagonalMat3{width, height, depth})));

	Graphics::MeshRegistry::get(Graphics::MeshRegistry::box)->render();
}

void renderBounds(const Bounds& bounds, const Color& color) {
	Vec3Fix diagonal = bounds.getDiagonal();
	Position position = bounds.getCenter();
	renderBox(GlobalCFrame(position), static_cast<double>(diagonal.x), static_cast<double>(diagonal.y), static_cast<double>(diagonal.z), color);
}

static Color getCyclingColor(int depth) {
	Color color = colors[depth % 6];
	color.a = 0.3f;
	return color;
}

static void renderBoundsForDepth(const Bounds& bounds, int depth) {
	renderBounds(bounds.expanded((10 - depth) * 0.002), getCyclingColor(depth));
}

static void recursiveRenderColTree(const P3D::TreeTrunk& curTrunk, int curTrunkSize, int depth) {
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];

		if(subNode.isTrunkNode()) {
			recursiveRenderColTree(subNode.asTrunk(), subNode.getTrunkSize(), depth + 1);
		}

		renderBoundsForDepth(curTrunk.getBoundsOfSubNode(i), depth);
	}
}

static bool recursiveColTreeForOneObject(const TreeTrunk& curTrunk, int curTrunkSize, const Part* part, const Bounds& bounds, int depth) {
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];

		if(subNode.isTrunkNode()) {
			if(recursiveColTreeForOneObject(subNode.asTrunk(), subNode.getTrunkSize(), part, bounds, depth + 1)) {
				renderBoundsForDepth(curTrunk.getBoundsOfSubNode(i), depth);
				return true;
			}
		} else {
			if(subNode.asObject() == part) {
				return true;
			}
		}
	}
	return false;
}

static void renderTree(const BoundsTree<Part>& tree) {
	auto baseTrunk = tree.getPrototype().getBaseTrunk();
	recursiveRenderColTree(baseTrunk.first, baseTrunk.second, 0);
}
static void renderTreeForOneObject(const BoundsTree<Part>& tree, const Part& part) {
	auto baseTrunk = tree.getPrototype().getBaseTrunk();
	recursiveColTreeForOneObject(baseTrunk.first, baseTrunk.second, &part, part.getBounds(), 0);
}



void DebugLayer::onInit(Engine::Registry64& registry) {

	// Origin init
	originMesh = new ArrayMesh(nullptr, 1, 3, Renderer::POINT);

	// Vector init
	vectorMesh = new VectorMesh(nullptr, 0);

	// Point init
	pointMesh = new PointMesh(nullptr, 0);

}

void DebugLayer::onUpdate(Engine::Registry64& registry) {

}

void DebugLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}

void DebugLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;
	using namespace VisualDebug;
	using namespace AppDebug;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();
	enableBlending();

	graphicsMeasure.mark(GraphicsProcess::VECTORS);


	// Initialize vector log buffer
	AddableBuffer<ColoredVector>& vecLog = getVectorBuffer();
	AddableBuffer<ColoredPoint>& pointLog = getPointBuffer();

	{
		std::shared_lock<UpgradeableMutex> worldReadLock(*screen->worldMutex);
		for(const MotorizedPhysical* physical : screen->world->physicals) {
			Position com = physical->getCenterOfMass();
			pointLog.add(ColoredPoint(com, Debug::CENTER_OF_MASS));
		}

		Screen* screen = static_cast<Screen*>(this->ptr);
		if(screen->selectedPart != nullptr) {
			const GlobalCFrame& selectedCFrame = screen->selectedPart->getCFrame();
			Motion partMotion = screen->selectedPart->getMotion();
			Polyhedron asPoly = screen->selectedPart->hitbox.asPolyhedron();
			for(const Vec3f& corner : asPoly.iterVertices()) {
				vecLog.add(ColoredVector(selectedCFrame.localToGlobal(corner), partMotion.getVelocityOfPoint(selectedCFrame.localToRelative(corner)), Debug::VELOCITY));
			}

			if(colissionSpheresMode == SphereColissionRenderMode::SELECTED) {
				Physical& selectedPhys = *screen->selectedPart->parent;

				for(Part& part : selectedPhys.rigidBody) {
					Color yellow = Colors::YELLOW;
					yellow.a = 0.5;
					BoundingBox localBounds = screen->selectedPart->getLocalBounds();
					renderBox(screen->selectedPart->getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

					Color green = Colors::GREEN;
					green.a = 0.5;
					renderSphere(part.maxRadius, part.getPosition(), green);
				}
			}
		}

		if(colissionSpheresMode == SphereColissionRenderMode::ALL) {
			for(MotorizedPhysical* phys : screen->world->physicals) {
				for(Part& part : phys->rigidBody) {
					Color yellow = Colors::YELLOW;
					yellow.a = 0.5;
					BoundingBox localBounds = part.getLocalBounds();
					renderBox(part.getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

					Color green = Colors::GREEN;
					green.a = 0.5;
					renderSphere(part.maxRadius, part.getPosition(), green);
				}
			}
		}

		if(colTreeRenderMode == -2) {
			if(screen->selectedPart != nullptr) {
				renderTreeForOneObject(screen->selectedPart->layer->tree, *screen->selectedPart);
			}
		} else if(colTreeRenderMode >= 0) {
			renderTree(getLayerByID(screen->world->layers, colTreeRenderMode)->tree);
		}
		worldReadLock.unlock(); // unlock_shared
	}

	disableDepthTest();

	// Update debug meshes
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	updateVectorMesh(vectorMesh, vecLog.data, vecLog.size);
	updatePointMesh(pointMesh, pointLog.data, pointLog.size);

	// Render vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	Shaders::vectorShader->updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	vectorMesh->render();

	// Render point mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	Shaders::pointShader->updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	pointMesh->render();

	// Render origin mesh
	graphicsMeasure.mark(GraphicsProcess::ORIGIN);
	Shaders::originShader->updateProjection(screen->camera.viewMatrix, screen->camera.getViewRotation(), screen->camera.projectionMatrix, screen->camera.orthoMatrix, screen->camera.cframe.position);
	originMesh->render();

	endScene();
}

void DebugLayer::onClose(Engine::Registry64& registry) {

}

};