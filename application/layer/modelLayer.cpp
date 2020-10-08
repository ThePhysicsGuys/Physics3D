#include "core.h"

#include "modelLayer.h"

#include "view/screen.h"
#include "shader/shaders.h"
#include "extendedPart.h"
#include "worlds.h"

#include "../engine/ecs/registry.h"
#include "ecs/components.h"
#include "ecs/light.h"
#include "ecs/material.h"
#include "ecs/model.h"
#include "ecs/mesh.h"

#include "../engine/meshRegistry.h"

#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/meshLibrary.h"
#include "../graphics/debug/visualDebug.h"

#include "../graphics/path/path3D.h"
#include "../graphics/batch/batch.h"
#include "../graphics/batch/batchConfig.h"
#include "../graphics/resource/textureResource.h"
#include "../graphics/gui/color.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/sharedLockGuard.h"
#include "../physics/misc/filters/visibilityFilter.h"
#include "../physics/math/mathUtil.h"

#include "../util/resource/resourceManager.h"
#include "../layer/shadowLayer.h"

namespace P3D::Application {

// Light uniforms
std::vector<Light*> lights;

enum class RelationToSelectedPart {
	NONE,
	SELF,
	DIRECT_ATTACH,
	MAINPART,
	PHYSICAL_ATTACH,
	MAINPHYSICAL_ATTACH
};

static RelationToSelectedPart getRelationToSelectedPart(const Part* selectedPart, const Part* testPart) {
	if (selectedPart == nullptr)
		return RelationToSelectedPart::NONE;

	if (testPart == selectedPart)
		return RelationToSelectedPart::SELF;

	if (selectedPart->parent != nullptr && testPart->parent != nullptr) {
		if (testPart->parent == selectedPart->parent) {
			if (testPart->isMainPart())
				return RelationToSelectedPart::MAINPART;
			else
				return RelationToSelectedPart::DIRECT_ATTACH;
		} else if (testPart->parent->mainPhysical == selectedPart->parent->mainPhysical) {
			if (testPart->parent->isMainPhysical())
				return RelationToSelectedPart::MAINPHYSICAL_ATTACH;
			else
				return RelationToSelectedPart::PHYSICAL_ATTACH;
		}
	}

	return RelationToSelectedPart::NONE;
}

static Color getAmbientForPartForSelected(Screen* screen, Part* part) {
	switch (getRelationToSelectedPart(screen->selectedPart, part)) {
		case RelationToSelectedPart::NONE:
			return Color(0.0f, 0, 0, 0);
		case RelationToSelectedPart::SELF:
			return Color(0.5f, 0, 0, 0);
		case RelationToSelectedPart::DIRECT_ATTACH:
			return Color(0, 0.25f, 0, 0);
		case RelationToSelectedPart::MAINPART:
			return Color(0, 1.0f, 0, 0);
		case RelationToSelectedPart::PHYSICAL_ATTACH:
			return Color(0, 0, 0.25f, 0);
		case RelationToSelectedPart::MAINPHYSICAL_ATTACH:
			return Color(0, 0, 1.0f, 0);
	}

	return Color(0, 0, 0, 0);
}

static Color getAlbedoForPart(Screen* screen, Part* part) {
	Color computedAmbient = getAmbientForPartForSelected(screen, part);
	if (part == screen->intersectedPart)
		computedAmbient += Vec4f(-0.1f, -0.1f, -0.1f, 0);

	return computedAmbient;
}

void ModelLayer::onInit(Engine::Registry64& registry) {
	using namespace Graphics;
	Screen* screen = static_cast<Screen*>(this->ptr);

	Attenuation attenuation = { 1, 1, 1 };
	lights.push_back(new Light(Vec3f(10, 5, -10), Color3(1, 0.84f, 0.69f), 300, attenuation));
	lights.push_back(new Light(Vec3f(10, 5, 10), Color3(1, 0.84f, 0.69f), 300, attenuation));
	lights.push_back(new Light(Vec3f(-10, 5, -10), Color3(1, 0.84f, 0.69f), 200, attenuation));
	lights.push_back(new Light(Vec3f(-10, 5, 10), Color3(1, 0.84f, 0.69f), 500, attenuation));
	lights.push_back(new Light(Vec3f(0, 5, 0), Color3(1, 0.90f, 0.75f), 400, attenuation));

	Shaders::basicShader.updateLight(lights);
	Shaders::basicShader.updateTexture(false);
	Shaders::instanceShader.updateLight(lights);
	Shaders::instanceShader.updateTexture(false);
}

void ModelLayer::onUpdate(Engine::Registry64& registry) {

}

void ModelLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}

void ModelLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	graphicsMeasure.mark(GraphicsProcess::UPDATE);
	Shaders::debugShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	//Shaders::instanceShader.updateProjection(lookAt(fromPosition(screen->camera.cframe.position), Vec3f(0, 0, 0)), screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::instanceShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	// Shadow
	Vec3f from = { -10, 10, -10 };
	Vec3f to = { 0, 0, 0 };
	Vec3f sunDirection = to - from;
	ShadowLayer::lightView = lookAt(from, to);
	ShadowLayer::lighSpaceMatrix = ShadowLayer::lightProjection * ShadowLayer::lightView;

	Renderer::activeTexture(1);
	Renderer::bindTexture2D(ShadowLayer::depthMap);
	Shaders::instanceShader.setUniform("shadowMap", 1);
	Shaders::instanceShader.setUniform("lightMatrix", ShadowLayer::lighSpaceMatrix);
	Shaders::instanceShader.updateSunDirection(sunDirection);

	// Filter on mesh ID and transparency
	size_t maxMeshCount = 0;
	std::map<int, size_t> meshCounter;
	std::multimap<int, ExtendedPart*> visibleParts;
	std::map<double, ExtendedPart*> transparentParts;
	graphicsMeasure.mark(GraphicsProcess::PHYSICALS);
	screen->world->syncReadOnlyOperation([this, &visibleParts, &transparentParts, &meshCounter, &maxMeshCount, screen, &registry] () {
		VisibilityFilter filter = VisibilityFilter::forWindow(screen->camera.cframe.position, screen->camera.getForwardDirection(), screen->camera.getUpDirection(), screen->camera.fov, screen->camera.aspect, screen->camera.zfar);

		auto view = registry.view<Comp::Model>();
		for (auto& entity : view) {
			Ref<Comp::Model> model = view.get<Comp::Model>(entity);
			if (!filter(*model->part))
				continue;	
			
			Ref<Comp::Mesh> mesh = registry.get<Comp::Mesh>(entity);
			if (!mesh.valid())
				continue;

			Comp::Material material = registry.getOr<Comp::Material>(entity, Comp::Material());
			if (material.albedo.w < 1.0f) {
				double distance = lengthSquared(Vec3(screen->camera.cframe.position - model->part->getPosition()));
				transparentParts.insert(std::make_pair(distance , model->part));
			} else {
				visibleParts.insert(std::make_pair(mesh->id, model->part));
				maxMeshCount = std::max(maxMeshCount, ++meshCounter[mesh->id]);
			}
			
		}

		// Ensure correct size
		uniforms.reserve(maxMeshCount);
		while (uniforms.size() < maxMeshCount)
			uniforms.push_back(Uniform());

		// Render normal meshes
		Shaders::instanceShader.bind();
		for (const auto& [id, count] : meshCounter) {
			if (id == -1) 
				continue;

			// Collect uniforms
			int offset = 0;
			auto meshes = visibleParts.equal_range(id);
			for (auto mesh = meshes.first; mesh != meshes.second; ++mesh) {
				ExtendedPart* part = mesh->second;

				Mat4f modelMatrix = part->getCFrame().asMat4WithPreScale(part->hitbox.scale);
				Comp::Material material = registry.getOr<Comp::Material>(part->entity, Comp::Material());
				material.albedo += getAlbedoForPart(screen, part);

				uniforms[offset] = Uniform {
					modelMatrix,
					material.albedo,
					material.metalness,
					material.roughness,
					material.ao
				};

				offset++;
			}

			Engine::MeshRegistry::meshes[id]->fillUniformBuffer(uniforms.data(), count * sizeof(Uniform), Renderer::STREAM_DRAW);
			Engine::MeshRegistry::meshes[id]->renderInstanced(count);
		}

		// Render transparent meshes
		Shaders::basicShader.bind();
		Renderer::enableBlending();
		for (auto iterator = transparentParts.rbegin(); iterator != transparentParts.rend(); ++iterator) {
			ExtendedPart* part = iterator->second;

			Ref<Comp::Mesh> mesh = registry.get<Comp::Mesh>(part->entity);
			if (!mesh.valid())
				continue;

			if (mesh->id == -1)
				continue;

			Comp::Material material = registry.getOr<Comp::Material>(part->entity, Comp::Material());
			material.albedo += getAlbedoForPart(screen, part);

			Shaders::basicShader.updateMaterial(material);
			Shaders::basicShader.updatePart(*part);
			Engine::MeshRegistry::meshes[mesh->id]->render(mesh->mode);

		}

		if (screen->selectedPart) {
			Shaders::debugShader.updateModel(screen->selectedPart->getCFrame().asMat4WithPreScale(screen->selectedPart->hitbox.scale));
			Ref<Comp::Mesh> mesh = registry.get<Comp::Mesh>(screen->selectedPart->entity);
			if (mesh.valid())
				Engine::MeshRegistry::meshes[mesh->id]->render();
		}
	});

	endScene();
}

void ModelLayer::onClose(Engine::Registry64& registry) {

}

};