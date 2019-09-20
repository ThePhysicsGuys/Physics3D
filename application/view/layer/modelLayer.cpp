#include "core.h"

#include "modelLayer.h"

#include "../screen.h"
#include "../shaderProgram.h"
#include "../light.h"

#include "../mesh/indexedMesh.h"

#include "../../worlds.h"
#include "../../extendedPart.h"

#include "../debug/visualDebug.h"

#include "../../meshLibrary.h"
#include "../engine/sharedLockGuard.h"


// Light uniforms
Vec3f sunDirection;
Vec3f sunColor;
Attenuation attenuation = { 0, 0, 0.5 };

const int lightCount = 5;
Light lights[lightCount] = {
	Light(Vec3f(10, 5, -10), Vec3f(1, 0.84f, 0.69f), 6, attenuation),
	Light(Vec3f(10, 5, 10), Vec3f(1, 0.84f, 0.69f), 6, attenuation),
	Light(Vec3f(-10, 5, -10), Vec3f(1, 0.84f, 0.69f), 6, attenuation),
	Light(Vec3f(-10, 5, 10), Vec3f(1, 0.84f, 0.69f), 6, attenuation),
	Light(Vec3f(0, 10, 0), Vec3f(1, 0.90f, 0.75f), 10, attenuation)
};

ModelLayer::ModelLayer() : Layer() {

}

ModelLayer::ModelLayer(Screen* screen, char flags) : Layer("Model", screen, flags) {

}

void ModelLayer::onInit() {
	Shaders::basicShader.createLightArray(lightCount);
}

void ModelLayer::onUpdate() {
	// Update lights
	/*static long long t = 0;
	float d = 0.5 + 0.5 * sin(t++ * 0.005);
	sunDirection = Vec3f(0, cos(t * 0.005) , sin(t * 0.005));
	lights[0].color = Vec3f(d, 0.3, 1-d);
	lights[1].color = Vec3f(1-d, 0.3, 1 - d);
	lights[2].color = Vec3f(0.2, 0.3*d, 1 - d);
	lights[3].color = Vec3f(1-d, 1-d, d);*/
}

void ModelLayer::onEvent(Event& event) {

}

void ModelLayer::onRender() {
	graphicsMeasure.mark(GraphicsProcess::PHYSICALS);

	SharedLockGuard lg(screen->world->lock);
	Shaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	std::map<double, ExtendedPart*> transparentMeshes;

	// Bind basic uniforms
	Shaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::basicShader.updateLight(lights, lightCount);
	Shaders::maskShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);

	// Render world objects
	for (ExtendedPart& part : *screen->world) {
		int meshId = part.drawMeshId;

		Material material = part.material;

		// Picker code
		if (&part == screen->intersectedPart)
			material.ambient = part.material.ambient + Vec4f(-0.1f, -0.1f, -0.1f, 0);
		else
			material.ambient = part.material.ambient;

		if (material.ambient.w < 1) {
			transparentMeshes[lengthSquared(Vec3(screen->camera.cframe.position - part.getPosition()))] = &part;
			continue;
		}

		Shaders::basicShader.updateMaterial(material);

		// Render each physical
		Shaders::basicShader.updatePart(part);

		if (meshId == -1) continue;

		/*if (&part == selectedPart) {
			maskFrameBuffer->bind();
			Renderer::clearDepth();
			Renderer::clearColor();
			Shaders::maskShader.updateModel(CFrameToMat4(part.cframe));
			meshes[meshId]->render();
			Shaders::edgeShader.updateTexture(maskFrameBuffer->texture);
			quad->render();
			image->texture = maskFrameBuffer->texture;
			modelFrameBuffer->bind();
		} else {*/
			screen->meshes[meshId]->render(part.renderMode);
		//}
	}

	for (auto iterator = transparentMeshes.rbegin(); iterator != transparentMeshes.rend(); ++iterator) {
		ExtendedPart* part = (*iterator).second;

		Material material = part->material;

		if (part == screen->selectedPart)
			material.ambient = part->material.ambient + Vec4f(0.1f, 0.1f, 0.1f, -0.2f);
		else if (part == screen->intersectedPart)
			material.ambient = part->material.ambient + Vec4f(-0.1f, -0.1f, -0.1f, 0);
		else
			material.ambient = part->material.ambient;

		Shaders::basicShader.updateMaterial(material);

		// Render each physical
		Shaders::basicShader.updatePart(*part);

		if (part->drawMeshId == -1) continue;

		screen->meshes[part->drawMeshId]->render(part->renderMode);
	}

	// Render lights
	graphicsMeasure.mark(GraphicsProcess::LIGHTING);
	for (Light light : lights) {
		Mat4f transformation = scale(translate(Matrix<float, 4, 4>::IDENTITY(), light.position), 0.1f);
		Shaders::basicShader.updateMaterial(Material(Vec4f(light.color, 1), Vec3f(), Vec3f(), 10));
		Shaders::basicShader.updateModel(transformation);
		Library::sphere->render();
	}
}

void ModelLayer::onClose() {

}
