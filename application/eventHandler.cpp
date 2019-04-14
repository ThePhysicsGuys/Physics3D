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

void EventHandler::setPartRayIntersectCallback(PartRayIntersectHandler handler) {
	partRayIntersectHandler = handler;
}

void EventHandler::setPartDragCallback(PartDragHandler handler) {
	partDragHandler = handler;
}

void EventHandler::setPartClickCallback(PartClickHandler handler) {
	partClickHandler = handler;
}

void EventHandler::setPartTouchCallback(PartTouchHandler handler) {
	partTouchHandler = handler;
}

void EventHandler::setPartReleaseCallback(PartReleaseHandler handler) {
	partReleaseHandler = handler;
}