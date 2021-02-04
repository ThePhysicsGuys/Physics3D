#pragma once

#include <string>
#include "extendedPart.h"
#include "../graphics/visualData.h"

namespace P3D::Application {

namespace Comp {

struct Transform : RefCountable {
	union {
		ExtendedPart* part;
		GlobalCFrame* cframe;
	};

	bool isPartAttached;

	explicit Transform(ExtendedPart* part) : part(part), isPartAttached(true) {}
	
	template<typename... Args>
	explicit Transform(Args&&... args) : isPartAttached(false) {
		this->cframe = new GlobalCFrame(std::forward<Args>(args)...);
	}

	~Transform() {
		if (!isPartAttached) {
			delete cframe;
		}
	}
	
	void setCFrame(const GlobalCFrame& cframe) {
		if (isPartAttached) {
			part->setCFrame(cframe);
		} else {
			*this->cframe = cframe;
		}
	}

	GlobalCFrame getCFrame() {
		if (isPartAttached) {
			return part->getCFrame();
		} else {
			return *this->cframe;
		}
	}
};

// The name of an entity
struct Name : RefCountable {
	std::string name;

	Name(const std::string& name) : name(name) {}

	void setName(const std::string& name) {
		this->name = name;
	}
};

// The model of the entity, as it is being physicsed in the engine 
struct Model : RefCountable {
	ExtendedPart* part;

	Model(ExtendedPart* part) : part(part) {}

	ExtendedPart* operator->() const {
		return part;
	}
};

// The mesh of an entity, as it is rendered
struct Mesh : public RefCountable {
	// The render mode, default is fill
	int mode = 0x1B02;

	// The mesh id in the mesh registry
	int id;

	// Whether the mesh has UV coordinates
	bool hasUVs;

	// Whether the mesh has normal vectors
	bool hasNormals;

	Mesh(const VisualData& data) : mode(data.mode), id(data.id), hasUVs(data.hasUVs), hasNormals(data.hasNormals) {}
	Mesh(int mode, int id, bool hasUVs, bool hasNormals) : mode(mode), id(id), hasUVs(hasUVs), hasNormals(hasNormals) {}
	Mesh(int id, bool hasUVs, bool hasNormals) : id(id), hasUVs(hasUVs), hasNormals(hasNormals) {}
};

struct Light : public RefCountable {
	struct Attenuation {
		float constant;
		float linear;
		float exponent;
	};
	
	Color3 color;
	float intensity;
	Attenuation attenuation;

	Light(const Color3& color, float intensity, const Attenuation& attenuation) : color(color), intensity(intensity), attenuation(attenuation) {}
};

	
}

};