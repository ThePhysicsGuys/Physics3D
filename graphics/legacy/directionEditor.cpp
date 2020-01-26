#include "core.h"

#include "directionEditor.h"

#include "../physics/math/cframe.h"
#include "../physics/math/mathUtil.h"
#include "../physics/math/globalCFrame.h"

#include "gui.h"
#include "texture.h"
#include "renderUtils.h"
#include "path/path.h"
#include "buffers/frameBuffer.h"
#include "mesh/indexedMesh.h"
#include "mesh/primitive.h"
#include "meshLibrary.h"

DirectionEditor::DirectionEditor(double x, double y, double width, double height) : Component(x, y, width, height) {
	viewPosition = Position(0.0, 0.0, -3.0);
	viewMatrix = CFrameToMat4(GlobalCFrame(viewPosition));
	modelMatrix = Matrix<float, 4, 4>::IDENTITY(); //.scale(4, 0.5, 4);
	rspeed = 10;
}

void DirectionEditor::render() {
	if (visible) {

		Color blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		// Draw onto gui framebuffer
		GUI::guiFrameBuffer->bind();
		Renderer::enableDepthTest();
		Renderer::clearColor();
		Renderer::clearDepth();

		Library::vector->render();

		GUI::guiFrameBuffer->unbind();
		Renderer::disableDepthTest();

		Path::rect(position, dimension, 0.0f, COLOR::blend(COLOR::BACK, blendColor));

		Vec2 contentPosition = position + Vec2(GUI::padding, -GUI::padding);
		Vec2 contentDimension = dimension - Vec2(GUI::padding) * 2;
		Path::rectUV(GUI::guiFrameBuffer->texture->getID(), contentPosition, contentDimension, Vec2f(0), Vec2f(1), blendColor);
	}
}

void DirectionEditor::rotate(double dalpha, double dbeta, double dgamma) {
	modelMatrix = Mat4f(Matrix<float, 3, 3>(rotX(float(dalpha)) * Mat3f(modelMatrix.getSubMatrix<3, 3>(0,0)) * rotZ(float(-dbeta))), 1.0f);
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
