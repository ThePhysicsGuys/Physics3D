#include "core.h"

#include "basicShader.h"

#include "ecs/material.h"
#include "extendedPart.h"

namespace Application {

void BasicShader::updatePart(const ExtendedPart& part) {
	bind();
	BasicShader::updateTexture(false);
	BasicShader::updateModel(part.getCFrame(), DiagonalMat3f(part.hitbox.scale));
}

void BasicShader::updateMaterial(const Material& material) {
	bind();
	setUniform("material.albedo", material.albedo);
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

void InstanceShader::updateTexture(bool textured) {
	bind();
	setUniform("material.albedoMap", 0);
	setUniform("material.normalMap", 1);
	setUniform("material.metalnessMap", 2);
	setUniform("material.roughnessMap", 3);
	setUniform("material.ambientOcclusionMap", 4);
	setUniform("material.textured", textured);
}

};