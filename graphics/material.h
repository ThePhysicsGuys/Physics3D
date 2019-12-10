#pragma once

#include "gui/color.h"

class Texture;

struct Material {
	Texture* texture = nullptr;
	Texture* normal = nullptr;

	Color ambient;
	Color3 diffuse;
	Color3 specular;
	float reflectance;

	Material(Color ambient, Color3 diffuse, Color3 specular, float reflectance, Texture* texture, Texture* normalMap) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance), texture(texture), normal(normalMap) {};
	Material(Color ambient, Color3 diffuse, Color3 specular, float reflectance, Texture* texture) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance), texture(texture) {};
	Material(Color ambient, Color3 diffuse, Color3 specular, float reflectance) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance) {};
	Material(Color ambient) : Material(ambient, Color3(1.0f), Color3(1.0f), 1.0f) {};
	Material(Texture* texture) : Material(Color(1.0), Color3(1.0), Color3(1.0f), 1.0f, texture) {};
	Material() : Material(Color(1.0f), Color3(1.0f), Color3(1.0f), 1.0f) {};

	bool operator==(const Material& other) const {
		return 
			other.ambient == ambient &&
			other.diffuse == diffuse &&
			other.specular == specular &&
			other.reflectance == reflectance &&
			other.texture == texture &&
			other.normal == normal;
	}

	void setTexture(Texture* texture);
	void setNormalMap(Texture* normalMap);
};