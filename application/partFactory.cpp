#include "core.h"

#include "partFactory.h"

PartFactory::PartFactory(const Shape& hitbox, const VisualShape& visualShape, Screen& screen, std::string name) : 
	hitbox(hitbox), name(name), count(0), visualShape(visualShape), drawMeshID(screen.addMeshShape(visualShape)) {}

ExtendedPart* PartFactory::produce(const GlobalCFrame& cframe, double density, double friction, std::string name) const {
	count++;
	GlobalCFrame realCFrame = cframe.localToGlobal(CFrame(backTransform));
	return new ExtendedPart(hitbox, visualShape, realCFrame, density, friction, drawMeshID, (name.empty()) ? this->name + " " + std::to_string(count) : name);
} 

ExtendedPart* PartFactory::produceScaled(const GlobalCFrame& cframe, double density, double friction, double scaleX, double scaleY, double scaleZ, std::string name) const {
	count++;
	GlobalCFrame realCFrame = cframe.localToGlobal(CFrame(backTransform));
	ExtendedPart* p = new ExtendedPart(hitbox, visualShape, realCFrame, density, friction, drawMeshID, (name.empty()) ? this->name + " " + std::to_string(count) : name);
	p->scale(scaleX, scaleY, scaleZ);
	return p;
}

ExtendedPart* createUniquePart(Screen& screen, const Shape& hitbox, const VisualShape& visualShape, const GlobalCFrame& position, double density, double friction, std::string name) {
	ExtendedPart* partPtr = new ExtendedPart(hitbox, visualShape, position, density, friction, 0, name);
	partPtr->drawMeshId = screen.addMeshShape(visualShape);
	return partPtr;
}
