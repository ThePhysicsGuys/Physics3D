#include "core.h"

#include "modelLayer.h"

#include "view/screen.h"
#include "shader/shaders.h"
#include "extendedPart.h"
#include "worlds.h"

#include "../engine/ecs/registry.h"
#include "ecs/components.h"
#include "ecs/material.h"

#include "../graphics/meshRegistry.h"

#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/debug/visualDebug.h"

#include "../graphics/gui/color.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/sharedLockGuard.h"
#include "../physics/misc/filters/visibilityFilter.h"

#include "../util/resource/resourceManager.h"
#include "../layer/shadowLayer.h"

#include "../graphics/batch/instanceBatchManager.h"

namespace P3D::Application {

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

static Color getAlbedoForPart(Screen* screen, ExtendedPart* part) {
	Color computedAmbient = getAmbientForPartForSelected(screen, part);

	if (part->entity == screen->intersectedEntity)
		computedAmbient += Vec4f(-0.1f, -0.1f, -0.1f, 0);

	return computedAmbient;
}

void ModelLayer::onInit(Engine::Registry64& registry) {
	using namespace Graphics;
	Screen* screen = static_cast<Screen*>(this->ptr);

	// Lights
	auto lights = registry.create();
	registry.add<Comp::Name>(lights, "Lights");

	auto light1 = registry.create(lights);
	auto light2 = registry.create(lights);
	auto light3 = registry.create(lights);
	auto light4 = registry.create(lights);
	auto light5 = registry.create(lights);

	Comp::Light::Attenuation attenuation = { 1, 1, 1 };
	
	registry.add<Comp::Light>(light1, Color3(1, 0.84f, 0.69f), 300, attenuation);
	registry.add<Comp::Light>(light2, Color3(1, 0.84f, 0.69f), 300, attenuation);
	registry.add<Comp::Light>(light3, Color3(1, 0.84f, 0.69f), 200, attenuation);
	registry.add<Comp::Light>(light4, Color3(1, 0.84f, 0.69f), 500, attenuation);
	registry.add<Comp::Light>(light5, Color3(1, 0.90f, 0.75f), 400, attenuation);
	
	registry.add<Comp::Transform>(light1, Position(10, 5, -10));
	registry.add<Comp::Transform>(light2, Position(10, 5, 10));
	registry.add<Comp::Transform>(light3, Position(-10, 5, -10));
	registry.add<Comp::Transform>(light4, Position(-10, 5, 10));
	registry.add<Comp::Transform>(light5, Position(0, 5, 0));

	// Dont know anymore
	Shaders::basicShader.updateTexture(false);
	Shaders::instanceShader.updateTexture(false);

	// Instance batch manager
	manager = new InstanceBatchManager<Uniform>(DEFAULT_UNIFORM_BUFFER_LAYOUT);
}

void ModelLayer::onUpdate(Engine::Registry64& registry) {
	auto view = registry.view<Comp::Light>();

	std::size_t index = 0;
	for (auto entity : view) {
		Ref<Comp::Light> light = view.get<Comp::Light>(entity);

		Position position;
		Ref<Comp::Transform> transform = registry.get<Comp::Transform>(entity);
		if (transform.valid()) 
			position = transform->getCFrame().getPosition();
		
		Shaders::basicShader.updateLight(index, position , *light);
		Shaders::instanceShader.updateLight(index, position, *light);
		
		index += 1;
	}

	Shaders::basicShader.updateLightCount(index);
	Shaders::instanceShader.updateLightCount(index);
}

void ModelLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}
		
void ModelLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	graphicsMeasure.mark(UPDATE);
	Shaders::debugShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::basicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	Shaders::instanceShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	// Shadow
	Vec3f from = { -10, 10, -10 };
	Vec3f to = { 0, 0, 0 };
	Vec3f sunDirection = to - from;
	ShadowLayer::lightView = lookAt(from, to);
	ShadowLayer::lighSpaceMatrix = ShadowLayer::lightProjection * ShadowLayer::lightView;
	activeTexture(1);
	bindTexture2D(ShadowLayer::depthMap);
	Shaders::instanceShader.setUniform("shadowMap", 1);
	Shaders::instanceShader.setUniform("lightMatrix", ShadowLayer::lighSpaceMatrix);
	Shaders::instanceShader.updateSunDirection(sunDirection);

	// Filter on mesh ID and transparency
	std::map<double, ExtendedPart*> transparentParts;
	graphicsMeasure.mark(PHYSICALS);
	screen->world->syncReadOnlyOperation([this, &transparentParts, screen, &registry] () {
		VisibilityFilter filter = VisibilityFilter::forWindow(screen->camera.cframe.position, screen->camera.getForwardDirection(), screen->camera.getUpDirection(), screen->camera.fov, screen->camera.aspect, screen->camera.zfar);

		auto view = registry.view<Comp::Model>();
		for (auto entity : view) {
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
				Mat4f modelMatrix = model->part->getCFrame().asMat4WithPreScale(model->part->hitbox.scale);
				material.albedo += getAlbedoForPart(screen, model->part);

				Uniform uniform {
					modelMatrix,
					material.albedo,
					material.metalness,
					material.roughness,
					material.ao
				};
				
				manager->add(mesh->id, uniform);
			}
		}

		Shaders::instanceShader.bind();
		manager->submit();
		

		// Render transparent meshes
		Shaders::basicShader.bind();
		enableBlending();
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
			MeshRegistry::meshes[mesh->id]->render(mesh->mode);
		}

		if (screen->selectedPart) {
			Shaders::debugShader.updateModel(screen->selectedPart->getCFrame().asMat4WithPreScale(screen->selectedPart->hitbox.scale));
			Ref<Comp::Mesh> mesh = registry.get<Comp::Mesh>(screen->selectedPart->entity);
			if (mesh.valid())
				MeshRegistry::meshes[mesh->id]->render();
		}
	});

	endScene();
}

void ModelLayer::onClose(Engine::Registry64& registry) {

}

};