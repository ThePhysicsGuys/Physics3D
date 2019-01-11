#pragma once

class Screen;

#include "../engine/physical.h"
#include "../engine/math/vec3.h"
#include "../util/log.h"

typedef void (*PhysicalRayIntersectHandler) (Screen*, Physical*, Vec3);
typedef void (*PhysicalDragHandler) (Screen*, Physical*, Vec3);
typedef void (*PhysicalClickHandler) (Screen*, Physical*, Vec3);
typedef void (*PhysicalTouchHandler) (Physical*, Physical*, Vec3);
typedef void (*PhysicalReleaseHandler) (Physical*, Physical*);

class EventHandler {
private:
	void* pointer;

public:
	PhysicalRayIntersectHandler physicalRayIntersectHandler = [] (Screen*, Physical*, Vec3) {};
	PhysicalDragHandler physicalDragHandler = [] (Screen*, Physical*, Vec3) {};
	PhysicalClickHandler physicalClickHandler = [] (Screen*, Physical*, Vec3) {};
	PhysicalTouchHandler physicalTouchHandler = [] (Physical*, Physical*, Vec3) {};
	PhysicalReleaseHandler physicalReleaseHandler = [] (Physical*, Physical*) {};
	
	void* getPtr() const;
	void setPtr(void* ptr);
	void setPhysicalRayIntersectCallback(PhysicalRayIntersectHandler handler);
	void setPhysicalDragCallback(PhysicalDragHandler handler);
	void setPhysicalClickCallback(PhysicalClickHandler handler);
	void setPhysicalTouchCallback(PhysicalTouchHandler handler);
	void setPhysicalReleaseCallback(PhysicalReleaseHandler handler);
};

#include "gui/screen.h"
