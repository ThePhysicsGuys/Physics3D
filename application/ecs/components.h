#pragma once

#include <string>
#include <variant>

#include <Physics3D/threading/synchonizedWorld.h>
#include "../application/extendedPart.h"
#include <Physics3D/geometry/builtinShapeClasses.h>
#include "../graphics/visualData.h"

namespace P3D::Application {

namespace Comp {

struct Hitbox : RC {
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

struct Transform : RC {
	struct ScaledCFrame {
		GlobalCFrame cframe;
		DiagonalMat3 scale;

		ScaledCFrame() : scale(DiagonalMat3::IDENTITY()) {}
		ScaledCFrame(const Position& position) : cframe(position), scale(DiagonalMat3::IDENTITY()) {}
		ScaledCFrame(const Position& position, double scale) : cframe(position), scale(DiagonalMat3::DIAGONAL(scale)) {}
		ScaledCFrame(const GlobalCFrame& cframe) : cframe(cframe), scale(DiagonalMat3::IDENTITY()) {}
		ScaledCFrame(const GlobalCFrame& cframe, double scale) : cframe(cframe), scale(DiagonalMat3::DIAGONAL(scale)) {}
		ScaledCFrame(const GlobalCFrame& cframe, const DiagonalMat3& scale) : cframe(cframe), scale(scale) {}
	};
	
	std::variant<ScaledCFrame, ExtendedPart*> cframe;

	Transform() : cframe(ScaledCFrame()) {}
	Transform(ExtendedPart* part) : cframe(part) {}
	Transform(const Position& position) : cframe(position) {}
	Transform(const Position& position, double scale) : cframe(ScaledCFrame(position, scale)) {}
	Transform(const GlobalCFrame& cframe) : cframe(cframe) {}
	Transform(const GlobalCFrame& cframe, double scale) : cframe(ScaledCFrame(cframe, scale)) {}
	Transform(const GlobalCFrame& cframe, const DiagonalMat3& scale) : cframe(ScaledCFrame(cframe, scale)) {}

	bool isPartAttached() {
		return std::holds_alternative<ExtendedPart*>(this->cframe);
	}

	template<typename Function>
	void asyncModify(SynchronizedWorld<ExtendedPart>* world, const Function& function) {
		if (isPartAttached()) 
			world->asyncModification(function);
		else
			function();
	}

	template<typename Function>
	void syncRead(SynchronizedWorld<ExtendedPart>* world, const Function& function) {
		if (isPartAttached())
			world->syncReadOnlyOperation(function);
		else
			function();
	}

	void setPart(ExtendedPart* part) {
		this->cframe = part;
	}
	
	ExtendedPart* getPart() {
		return std::get<ExtendedPart*>(this->cframe);
	}

	void translate(const Vec3& translation) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->translate(translation);
		} else {
			std::get<ScaledCFrame>(this->cframe).cframe.position += translation;
		}
	}

	void rotate(const Rotation& rotation) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			ExtendedPart* part = std::get<ExtendedPart*>(this->cframe);
			part->setCFrame(part->getCFrame().rotated(rotation));
		} else {
			std::get<ScaledCFrame>(this->cframe).cframe.rotate(rotation);
		}
	}

	void scale(double scaleX, double scaleY, double scaleZ) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->scale(scaleX, scaleY, scaleZ);
		} else {
			std::get<ScaledCFrame>(this->cframe).scale[0] *= scaleX;
			std::get<ScaledCFrame>(this->cframe).scale[1] *= scaleY;
			std::get<ScaledCFrame>(this->cframe).scale[2] *= scaleZ;
		}
	}
	
	GlobalCFrame getCFrame() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->getCFrame();
		} else {
			return std::get<ScaledCFrame>(this->cframe).cframe;
		}
	}

	void setCFrame(const GlobalCFrame& cframe) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->setCFrame(cframe);
		} else {
			std::get<ScaledCFrame>(this->cframe).cframe = cframe;
		}
	}

	Position getPosition() {
		return getCFrame().getPosition();
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

	Rotation getRotation() {
		return getCFrame().getRotation();
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

	DiagonalMat3 getScale() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->hitbox.scale;
		} else {
			return std::get<ScaledCFrame>(this->cframe).scale;
		}
	}

	void setScale(const DiagonalMat3& scale) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->setScale(scale);
		} else {
			std::get<ScaledCFrame>(this->cframe).scale = scale;
		}
	}

	double getWidth() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->hitbox.getWidth();
		} else {
			return std::get<ScaledCFrame>(this->cframe).scale[0];
		}
	}

	void setWidth(double width) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->hitbox.setWidth(width);
		} else {
			std::get<ScaledCFrame>(this->cframe).scale[0] = width;
		}
	}

	double getHeight() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->hitbox.getHeight();
		} else {
			return std::get<ScaledCFrame>(this->cframe).scale[1];
		}
	}
	
	void setHeight(double height) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->hitbox.setHeight(height);
		} else {
			std::get<ScaledCFrame>(this->cframe).scale[1] = height;
		}
	}

	double getDepth() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->hitbox.getDepth();
		} else {
			return std::get<ScaledCFrame>(this->cframe).scale[2];
		}
	}

	void setDepth(double depth) {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			std::get<ExtendedPart*>(this->cframe)->hitbox.setDepth(depth);
		} else {
			std::get<ScaledCFrame>(this->cframe).scale[2] = depth;
		}
	}

	double getMaxRadius() {
		if (std::holds_alternative<ExtendedPart*>(this->cframe)) {
			return std::get<ExtendedPart*>(this->cframe)->hitbox.getMaxRadius();
		} else {
			DiagonalMat3 scale = std::get<ScaledCFrame>(this->cframe).scale;
			return length(Vec3 { scale[0], scale[1], scale[2] });
		}
	}

	Mat4f getModelMatrix(bool scaled = true) {
		if (scaled)
			return getCFrame().asMat4WithPreScale(getScale());
		else
			return getCFrame().asMat4();
	}
};

// The name of an entity
struct Name : RC {
	std::string name;

	Name(const std::string& name) : name(name) {}

	void setName(const std::string& name) {
		this->name = name;
	}
};

// The collider of the entity, as it is being physicsed in the engine 
struct Collider : RC {
	ExtendedPart* part;

	Collider(ExtendedPart* part) : part(part) {}

	ExtendedPart* operator->() const {
		return part;
	}
};

// The mesh of an entity, as it is rendered
struct Mesh : public RC {
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

struct Light : public RC {
	struct Attenuation {
		float constant;
		float linear;
		float exponent;
	};

	Graphics::Color color;
	float intensity;
	Attenuation attenuation;

	Light(const Graphics::Color& color, float intensity, const Attenuation& attenuation) : color(color), intensity(intensity), attenuation(attenuation) {}
};

	
}

};