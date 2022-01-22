#pragma once

#include <string>
#include <variant>
#include <functional>

#include <Physics3D/geometry/builtinShapeClasses.h>
#include <Physics3D/threading/upgradeableMutex.h>
#include <Physics3D/geometry/shapeCreation.h>
#include "Physics3D/softlinks/elasticLink.h"
#include "Physics3D/softlinks/magneticLink.h"
#include "Physics3D/softlinks/springLink.h"
#include "../application/extendedPart.h"
#include "../graphics/visualData.h"
#include "../worlds.h"
#include "Physics3D/misc/toString.h"
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

				ScaledCFrame()
					: cframe(GlobalCFrame())
					, scale(DiagonalMat3::IDENTITY()) {}

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

				void translate(const Vec3& translation) {
					this->cframe.translate(translation);
				}

				void rotate(const Rotation& rotation) {
					this->cframe.rotate(rotation);
				}
			};

			std::variant<ExtendedPart*, ScaledCFrame> root;
			std::variant<CFrame, CFrame*> offset;

			std::function<void()> onChange;

			Transform()
				: root(ScaledCFrame())
				, offset(nullptr) {}

			Transform(ExtendedPart* root)
				: root(root)
				, offset(nullptr) {}

			Transform(ExtendedPart* root, CFrame* offset)
				: root(root)
				, offset(offset) {}

			Transform(ExtendedPart* root, const CFrame& offset)
				: root(root)
				, offset(offset) {}

			Transform(const Position& position)
				: root(ScaledCFrame(position))
				, offset(nullptr) {}

			Transform(const Position& position, double scale)
				: root(ScaledCFrame(position, scale))
				, offset(nullptr) {}

			Transform(const GlobalCFrame& cframe)
				: root(ScaledCFrame(cframe))
				, offset(nullptr) {}

			Transform(const GlobalCFrame& cframe, double scale)
				: root(ScaledCFrame(cframe, scale))
				, offset(nullptr) {}

			Transform(const GlobalCFrame& cframe, const DiagonalMat3& scale)
				: root(ScaledCFrame(cframe, scale))
				, offset(nullptr) {}

			bool isRootPart() {
				return std::holds_alternative<ExtendedPart*>(this->root);
			}

			bool isRootCFrame() {
				return std::holds_alternative<ScaledCFrame>(this->root);
			}

			bool hasOffset() {
				return std::holds_alternative<CFrame>(this->offset) || std::holds_alternative<CFrame*>(this->offset) && std::get<CFrame*>(this->offset) != nullptr;
			}

			bool isOffsetStoredLocal() {
				if (!hasOffset())
					return false;

				return std::holds_alternative<CFrame>(this->offset);
			}

			bool isOffsetStoredRemote() {
				if (!hasOffset())
					return false;

				return std::holds_alternative<CFrame*>(this->offset);
			}

			void addCallback(const std::function<void()>& onChange) {
				this->onChange = onChange;
			}

			void setRoot(ExtendedPart* root) {
				this->root = root;
			}

			void setRoot(const ScaledCFrame& root) {
				this->root = root;
			}

			void setOffset(CFrame* offset) {
				this->offset = offset;
			}

			void setOffset(const CFrame& offset) {
				this->offset = offset;
			}

			template <typename Function>
			void modify(UpgradeableMutex& mutex, const Function& function) {
				if (isRootPart() || isOffsetStoredRemote()) {
					mutex.lock();
					function();
					mutex.unlock();
				} else
					function();

				if (onChange != nullptr)
					onChange();
			}

			void translate(const Vec3& translation) {
				if (hasOffset()) {
					Vec3 relativeTranslation = getRootCFrame().relativeToLocal(translation);

					if (isOffsetStoredLocal()) {
						std::get<CFrame>(this->offset).translate(relativeTranslation);
					} else {
						std::get<CFrame*>(this->offset)->translate(relativeTranslation);
					}
				} else {
					if (isRootPart()) {
						std::get<ExtendedPart*>(this->root)->translate(translation);
					} else {
						std::get<ScaledCFrame>(this->root).translate(translation);
					}
				}

				if (onChange != nullptr)
					onChange();
			}

			void rotate(const Vec3& normal, double angle) {
				if (hasOffset()) {
					Vec3 relativeNormal = getRootCFrame().relativeToLocal(normal);
					Rotation relativeRotation = Rotation::fromRotationVector(relativeNormal * angle);

					if (isOffsetStoredLocal()) {
						std::get<CFrame>(this->offset).rotate(relativeRotation);
					} else {
						std::get<CFrame*>(this->offset)->rotate(relativeRotation);
					}
				} else {
					Rotation rotation = Rotation::fromRotationVector(normal * angle);

					if (isRootPart()) {
						ExtendedPart* part = std::get<ExtendedPart*>(this->root);
						part->setCFrame(part->getCFrame().rotated(rotation));
					} else {
						std::get<ScaledCFrame>(this->root).rotate(rotation);
					}
				}

				if (onChange != nullptr)
					onChange();
			}

			void rotate(const Rotation& rotation) {
				if (hasOffset()) {
					Rotation rootRotation = getRootCFrame().getRotation();
					Rotation relativeRotation = ~rootRotation * rotation * rootRotation;

					if (isOffsetStoredLocal()) {
						std::get<CFrame>(this->offset).rotate(relativeRotation);
					} else {
						std::get<CFrame*>(this->offset)->rotate(relativeRotation);
					}
				} else {
					if (isRootPart()) {
						ExtendedPart* part = std::get<ExtendedPart*>(this->root);
						part->setCFrame(part->getCFrame().rotated(rotation));
					} else {
						std::get<ScaledCFrame>(this->root).rotate(rotation);
					}
				}

				if (onChange != nullptr)
					onChange();
			}

			void scale(double scaleX, double scaleY, double scaleZ) {
				if (isRootPart()) {
					std::get<ExtendedPart*>(this->root)->scale(scaleX, scaleY, scaleZ);
				} else {
					std::get<ScaledCFrame>(this->root).scale *= DiagonalMat3({ scaleX, scaleY, scaleZ });
				}

				if (onChange != nullptr)
					onChange();
			}

			GlobalCFrame getRootCFrame() {
				if (isRootPart()) {
					return std::get<ExtendedPart*>(this->root)->getCFrame();
				} else {
					return std::get<ScaledCFrame>(this->root).cframe;
				}
			}

			CFrame getOffsetCFrame() {
				if (hasOffset()) {
					if (isOffsetStoredLocal())
						return std::get<CFrame>(this->offset);
					else
						return *std::get<CFrame*>(this->offset);
				} else {
					return CFrame();
				}
			}

			GlobalCFrame getCFrame() {
				GlobalCFrame rootCFrame = getRootCFrame();
				CFrame offsetCFrame = getOffsetCFrame();

				return rootCFrame.localToGlobal(offsetCFrame);
			}

			void setCFrame(const GlobalCFrame& cframe) {
				if (hasOffset()) {
					CFrame relativeCFrame = getRootCFrame().globalToLocal(cframe);
					if (isOffsetStoredLocal()) {
						std::get<CFrame>(this->offset) = relativeCFrame;
					} else {
						*std::get<CFrame*>(this->offset) = relativeCFrame;
					}
				} else {
					if (isRootPart()) {
						std::get<ExtendedPart*>(this->root)->setCFrame(cframe);
					} else {
						std::get<ScaledCFrame>(this->root).cframe = cframe;
					}
				}

				if (onChange != nullptr)
					onChange();
			}

			Position getPosition() {
				return getCFrame().getPosition();
			}

			void setPosition(const Position& position) {
				GlobalCFrame cframe = getCFrame();
				cframe.position = position;

				setCFrame(cframe);
			}

			Rotation getRotation() {
				return getCFrame().getRotation();
			}

			void setRotation(const Rotation& rotation) {
				GlobalCFrame cframe = getCFrame();
				cframe.rotation = rotation;

				setCFrame(cframe);
			}

			DiagonalMat3 getScale() {
				if (isRootPart()) {
					return std::get<ExtendedPart*>(this->root)->hitbox.scale;
				} else {
					return std::get<ScaledCFrame>(this->root).scale;
				}
			}

			void setScale(const DiagonalMat3& scale) {
				if (isRootPart()) {
					std::get<ExtendedPart*>(this->root)->setScale(scale);
				} else {
					std::get<ScaledCFrame>(this->root).scale = scale;
				}

				if (onChange != nullptr)
					onChange();
			}

			double getWidth() {
				if (isRootPart()) {
					return std::get<ExtendedPart*>(this->root)->hitbox.getWidth();
				} else {
					return std::get<ScaledCFrame>(this->root).scale[0] * 2;
				}
			}

			void setWidth(double width) {
				if (isRootPart()) {
					std::get<ExtendedPart*>(this->root)->hitbox.setWidth(width);
				} else {
					std::get<ScaledCFrame>(this->root).scale[0] = width / 2;
				}

				if (onChange != nullptr)
					onChange();
			}

			double getHeight() {
				if (isRootPart()) {
					return std::get<ExtendedPart*>(this->root)->hitbox.getHeight();
				} else {
					return std::get<ScaledCFrame>(this->root).scale[1] * 2;
				}
			}

			void setHeight(double height) {
				if (isRootPart()) {
					std::get<ExtendedPart*>(this->root)->hitbox.setHeight(height);
				} else {
					std::get<ScaledCFrame>(this->root).scale[1] = height / 2;
				}

				if (onChange != nullptr)
					onChange();
			}

			double getDepth() {
				if (isRootPart()) {
					return std::get<ExtendedPart*>(this->root)->hitbox.getDepth();
				} else {
					return std::get<ScaledCFrame>(this->root).scale[2] * 2;
				}
			}

			void setDepth(double depth) {
				if (isRootPart()) {
					std::get<ExtendedPart*>(this->root)->hitbox.setDepth(depth);
				} else {
					std::get<ScaledCFrame>(this->root).scale[2] = depth / 2;
				}

				if (onChange != nullptr)
					onChange();
			}

			double getMaxRadius() {
				if (isRootPart()) {
					return std::get<ExtendedPart*>(this->root)->hitbox.getMaxRadius();
				} else {
					return length(Vec3 { getWidth(), getHeight(), getDepth() });
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

			// Whether the mesh is visible
			bool visible;

			Mesh(const VisualData& data)
				: mode(data.mode)
				, id(data.id)
				, hasUVs(data.hasUVs)
				, hasNormals(data.hasNormals)
				, visible(true) {}

			Mesh(int mode, int id, bool hasUVs, bool hasNormals)
				: mode(mode)
				, id(id)
				, hasUVs(hasUVs)
				, hasNormals(hasNormals)
				, visible(true) {}

			Mesh(int id, bool hasUVs, bool hasNormals)
				: id(id)
				, hasUVs(hasUVs)
				, hasNormals(hasNormals)
				, visible(true) {}
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
			Part* from;
			Part* to;

			CFrame* attachment;
			bool isAttachmentToMainPart = false;

			Attachment(Part* from, Part* to) : from(from), to(to) {
				this->isAttachmentToMainPart = to->isMainPart();
				this->attachment = &to->parent->rigidBody.getAttachFor(getChildPart()).attachment;
			}

			void setAttachment(const CFrame& cframe) {
				to->parent->rigidBody.setAttachFor(getChildPart(), cframe);
			}

			ExtendedPart* getMainPart() {
				return reinterpret_cast<ExtendedPart*>(isAttachmentToMainPart ? to : from);
			}

			ExtendedPart* getChildPart() {
				return reinterpret_cast<ExtendedPart*>(isAttachmentToMainPart ? from : to);
			}
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
