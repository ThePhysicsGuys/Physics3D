#include "directionEditor.h"

#include "../engine/math/tempUnsafeCasts.h"

#include "gui.h"

#include "../material.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"
#include "../screen.h"

#include "../buffers/frameBuffer.h"

#include "../mesh/indexedMesh.h"
#include "../mesh/primitive.h"

#include "../engine/math/cframe.h"
#include "../engine/math/vec4.h"
#include "../engine/math/mathUtil.h"


DirectionEditor::DirectionEditor(double x, double y, double width, double height) : Component(x, y, width, height) {
	viewPosition = Position(0.0, 0., -3.0);
	viewMatrix = Mat4f().translate(TEMP_CAST_POSITION_TO_VEC(viewPosition));
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
		GUI::vectorMesh->render();

		GUI::guiFrameBuffer->unbind();
		Renderer::disableDepthTest();

		GUI::quad->resize(position, dimension);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::COLOR::BACK, blendColor));
		GUI::quad->render(Renderer::WIREFRAME);

		Vec2 contentPosition = position + Vec2(GUI::padding, -GUI::padding);
		Vec2 contentDimension = dimension - Vec2(GUI::padding) * 2;
		GUI::quad->resize(contentPosition, contentDimension);
		Shaders::quadShader.updateTexture(GUI::guiFrameBuffer->texture, blendColor);
		GUI::quad->render();
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
