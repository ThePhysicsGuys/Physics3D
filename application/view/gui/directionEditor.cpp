#include "directionEditor.h"

#include "../shaderProgram.h"
#include "../renderUtils.h"

#include "../engine/math/cframe.h"

DirectionEditor::DirectionEditor(double x, double y, double width, double height) : Component(x, y, width, height) {
	viewPosition = Vec3f(0, 0, -2);
	viewMatrix = Mat4f().translate(-viewPosition);
}

DirectionEditor::DirectionEditor(double x, double y) : Component(x, y) {

}

void DirectionEditor::render() {
	GUI::guiFrameBuffer->resize(dimension);

	// Draw onto gui framebuffer
	GUI::guiFrameBuffer->bind();
	Renderer::clearColor();
	Renderer::clearDepth();
	//Renderer::enableDepthTest();
	   
	Mat4f modelMatrix = Mat4f();
	Shaders::basicShader.updateMaterial(Material(GUI::COLOR::G));
	Shaders::basicShader.updateModel(modelMatrix);
	Shaders::basicShader.updateProjection(viewMatrix, GUI::screen->camera.projectionMatrix, viewPosition);
	GUI::vectorMesh->render();

	//Renderer::disableDepthTest();
	GUI::guiFrameBuffer->unbind();

	GUI::quad->resize(position, dimension);
	Shaders::quadShader.updateTexture(GUI::guiFrameBuffer->texture, Vec4(0.8, 0.8, 0.8, 0.8));
	GUI::quad->render();
}

Vec2 DirectionEditor::resize() {
	return dimension;
}

void DirectionEditor::press(Vec2 point) {

}

void DirectionEditor::release(Vec2 point) {

}

void DirectionEditor::drag(Vec2 dxy) {

}
