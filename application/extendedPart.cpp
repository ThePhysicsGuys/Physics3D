#include "core.h"

#include "extendedPart.h"
#include "../physics/misc/serialization.h"

ExtendedPart::ExtendedPart(Part&& part, int drawMeshId) : visualShape(part.hitbox.asPolyhedron()), Part(std::move(part)), drawMeshId(drawMeshId), name("Part") {}

ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, int drawMeshId, std::string name) :
	Part(hitbox, position, properties), visualShape(hitbox.asPolyhedron()), drawMeshId(drawMeshId), name("Part") {}
ExtendedPart::ExtendedPart(const VisualShape& shape, const GlobalCFrame& position, const PartProperties& properties, int drawMeshId, std::string name) :
	Part(static_cast<const VisualShape&>(shape), position, properties), visualShape(shape), drawMeshId(drawMeshId), name("Part") {}
ExtendedPart::ExtendedPart(const Shape& hitbox, const VisualShape& shape, const GlobalCFrame& position, const PartProperties& properties, int drawMeshId, std::string name) :
	Part(hitbox, position, properties), visualShape(shape), drawMeshId(drawMeshId), name(name) {}

void ExtendedPart::scale(double scaleX, double scaleY, double scaleZ) {
	Part::scale(scaleX, scaleY, scaleZ);

	visualScale = DiagonalMat3f{ float(scaleX), float(scaleY), float(scaleZ) } * visualScale;
}

void ExtendedPart::serializeCore(std::ostream& ostream) const {
	Part::serializeCore(ostream);

	::serialize(drawMeshId, ostream);
}
ExtendedPart ExtendedPart::deserialize(std::istream& istream) {
	Part p = Part::deserialize(istream);
	int drawMeshId = ::deserialize<int>(istream);
	return ExtendedPart(std::move(p), drawMeshId);
}
