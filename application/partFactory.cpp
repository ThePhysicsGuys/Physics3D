#include "core.h"

#include "partFactory.h"

PartFactory::PartFactory(const Shape& hitbox, const VisualShape& visualShape, Screen& screen, std::string name) : 
	hitbox(hitbox), name(name), count(0), visualShape(visualShape), drawMeshID(screen.addMeshShape(visualShape)) {}

ExtendedPart* PartFactory::produce(const GlobalCFrame& cframe, const PartProperties& properties, std::string name) const {
	count++;
	GlobalCFrame realCFrame = cframe.localToGlobal(CFrame(backTransform));
	return new ExtendedPart(hitbox, visualShape, realCFrame, properties, drawMeshID, (name.empty()) ? this->name + " " + std::to_string(count) : name);
}
ExtendedPart* PartFactory::produce(const PartProperties& properties, std::string name) const {
	return this->produce(GlobalCFrame(), properties, name);
}
ExtendedPart* PartFactory::produce(ExtendedPart* partToAttachTo, const CFrame& attachment, const PartProperties& properties, std::string name) const {
	ExtendedPart* part = this->produce(GlobalCFrame(), properties, name);
	partToAttachTo->attach(*part, attachment);
	return part;
}

ExtendedPart* PartFactory::produceScaled(const GlobalCFrame& cframe, const PartProperties& properties, double scaleX, double scaleY, double scaleZ, std::string name) const {
	count++;
	GlobalCFrame realCFrame = cframe.localToGlobal(CFrame(backTransform));
	ExtendedPart* p = new ExtendedPart(hitbox, visualShape, realCFrame, properties, drawMeshID, (name.empty()) ? this->name + " " + std::to_string(count) : name);
	p->scale(scaleX, scaleY, scaleZ);
	return p;
}
ExtendedPart* PartFactory::produceScaled(const PartProperties& properties, double scaleX, double scaleY, double scaleZ, std::string name) const {
	return produceScaled(GlobalCFrame(), properties, scaleX, scaleY, scaleZ, name);
}
ExtendedPart* PartFactory::produceScaled(ExtendedPart* partToAttachTo, const CFrame& attachment, const PartProperties& properties, double scaleX, double scaleY, double scaleZ, std::string name) const {
	ExtendedPart* part = this->produceScaled(GlobalCFrame(), properties, scaleX, scaleY, scaleZ, name);
	partToAttachTo->attach(*part, attachment);
	return part;
}

ExtendedPart* createUniquePart(Screen& screen, const Shape& hitbox, const VisualShape& visualShape, const GlobalCFrame& position, const PartProperties& properties, std::string name) {
	ExtendedPart* partPtr = new ExtendedPart(hitbox, visualShape, position, properties, 0, name);
	partPtr->drawMeshId = screen.addMeshShape(visualShape);
	return partPtr;
}
