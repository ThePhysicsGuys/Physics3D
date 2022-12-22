#include "magnetForce.h"

#include "../world.h"

namespace P3D {

MagnetForce::MagnetForce(double pickerStrength, double pickerSpeedStrength) : 
	selectedPart(nullptr),
	localSelectedPoint(),
	magnetPoint(),
	pickerStrength(pickerStrength),
	pickerSpeedStrength(pickerSpeedStrength) {}

MagnetForce::MagnetForce(Part& selectedPart, Vec3 localSelectedPoint, Position magnetPoint, double pickerStrength, double pickerSpeedStrength) :
	selectedPart(&selectedPart),
	localSelectedPoint(localSelectedPoint),
	magnetPoint(magnetPoint),
	pickerStrength(pickerStrength),
	pickerSpeedStrength(pickerSpeedStrength) {}

void MagnetForce::apply(WorldPrototype* world) {
	if(selectedPart != nullptr) {
		Physical* selectedPartPhys = selectedPart->getPhysical();
		if(selectedPartPhys != nullptr) {
			MotorizedPhysical* selectedPhysical = selectedPartPhys->mainPhysical;

			// Magnet force
			Position absoluteSelectedPoint = selectedPart->getCFrame().localToGlobal(localSelectedPoint);
			Position centerOfmass = selectedPhysical->getCenterOfMass();
			Vec3 delta = magnetPoint - absoluteSelectedPoint;
			Vec3 relativeSelectedPointSpeed = selectedPart->getMotion().getVelocityOfPoint(absoluteSelectedPoint - centerOfmass);
			Vec3 force = selectedPhysical->totalMass * (delta * pickerStrength - relativeSelectedPointSpeed * pickerSpeedStrength);

			selectedPhysical->applyForceToPhysical(absoluteSelectedPoint - centerOfmass, force);
			selectedPhysical->motionOfCenterOfMass.rotation.rotation[0] *= 0.8;
		}
	}
}
};

