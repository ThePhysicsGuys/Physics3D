#pragma once

class Screen;
struct Camera;

#include "extendedPart.h"
#include "../engine/math/vec3.h"

typedef void (*CameraMoveHandler) (Screen&, Camera*, Vec3);
typedef void (*WindowResizeHandler) (Screen&, unsigned int, unsigned int);
typedef void (*PartRayIntersectHandler) (Screen&, ExtendedPart*, Vec3);
typedef void (*PartDragHandler) (Screen&, ExtendedPart*, Vec3);
typedef void (*PartClickHandler) (Screen&, ExtendedPart*, Vec3);
typedef void (*PartTouchHandler) (ExtendedPart*, ExtendedPart*, Vec3);
typedef void (*PartReleaseHandler) (ExtendedPart*, ExtendedPart*);

class EventHandler {
private:
	void* pointer;

public:
	CameraMoveHandler cameraMoveHandler = [] (Screen&, Camera*, Vec3) {};
	WindowResizeHandler windowResizeHandler = [] (Screen&, unsigned int, unsigned int) {};
	PartRayIntersectHandler partRayIntersectHandler = [] (Screen&, ExtendedPart*, Vec3) {};
	PartDragHandler partDragHandler = [] (Screen&, ExtendedPart*, Vec3) {};
	PartClickHandler partClickHandler = [] (Screen&, ExtendedPart*, Vec3) {};
	PartTouchHandler partTouchHandler = [] (ExtendedPart*, ExtendedPart*, Vec3) {};
	PartReleaseHandler partReleaseHandler = [] (ExtendedPart*, ExtendedPart*) {};
	
	void* getPtr() const;
	void setPtr(void* ptr);
	void setCameraMoveCallback(CameraMoveHandler handler);
	void setWindowResizeCallback(WindowResizeHandler handler);
	void setPartRayIntersectCallback(PartRayIntersectHandler handler);
	void setPartDragCallback(PartDragHandler handler);
	void setPartClickCallback(PartClickHandler handler);
	void setPartTouchCallback(PartTouchHandler handler);
	void setPartReleaseCallback(PartReleaseHandler handler);
};
