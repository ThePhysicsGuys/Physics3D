#include "core.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "modelLayer.h"

#include "view/screen.h"
#include "shader/shaders.h"
#include "extendedPart.h"
#include "worlds.h"

#include "../graphics/light.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/meshLibrary.h"
#include "../graphics/debug/visualDebug.h"

#include "../physics/sharedLockGuard.h"
#include "../physics/misc/filters/visibilityFilter.h"

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
	manager = new InstanceBatchManager();
}

void ModelLayer::onInit() {
	ApplicationShaders::basicShader.createLightArray(lightCount);
	ApplicationShaders::instanceBasicShader.createLightArray(lightCount);
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

	std::map<double, ExtendedPart*> transparentMeshes;

	std::vector<ExtendedPart*> meshesToDraw;

	graphicsMeasure.mark(GraphicsProcess::WAIT_FOR_LOCK);
	
	screen->world->syncReadOnlyOperation([this, &meshesToDraw]() {
		graphicsMeasure.mark(GraphicsProcess::UPDATE);

		// Bind basic uniforms
		ApplicationShaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
		ApplicationShaders::basicShader.updateLight(lights, lightCount);
		ApplicationShaders::maskShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);

		graphicsMeasure.mark(GraphicsProcess::PHYSICALS);

		const Camera& camera = screen->camera;

		VisibilityFilter filter = VisibilityFilter::forWindow(camera.cframe.position, camera.getForwardDirection(), camera.getUpDirection(), camera.fov, camera.aspect, camera.zfar);
		// Render world objects
		for (ExtendedPart& part : screen->world->iterPartsFiltered(filter, ALL_PARTS)) {
			meshesToDraw.push_back(&part);
		}
	});

	for(ExtendedPart* part : meshesToDraw) {
		Material material = part->material;

		// Picker code
		if(part == screen->intersectedPart)
			material.ambient = part->material.ambient + Vec4f(-0.1f, -0.1f, -0.1f, 0);
		else
			material.ambient = part->material.ambient;

		if(material.ambient.w < 1) {
			transparentMeshes[lengthSquared(Vec3(screen->camera.cframe.position - part->getPosition()))] = part;
			continue;
		}

		ApplicationShaders::basicShader.updateMaterial(material);
		ApplicationShaders::basicShader.updatePart(*part);

		/*fillUniformBuffer(0, texture, material, CFrameToMat4(part.getCFrame()));
		glBindTexture(GL_TEXTURE_2D, id);
		glActiveTexture(GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 28, 1, 0, GL_RGBA, GL_FLOAT, texture);
		Shaders::basicShader.updateUniforms(id);*/

		int meshId = part->visualData.drawMeshId;

		if(meshId == -1) continue;

		Screen::meshes[meshId]->render(part->renderMode);
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

		ApplicationShaders::basicShader.updateMaterial(material);
		ApplicationShaders::basicShader.updatePart(*part);

		if (part->visualData.drawMeshId == -1) continue;

		Screen::meshes[part->visualData.drawMeshId]->render(part->renderMode);
	}

	// Render lights
	graphicsMeasure.mark(GraphicsProcess::LIGHTING);
	for (Light light : lights) {
		Mat4f transformation = scale(translate(Matrix<float, 4, 4>::IDENTITY(), light.position), 0.1f);
		ApplicationShaders::basicShader.updateMaterial(Material(Vec4f(light.color, 1), Vec3f(), Vec3f(), 10));
		ApplicationShaders::basicShader.updateModel(transformation);
		Library::sphere->render();
	}


	/*int minX = -2;
	int maxX = 2;
	int minY = 0;
	int maxY = 20;
	int minZ = -2;
	int maxZ = 2; 

	for (double x = minX; x < maxX; x += 1.01) {
		for (double y = minY; y < maxY; y += 1.01) {
			for (double z = minZ; z < maxZ; z += 1.01) {
				manager->add(screen->meshes[2], UniformLayout(CFrameToMat4(CFrame(x, y, z)), Vec4f(1), Vec3f(1), Vec3f(1), 1));
			}
		}
	}

	ApplicationShaders::instanceBasicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	ApplicationShaders::instanceBasicShader.updateLight(lights, lightCount);

	//manager->submit();*/
}

void ModelLayer::onClose() {

}
