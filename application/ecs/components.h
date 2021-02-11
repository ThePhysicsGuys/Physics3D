#pragma once

#include <string>
#include <variant>
#include "extendedPart.h"

#include "../physics/geometry/builtinShapeClasses.h"
#include "../graphics/visualData.h"

namespace P3D::Application {

namespace Comp {

struct Hitbox : RefCountable {
	std::variant<Shape, ExtendedPart*> hitbox;

	Hitbox() : hitbox(Shape(&CubeClass::instance)) {}
	Hitbox(ExtendedPart* part) : hitbox(part) {}
	Hitbox(const ShapeClass* shapeClass) : hitbox(Shape(shapeClass)) {}
	Hitbox(const Shape& shape) : hitbox(shape) {}

	Shape getShape() {
		if (std::holds_alternative<Shape>(this->hitbox)) {
			return std::get<Shape>(hitbox);
		} else {
			return std::get<ExtendedPart*>(this->hitbox)->hitbox;
		}
	}

	bool isPartAttached() {
		return std::holds_alternative<ExtendedPart*>(this->hitbox);
	}

	ExtendedPart* getPart() {
		return std::get<ExtendedPart*>(this->hitbox);
	}

	DiagonalMat3 getScale() {
		return this->getShape().scale;
	}

	void setScale(const DiagonalMat3& scale) {
		if (std::holds_alternative<ExtendedPart*>(this->hitbox)) {
			std::get<ExtendedPart*>(this->hitbox)->setScale(scale);
		} else {
			std::get<Shape>(this->hitbox).scale = scale;
		}
	}
};

struct Transform : RefCountable {
	struct ScaledCFrame {
		GlobalCFrame cframe;
		DiagonalMat3 scale;

		ScaledCFrame() : scale(DiagonalMat3::IDENTITY()) {}
		ScaledCFrame(const Position& position) : cframe(position), scale(DiagonalMat3::IDENTITY()) {}
		ScaledCFrame(const GlobalCFrame& cframe) : cframe(cframe), scale(DiagonalMat3::IDENTITY()) {}
		ScaledCFrame(const GlobalCFrame& cframe, const DiagonalMat3& scale) : cframe(cframe), scale(scale) {}
	};
	
	std::variant<ScaledCFrame, ExtendedPart*> cframe;

	Transform() : cframe(ScaledCFrame()) {}
	Transform(ExtendedPart* part) : cframe(part) {}
	Transform(const Position& position) : cframe(position) {}
	Transform(const GlobalCFrame& cframe) : cframe(cframe) {}
	Transform(const GlobalCFrame& cframe, const DiagonalMat3& scale) : cframe(ScaledCFrame(cframe, scale)) {}

	bool isPartAttached() {
		return std::holds_alternative<ExtendedPart*>(this->cframe);
	}

	void setPart(ExtendedPart* part) {
		this->cframe = part;
	}
	
	ExtendedPart* getPart() {
		return std::get<ExtendedPart*>(this->cframe);
	}
	
	void setCFrame(const GlobalCFrame& cframe) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->setCFrame(cframe);
		} else {
			std::get<ScaledCFrame>(this->cframe).cframe = cframe;
		}
	}

	void setScale(const DiagonalMat3& scale) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->setScale(scale);
		} else {
			std::get<ScaledCFrame>(this->cframe).scale = scale;
		}
	}

	void setPosition(const Position& position) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			ExtendedPart* part = std::get<ExtendedPart*>(this->cframe);
			GlobalCFrame cframe = part->getCFrame();
			cframe.position = position;
			
			part->setCFrame(cframe);
		} else {
			std::get<ScaledCFrame>(this->cframe).cframe.position = position;
		}
	}

	void setRotation(const Rotation& rotation) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			ExtendedPart* part = std::get<ExtendedPart*>(this->cframe);
			GlobalCFrame cframe = part->getCFrame();
			cframe.rotation = rotation;

			part->setCFrame(cframe);
		} else {
			std::get<ScaledCFrame>(this->cframe).cframe.rotation = rotation;
		}
	}
	
	GlobalCFrame getCFrame() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->getCFrame();
		} else {
			return std::get<ScaledCFrame>(this->cframe).cframe;
		}
	}

	DiagonalMat3 getScale() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->hitbox.scale;
		} else {
			return std::get<ScaledCFrame>(this->cframe).scale;
		}
	}

	Position getPosition() {
		return getCFrame().getPosition();
	}

	Rotation getRotation() {
		return getCFrame().getRotation();
	}

	Mat4f getModelMatrix() {
		return getCFrame().asMat4WithPreScale(getScale());
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

// The collider of the entity, as it is being physicsed in the engine 
struct Collider : RefCountable {
	ExtendedPart* part;

	Collider(ExtendedPart* part) : part(part) {}

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