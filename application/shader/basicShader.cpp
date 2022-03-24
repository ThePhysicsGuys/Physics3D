#include "core.h"

#include "basicShader.h"

#include "../graphics/ecs/components.h"
#include "extendedPart.h"

namespace P3D::Application {

void BasicShader::updatePart(const ExtendedPart& part) {
	bind();
	updateModel(part.getCFrame(), DiagonalMat3f(part.hitbox.scale));
}

void BasicShader::updateMaterial(const Graphics::Comp::Material& material) {
	bind();
	setUniform("uAlbedo", Vec4f(material.albedo));
	setUniform("uRoughness", material.roughness);
	setUniform("uMetalness", material.metalness);
	setUniform("uAmbientOcclusion", material.ao);
}

};