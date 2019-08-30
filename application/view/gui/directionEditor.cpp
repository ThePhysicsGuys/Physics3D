#include "directionEditor.h"

#include "../engine/math/vec.h"
#include "../engine/math/cframe.h"
#include "../engine/math/mathUtil.h"

#include "gui.h"
#include "path.h"
#include "../texture.h"
#include "../material.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"
#include "../screen.h"

#include "../buffers/frameBuffer.h"

#include "../mesh/indexedMesh.h"
#include "../mesh/primitive.h"

#include "../../meshLibrary.h"

DirectionEditor::DirectionEditor(double x, double y, double width, double height) : Component(x, y, width, height) {
	viewPosition = Position(0.0, 0.0, -3.0);
	viewMatrix = CFrameToMat4(GlobalCFrame(viewPosition));
	modelMatrix = Mat4f(); //.scale(4, 0.5, 4);
	rspeed = 10;
}

void DirectionEditor::render() {
	if (visible) {

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		// Draw onto gui framebuffer
		GUI::guiFrameBuffer->bind();
		Renderer::enableDepthTest();
		Renderer::clearColor();
		Renderer::clearDepth();

		Shaders::basicShader.updateMaterial(Material(GUI::COLOR::blend(GUI::COLOR::G, blendColor)));
		Shaders::basicShader.updateModel(modelMatrix);
		Shaders::basicShader.updateProjection(viewMatrix, GUI::screen->camera.projectionMatrix, viewPosition);
		Library::vector->render();

		GUI::guiFrameBuffer->unbind();
		Renderer::disableDepthTest();

		Path::rect(position, dimension, 0.0f, GUI::COLOR::blend(GUI::COLOR::BACK, blendColor));

		Vec2 contentPosition = position + Vec2(GUI::padding, -GUI::padding);
		Vec2 contentDimension = dimension - Vec2(GUI::padding) * 2;
		Path::rectUV(GUI::guiFrameBuffer->texture->getID(), contentPosition, contentDimension, Vec2f(0), Vec2f(1), blendColor);
	}
}

void DirectionEditor::rotate(double dalpha, double dbeta, double dgamma) {
	modelMatrix = rotX(float(dalpha)) * modelMatrix.getRotation() * rotZ(float(-dbeta));
	(*action)(this);
}

Vec2 DirectionEditor::resize() {
	return dimension;
}

void DirectionEditor::drag(Vec2 newPoint, Vec2 oldPoint) {
	if (disabled)
		return;

	Vec2 dmxy = newPoint - oldPoint;
	rotate(dmxy.y * rspeed, dmxy.x * rspeed, 0);
}
