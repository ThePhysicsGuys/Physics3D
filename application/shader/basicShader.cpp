#include "core.h"

#include "basicShader.h"

#include "../graphics/ecs/components.h"
#include "extendedPart.h"

namespace P3D::Application {

void BasicShader::updatePart(const ExtendedPart& part) {
	bind();
	updateTexture(false);
	updateModel(part.getCFrame(), DiagonalMat3f(part.hitbox.scale));
}

void BasicShader::updateMaterial(const Graphics::Comp::Material& material) {
	bind();
	setUniform("material.albedo", Vec4f(material.albedo));
	setUniform("material.metalness", material.metalness);
	setUniform("material.roughness", material.roughness);
	setUniform("material.ambientOcclusion", material.ao);
}

void BasicShader::updateTexture(bool textured) {
	bind();
	setUniform("material.albedoMap", 0);
	setUniform("material.normalMap", 1);
	setUniform("material.metalnessMap", 2);
	setUniform("material.roughnessMap", 3);
	setUniform("material.ambientOcclusionMap", 4);
	setUniform("material.textured", textured);
}

};