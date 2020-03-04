#pragma once

#include "../graphics/gui/color.h"
#include "../engine/ecs/component.h"

namespace Graphics {
class Texture;
};

namespace Application {

struct Material : public Engine::Component {
	DEFINE_COMPONENT(Material, true);

	Graphics::Texture* texture = nullptr;
	Graphics::Texture* normal = nullptr;

	Color ambient;
	Color3 diffuse;
	Color3 specular;
	float reflectance;

	Material(Color ambient, Color3 diffuse, Color3 specular, float reflectance, Graphics::Texture* texture, Graphics::Texture* normalMap) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance), texture(texture), normal(normalMap) {};
	Material(Color ambient, Color3 diffuse, Color3 specular, float reflectance, Graphics::Texture* texture) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance), texture(texture) {};
	Material(Color ambient, Color3 diffuse, Color3 specular, float reflectance) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance) {};
	Material(Color ambient) : Material(ambient, Color3(1.0f), Color3(1.0f), 1.0f) {};
	Material(Graphics::Texture* texture) : Material(Color(1.0), Color3(1.0), Color3(1.0f), 1.0f, texture) {};
	Material() : Material(Color(1.0f), Color3(1.0f), Color3(1.0f), 1.0f) {};

	bool operator==(const Material& other) const;
	void setTexture(Graphics::Texture* texture);
	void setNormalMap(Graphics::Texture* normalMap);
};

};