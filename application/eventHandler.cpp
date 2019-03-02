#include "eventHandler.h"

#include "gui\screen.h"

void* EventHandler::getPtr() const {
	return pointer;
}

void EventHandler::setPtr(void* ptr) {
	pointer = ptr;
}

void EventHandler::setCameraMoveCallback(CameraMoveHandler handler) {
	cameraMoveHandler = handler;
}

void EventHandler::setWindowResizeCallback(WindowResizeHandler handler) {
	windowResizeHandler = handler;
}

void EventHandler::setPhysicalRayIntersectCallback(PhysicalRayIntersectHandler handler) {
	physicalRayIntersectHandler = handler;
}

void EventHandler::setPhysicalDragCallback(PhysicalDragHandler handler) {
	physicalDragHandler = handler;
}

void EventHandler::setPhysicalClickCallback(PhysicalClickHandler handler) {
	physicalClickHandler = handler;
}

void EventHandler::setPhysicalTouchCallback(PhysicalTouchHandler handler) {
	physicalTouchHandler = handler;
}

void EventHandler::setPhysicalReleaseCallback(PhysicalReleaseHandler handler) {
	physicalReleaseHandler = handler;
}