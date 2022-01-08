#pragma once

#include <string>
#include <variant>

#include <Physics3D/geometry/builtinShapeClasses.h>
#include <Physics3D/threading/upgradeableMutex.h>
#include <Physics3D/geometry/shapeCreation.h>
#include "Physics3D/softlinks/elasticLink.h"
#include "Physics3D/softlinks/magneticLink.h"
#include "Physics3D/softlinks/springLink.h"
#include "../application/extendedPart.h"
#include "../graphics/visualData.h"
#include "../worlds.h"
#include "Physics3D/softlinks/alignmentLink.h"

namespace P3D::Application {

	namespace Comp {

		struct Hitbox : RC {
			std::variant<Shape, ExtendedPart*> hitbox;

			Hitbox() : hitbox(boxShape(2.0, 2.0, 2.0)) {}
			Hitbox(ExtendedPart* part) : hitbox(part) {}
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

				ScaledCFrame(const Position& position)
					: cframe(position)
					, scale(DiagonalMat3::IDENTITY()) {}

				ScaledCFrame(const Position& position, double scale)
					: cframe(position)
					, scale(DiagonalMat3::DIAGONAL(scale)) {}

				ScaledCFrame(const GlobalCFrame& cframe)
					: cframe(cframe)
					, scale(DiagonalMat3::IDENTITY()) {}

				ScaledCFrame(const GlobalCFrame& cframe, double scale)
					: cframe(cframe)
					, scale(DiagonalMat3::DIAGONAL(scale)) {}

				ScaledCFrame(const GlobalCFrame& cframe, const DiagonalMat3& scale)
					: cframe(cframe)
					, scale(scale) {}
			};

			std::variant<ScaledCFrame, ExtendedPart*, CFrame*> cframe;

			Transform() : cframe(ScaledCFrame()) {}
			Transform(ExtendedPart* part) : cframe(part) {}
			Transform(CFrame* cframe) : cframe(cframe) {}
			Transform(const Position& position) : cframe(position) {}
			Transform(const Position& position, double scale) : cframe(ScaledCFrame(position, scale)) {}
			Transform(const GlobalCFrame& cframe) : cframe(cframe) {}
			Transform(const GlobalCFrame& cframe, double scale) : cframe(ScaledCFrame(cframe, scale)) {}
			Transform(const GlobalCFrame& cframe, const DiagonalMat3& scale) : cframe(ScaledCFrame(cframe, scale)) {}

			bool isPartAttached() {
				return std::holds_alternative<ExtendedPart*>(this->cframe);
			}

			bool isCFrameAttached() {
				return std::holds_alternative<CFrame*>(this->cframe);
			}

			template <typename Function>
			void asyncModify(PlayerWorld* world, UpgradeableMutex& worldMutex, const Function& function) {
				if (isPartAttached() || isCFrameAttached()) {
					std::unique_lock<UpgradeableMutex> (worldMutex);
					function();
				} else
					function();
			}

			template <typename Function>
			void syncRead(PlayerWorld* world, UpgradeableMutex& worldMutex, const Function& function) {
				if (isPartAttached() || isCFrameAttached()) {
					std::unique_lock<UpgradeableMutex> (worldMutex);
					function();
				} else
					function();
			}

			void setPart(ExtendedPart* part) {
				this->cframe = part;
			}

			void setCFrame(CFrame* cframe) {
				this->cframe = cframe;
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
				if (isPartAttached()) {
					return std::get<ExtendedPart*>(this->cframe)->getCFrame();
				} else if (isCFrameAttached()) {
					CFrame* relativeFrame = std::get<CFrame*>(this->cframe);
					return GlobalCFrame(castVec3ToPosition(relativeFrame->position), relativeFrame->rotation);
				} else {
					return std::get<ScaledCFrame>(this->cframe).cframe;
				}
			}

			void setCFrame(const GlobalCFrame& cframe) {
				if (isPartAttached()) {
					std::get<ExtendedPart*>(this->cframe)->setCFrame(cframe);
				} else if (isCFrameAttached()) {
					CFrame* relativeFrame = std::get<CFrame*>(this->cframe);
					relativeFrame->position = castPositionToVec3(cframe.position);
					relativeFrame->rotation = cframe.rotation;
				} else {
					std::get<ScaledCFrame>(this->cframe).cframe = cframe;
				}
			}

			Position getPosition() {
				return getCFrame().getPosition();
			}

			void setPosition(const Position& position) {
				if (isPartAttached()) {
					ExtendedPart* part = std::get<ExtendedPart*>(this->cframe);
					GlobalCFrame cframe = part->getCFrame();
					cframe.position = position;

					part->setCFrame(cframe);
				} else if (isCFrameAttached()) {
					std::get<CFrame*>(this->cframe)->position = castPositionToVec3(position);
				} else {
					std::get<ScaledCFrame>(this->cframe).cframe.position = position;
				}
			}

			Rotation getRotation() {
				return getCFrame().getRotation();
			}

			void setRotation(const Rotation& rotation) {
				if (isPartAttached()) {
					ExtendedPart* part = std::get<ExtendedPart*>(this->cframe);
					GlobalCFrame cframe = part->getCFrame();
					cframe.rotation = rotation;

					part->setCFrame(cframe);
				} else if (isCFrameAttached()) {
					std::get<CFrame*>(this->cframe)->rotation = rotation;
				} else {
					std::get<ScaledCFrame>(this->cframe).cframe.rotation = rotation;
				}
			}

			DiagonalMat3 getScale() {
				if (isPartAttached()) {
					return std::get<ExtendedPart*>(this->cframe)->hitbox.scale;
				} else if (isCFrameAttached()) {
					return DiagonalMat3::IDENTITY();
				} else {
					return std::get<ScaledCFrame>(this->cframe).scale;
				}
			}

			void setScale(const DiagonalMat3& scale) {
				if (isPartAttached()) {
					std::get<ExtendedPart*>(this->cframe)->setScale(scale);
				} else if (isCFrameAttached()) { } else {
					std::get<ScaledCFrame>(this->cframe).scale = scale;
				}
			}

			double getWidth() {
				if (isPartAttached()) {
					return std::get<ExtendedPart*>(this->cframe)->hitbox.getWidth();
				} else if (isCFrameAttached()) {
					return 0;
				} else {
					return std::get<ScaledCFrame>(this->cframe).scale[0] * 2;
				}
			}

			void setWidth(double width) {
				if (isPartAttached()) {
					std::get<ExtendedPart*>(this->cframe)->hitbox.setWidth(width);
				} else if (isCFrameAttached()) { } else {
					std::get<ScaledCFrame>(this->cframe).scale[0] = width / 2;
				}
			}

			double getHeight() {
				if (isPartAttached()) {
					return std::get<ExtendedPart*>(this->cframe)->hitbox.getHeight();
				} else if (isCFrameAttached()) {
					return 0;
				} else {
					return std::get<ScaledCFrame>(this->cframe).scale[1] * 2;
				}
			}

			void setHeight(double height) {
				if (isPartAttached()) {
					std::get<ExtendedPart*>(this->cframe)->hitbox.setHeight(height);
				} else if (isCFrameAttached()) { } else {
					std::get<ScaledCFrame>(this->cframe).scale[1] = height / 2;
				}
			}

			double getDepth() {
				if (isPartAttached()) {
					return std::get<ExtendedPart*>(this->cframe)->hitbox.getDepth();
				} else if (isCFrameAttached()) {
					return 0;
				} else {
					return std::get<ScaledCFrame>(this->cframe).scale[2] * 2;
				}
			}

			void setDepth(double depth) {
				if (isPartAttached()) {
					std::get<ExtendedPart*>(this->cframe)->hitbox.setDepth(depth);
				} else if (isCFrameAttached()) { } else {
					std::get<ScaledCFrame>(this->cframe).scale[2] = depth / 2;
				}
			}

			double getMaxRadius() {
				if (isPartAttached()) {
					return std::get<ExtendedPart*>(this->cframe)->hitbox.getMaxRadius();
				} else if (isCFrameAttached()) {
					return 0;
				} else {
					DiagonalMat3 scale = std::get<ScaledCFrame>(this->cframe).scale;
					return length(Vec3{scale[0], scale[1], scale[2]});
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

			Mesh(const VisualData& data)
				: mode(data.mode)
				, id(data.id)
				, hasUVs(data.hasUVs)
				, hasNormals(data.hasNormals) {}

			Mesh(int mode, int id, bool hasUVs, bool hasNormals)
				: mode(mode)
				, id(id)
				, hasUVs(hasUVs)
				, hasNormals(hasNormals) {}

			Mesh(int id, bool hasUVs, bool hasNormals)
				: id(id)
				, hasUVs(hasUVs)
				, hasNormals(hasNormals) {}
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

			Light(const Graphics::Color& color, float intensity, const Attenuation& attenuation)
				: color(color)
				, intensity(intensity)
				, attenuation(attenuation) {}
		};

		struct Attachment : public RC {
			AttachedPart* attachment;

			Attachment(AttachedPart* attachment) : attachment(attachment) {}
		};

		struct SoftLink : public RC {
			P3D::SoftLink* link;

			SoftLink(P3D::SoftLink* link) : link(link) {}

			void setPositionA(const Vec3& position) {
				link->attachedPartA.attachment.position = position;
			}

			void setPositionB(const Vec3& position) {
				link->attachedPartB.attachment.position = position;
			}

			Vec3 getPositionA() const {
				return link->attachedPartA.attachment.position;
			}

			Vec3 getPositionB() const {
				return link->attachedPartB.attachment.position;
			}
		};

		struct MagneticLink : public SoftLink {
			MagneticLink(P3D::MagneticLink* link) : SoftLink(link) {}
		};

		struct SpringLink : public SoftLink {
			SpringLink(P3D::SpringLink* link) : SoftLink(link) {}
		};

		struct ElasticLink : public SoftLink {
			ElasticLink(P3D::ElasticLink* link) : SoftLink(link) {}
		};

		struct AlignmentLink : public SoftLink {
			AlignmentLink(P3D::AlignmentLink* link) : SoftLink(link) {}
		};

		struct HardConstraint : public RC {
			HardPhysicalConnection* hardConstraint;

			HardConstraint(HardPhysicalConnection* hardConstraint) : hardConstraint(hardConstraint) {}

			CFrame* getChildAttachment() {
				return &hardConstraint->attachOnChild;
			}

			CFrame* getParentAttachment() {
				return &hardConstraint->attachOnParent;
			}
		};

		struct FixedConstraint : public HardConstraint {
			FixedConstraint(HardPhysicalConnection* connection) : HardConstraint(connection) {}
		};
	}

};
